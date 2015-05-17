#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>

#define BUFSIZE 4096
#define PIDPATH "/var/run/http_pid"
#define NEW 'n'
#define FINISH 'f'
#define EXIT 'e'
#define CONTINUE 'c'
int fd1[2], fd2[2];

const static char http_error_hdr[]="HTTP/1.1 404 Not Found\nServer: HTTP Server\nConnection: close\n";
const static char http_html_hdr[]="HTTP/1.1 200 OK\nServer: HTTP Server\nConnection: close\n";
const static char unhandle_hdr[]="Support GET request only!\n";
const static char http_index_html[]="this is response message to client.\n";


typedef struct {
    pid_t pid;
    char status; 
} REPORT;
typedef struct {
    pid_t pid;
    char status;
} Item;
static Item *q;
static int N, head, tail;
#define ENTER 10
#define ESCAPE 27

void init_curses() 
{ 
	initscr();
	start_color();
	init_pair(1,COLOR_WHITE,COLOR_BLUE); 
	init_pair(2,COLOR_BLUE,COLOR_WHITE);
	init_pair(3,COLOR_RED,COLOR_WHITE); 
	init_pair(4,COLOR_GREEN,COLOR_BLUE);
	curs_set(0); 
	noecho(); 
	keypad(stdscr,TRUE); 
} 
void draw_menubar(WINDOW *menubar)
{
	wbkgd(menubar,COLOR_PAIR(2));
	waddstr(menubar," Start");
	wattron(menubar,COLOR_PAIR(3));
	waddstr(menubar,"(F2)");
	wattroff(menubar,COLOR_PAIR(3));
}
WINDOW **draw_menu(int start_col)
{
	int i;
	WINDOW **items;
	items=(WINDOW **)malloc(3*sizeof(WINDOW *));
	items[0]=newwin(10,19,2,start_col);
	wbkgd(items[0],COLOR_PAIR(2));
	box(items[0],ACS_VLINE,ACS_HLINE);
	items[1]=subwin(items[0],1,17,3,start_col+1);
	items[2]=subwin(items[0],1,17,4,start_col+1);
	wprintw(items[1],"SETTING");
	wprintw(items[2],"EXIT");
	wbkgd(items[1],COLOR_PAIR(2));
	wrefresh(items[0]);
	return items;
}
void delete_menu(WINDOW **items,int count)
{
	int i;
	for (i=0;i<count;i++)
		delwin(items[i]);
	free(items);
}
int scroll_menu(WINDOW **items,int count,int menu_start_col)
{
	int key;
	int selected=0;
	while (1) {
		key=getch();
		if (key==KEY_DOWN || key==KEY_UP) {
			wbkgd(items[selected+1],COLOR_PAIR(2));
			wnoutrefresh(items[selected+1]);
			if (key==KEY_DOWN) {
				selected=(selected+1) % count;
			} 
		else {
		selected=(selected+count-1) % count;
		}
		wbkgd(items[selected+1],COLOR_PAIR(1));
		wnoutrefresh(items[selected+1]);
		doupdate();
		} 
		else if (key==KEY_LEFT || key==KEY_RIGHT) {
			delete_menu(items,count+1);
			touchwin(stdscr);
			refresh();
			items=draw_menu(20-menu_start_col);
			return scroll_menu(items,8,20-menu_start_col);
			} 
			else if (key==ESCAPE) {
				return -1;
			}
			else if (key==ENTER) {
				return selected;
			}
	}
}
char buff[1024];
WINDOW *alertWindow,*querybox;
WINDOW *menubar,*messagebar;
int log_index=1;
void updatedlog(char* str)
{
	if(log_index>7)
	{
	sleep(1);
	werase(alertWindow);
	log_index=1;
	}
	curs_set(0);
    noecho();
	werase(querybox);
	box(alertWindow,0,0);
	wprintw(alertWindow," %s\n",str );
	wrefresh(alertWindow);
	touchwin(stdscr);
	refresh();
	log_index++;
}
int main()
{
	int key,ch;
	int y,x;
	WINDOW *wyes,*wno,*winput;
	init_curses();
	menubar=subwin(stdscr,1,80,1,0);
	messagebar=subwin(stdscr,1,79,23,1);
	draw_menubar(menubar);
	move(20,1);
	printw("Press F2 to open the menus. ");
	printw("ESC quits.");
	box(stdscr,0,0);
	getmaxyx(stdscr,y,x);
        curs_set(0);
        x=x/2-34/2;
        y=y/2-14/2;
	refresh();
	alertWindow = subwin(stdscr,12,60,2,2); 
	wyes = newwin(8,10,y+2,x+1); 
	querybox=subwin(stdscr,8,40,y,x); 
	
	curs_set(1);
    echo();
	box(querybox,0,0);
	mvwaddstr(querybox,2,1,"please input the number of initial process.");
	do {
		int selected_item;
		WINDOW **menu_items;

		key=getch();
		werase(messagebar);
		wrefresh(messagebar);
		if (key==KEY_F(2)) {
			menu_items=draw_menu(1);
			selected_item=scroll_menu(menu_items,2,0);
			delete_menu(menu_items,3);
			if (selected_item<0)
			wprintw(messagebar,"You haven't selected any item.");
			else
			{
				if (selected_item+1==1){
					int nums;
					move(y+2,x+20);
					touchwin(stdscr);
					refresh();
					
				scanw("%d",&nums);
				noecho();
				werase(messagebar);
				wrefresh(messagebar);
				wprintw(messagebar,"now will created  %d processes.",nums);
					
					touchwin(stdscr);
					refresh();
					task(messagebar,nums);

				}

			}


		}


	} while (key!=ESCAPE);

	delwin(alertWindow);
	delwin(menubar);
	delwin(messagebar);
	endwin();
	return 0;
}



void QUEUEinit(int maxN)
{
    q = (Item *)malloc((maxN+1)*sizeof(Item));
    N = maxN+1;
    head = N;
    tail = 0;
}

int QUEUEempty()
{
    return (head%N == tail);
}

void QUEUEput(Item item)
{
    q[tail++] = item;
    tail = tail%N;
}

Item QUEUEget()
{
    head = head %N;
    return q[head++];
}
int handle_http_request(char *path, int sockfd)
{
	if(!strcmp(path, "/"))
	{
    	write(sockfd, http_html_hdr, strlen(http_html_hdr));
    	write(sockfd, http_index_html, strlen(http_index_html));
  	}
  	else
	{
    	write(sockfd, http_error_hdr, strlen(http_error_hdr));
  	}
	close(sockfd);
  return 0;
}


void answer(int listenfd)
{
    int connfd;
    char buf[BUFSIZE];
    int count;
    int pid = getpid();
    struct sockaddr_in cliaddr;
    int size = sizeof(cliaddr);
    char comm;
    REPORT rep;
    rep.pid = pid;
    while (1) 
	  {
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr,(socklen_t *)&size );
        rep.status = NEW;
        if (write(fd1[1], &rep, sizeof(rep)) < 0) 
		{
            perror("write pipe new failed");
            exit(-1);
        }
		bzero(buf,BUFSIZE);
		count = read(connfd, buf, BUFSIZE);
		if(!strncmp(buf, "GET", 3))
		{
			char *path = buf + 4;
			char *space = strchr(path, ' ');
			*space = '\0';
			handle_http_request(path, connfd);
		}
		else
		{
			//printf("Support GET request only!\n");
			write(connfd,unhandle_hdr,strlen(unhandle_hdr));
			close(connfd);
		}
	
		rep.status = FINISH;
		if (write(fd1[1], &rep, sizeof(rep)) < 0) {
			perror("write pipe finish failed");
			exit(-1);
		}
	
		if (read(fd2[0], &comm, 1) < 1) {
			perror("read pipe failed");
			exit(-1);
		}
		if (comm == EXIT) { 
			printf("process [%d] exit\n", pid);
			exit(-1);
		}
		else if (comm == CONTINUE) { 
			//printf("[%d] continue to handle process\n", pid);
			sprintf(buff,"[%d] continue to handle process\n", pid);
   			updatedlog(buff);
		}
		else {
			sprintf(buff,"[%d] comm : %c illeagle\n", pid, comm);
   			updatedlog(buff);
			//printf("[%d] comm : %c illeagle\n", pid, comm);
		}
	}
}
int write_pid()
{
    int fd;
    if ((fd = open(PIDPATH, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR)) < 0)
    {
      perror("open pidfile faild");
        return -1;
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        int err = errno;
        perror("fcntl faild");
        if (err == EAGAIN)
        {
	sprintf(buff,"Another http-serv process is running now!\n");
   	updatedlog(buff);
        //printf("Another http-serv process is running now!\n");
        }
        return -1;
    }
    return 0;
}

int task(WINDOW *messagebars,int PRECHILD)
{
    int     listenfd;
    struct  sockaddr_in servaddr;
	int MAXCHILD;
	int port;
	//PRECHILD=5;
	MAXCHILD=1000;
	port = 8080;
    	pid_t pid;
        updatedlog("This the log window!");
        signal(SIGCHLD, SIG_IGN);
    if (write_pid() < 0)
    {
        return -1;
    }
    if (pipe(fd1) < 0) {
        perror("pipe failed");
        exit(-1);
    }
    if (pipe(fd2) < 0) {
        perror("pipe failed");
        exit(-1);
    }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  listen(listenfd, 1000);
  int i;
  for (i = 0; i < PRECHILD ; i++) {
    if ((pid = fork()) < 0) {
        perror("fork faild");
        exit(3);
    }
    else if (pid == 0) 
	{
        answer(listenfd);
    }
    else 
	{
	sprintf(buff,"have created child process %d\n", pid);
   	updatedlog(buff);
        // printf("have created child process %d\n", pid);
    }
  }
    char e = 'e';
    char c = 'c';
    int req_num = 0;
    int child_num = PRECHILD;
    REPORT rep;
    while (1) {
        if (read(fd1[0], &rep, sizeof(rep)) < sizeof(rep)) {
            perror("parent read pipe failed");
            exit(-1);
        }
        if (rep.status == 'n') {
            req_num ++;
	        
	    sprintf(buff,"parent: %d have receive new request\n", rep.pid);
	    updatedlog(buff);
            //printf("parent: %d have receive new request\n", rep.pid);
            if (req_num >= child_num && child_num <= MAXCHILD) { 
                if ((pid = fork()) < 0) {
                    perror("fork faild");
                    exit(3);
                }
                else if (pid == 0) {
                    answer(listenfd);
                }
                else {
                // printf("have create child %d\n", pid);
		sprintf(buff,"have create child %d\n", pid);
		updatedlog(buff);
                child_num ++;
                }
            }
        }
        else if (rep.status == 'f') {
            req_num --;
            if (child_num > (req_num + 1) && child_num > PRECHILD) {
                if (write(fd2[1], &e, sizeof(e)) < sizeof(e)) {
                    perror("pa write pipe failed");
                    exit(-2);
                }
                child_num --;
            }
            else {
                if (write(fd2[1], &c, sizeof(c)) < sizeof(c)) {
                    perror("pa write pipe failed");
                    exit(-2);
                }
            }
        }
    }
    return 0;
}


