#include <curses.h>
#include "main.h"
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
    waddstr(menubar," File");
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
            } else if (key==ESCAPE) {
                return -1;
            } else if (key==ENTER) {
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
    if(log_index==1)
    {
        werase(alertWindow);
    }
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
    alertWindow = subwin(stdscr,16,80,2,2); 
    wyes = newwin(8,10,y+2,x+1); 
    querybox = subwin(stdscr,8,40,y,x); 
    curs_set(1);
    echo();
    box(querybox,0,0);
    mvwaddstr(querybox,2,1,"please input a num.");
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

typedef struct {
    pid_t pid;
    char status;
} REPORT;

typedef int Item ;
static Item *q;
static int N, head, tail;

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
void freeQueue()
{
    if (q)
    {
       free(q);
    }
}

int task(WINDOW *messagebars,int Zz)
{
    int queuesize = 1000;
   QUEUEinit(queuesize);
    updatedlog("this the log window!");
    int Z=3;                             //size of the pool
    pNODE pool = (pNODE)calloc(Z, sizeof(NODE)) ;
    createchild(pool, Z);
    struct  sockaddr_in servaddr;
    int fd_listen, fd_client ;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5656);
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd_listen, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (Zz>=1000)             //number of socket queue
    {
        Zz=1000;
    }
    listen(fd_listen, Zz);
    fd_set readset, readyset ;
    int index ;
    strcpy(buff,"Init the Queue \n");
    updatedlog(buff);
    int select_ret ;
    struct timeval tm ;
    while(1)
    {
       for(index = 0; index < Z; index ++)
        {
            FD_SET(pool[index].s_sfd, &readset);
        }
        FD_ZERO(&readset);
        FD_ZERO(&readyset);
        FD_SET(fd_listen, &readset);
        tm.tv_sec = 0 ;
        tm.tv_usec = 1000 ;
        readyset = readset ;
        select_ret = select(1024, &readyset, NULL, NULL, &tm);
        if(select_ret == 0)        
        {
            continue ;
        }else if(select_ret == -1) 
        {
                exit(1);
        }else 
        {
            //检查是不是有数据到来。如果是，就接受这个连接
          int index ;
            if(FD_ISSET(fd_listen, &readyset))
           {
                fd_client = accept(fd_listen, NULL, NULL) ;    
                sprintf(buff,"have accept child process %d\n", fd_client);
                updatedlog(buff);
               QUEUEput(fd_client);
                for(index = 0 ; index < Z; index++)
                {
                   if(pool[index].s_state == ST_IDLE)   //free
                    {
                        sprintf(buff,"find a idle child process %d\n",index);
                        updatedlog(buff);
                        write(pool[index].s_sfd, &index, 4);
                       send_fd(pool[index].s_sfd, fd_client); 
                       pool[index].s_state = ST_BUSY ;  //working
                        break ;
                    }
                }
                close(fd_client);//使计数减少1
            }
            int iq;
            for(iq = 0 ; iq < queuesize; iq++)
            {
                if (QUEUEempty())
                    break;
                fd_client=QUEUEget();
               }
            for(index = 0; index < Z; index++)
            {
                if(FD_ISSET(pool[index].s_sfd, &readyset))
                {//判断哪个子进程的句柄可读，此时证明队列中的子进程是空闲的
                    int val ;
                   read(pool[index].s_sfd, &val, 4);
                   pool[index].s_state = ST_IDLE ;//把该队列中的进程状态设置为空闲
               }
            }
        }
    }   
    freeQueue();
}
void createchild(pNODE arr, int cnt)               //fork Z process
{     
    int index ; 
    for(index = 0; index < cnt; index ++)
    {
       pid_t pid ;
        int fds[2] ;//fds[0] - c  fds[1] - p
        socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
         sprintf(buff,"have created child process %d\n", pid);
       pid = fork() ;
       updatedlog(buff);
        if(pid == 0)// child
        {
            close(fds[1]);         
            childloop(fds[0]) ;  
        }else 
        {
            arr[index].s_sfd = fds[1] ;
           arr[index].s_state = ST_IDLE ;
            close(fds[0]);        
        }
    }
}
void childloop(int sfd)
{
    int fd_client;
    int flag ;
    int readn ;
    pid_t pid = getpid();
    while(1)
    {
        readn = read(sfd, &flag, 4);
        sprintf(buff,"have created child process %d\n", flag);//?
        updatedlog(buff);
        recv_fd(sfd, &fd_client);
        handle_request(fd_client);
        write(sfd, &pid, sizeof(pid));
    }
}

/* recv and send */

void st_recv( int *recv_len, int peer_sfd,  void *base, int len)

{

    int recvn;

    int recv_sum = 0;

    while(recv_sum < len)

    {

        recvn = recv(peer_sfd, base + recv_sum, len - recv_sum, 0);

        recv_sum += recvn;

    }

    if(recv_len != NULL)

    {

        *recv_len = recv_sum;

    }

}



void st_send( int *send_len, int peer_sfd, void *base, int len)

{

    int sendn;

    int send_sum = 0;

    while(send_sum < len)

    {

        sendn = send(peer_sfd, base + send_sum, len - send_sum, 0);

        send_sum += sendn;

    }

    if(send_len != NULL)

    {

        *send_len = send_sum;

    }

}

void handle_request(int sfd)

{    



    MSG my_msg ;

    int recvn ;

    while(1)

    {

        memset(&my_msg, 0, sizeof(MSG));

        st_recv(&recvn, sfd, &my_msg, 4);

        if(my_msg.msg_len  == 0)

        {

            break ;

        }

        st_recv(NULL, sfd, my_msg.msg_buf, my_msg.msg_len);

       // 把消息原样返回

        st_send(NULL, sfd, &my_msg, my_msg.msg_len + 4);



    }



}

void send_fd(int sfd, int fd_file) 

{

    struct msghdr my_msg ;

    memset(&my_msg, 0, sizeof(my_msg));

    

    struct iovec bufs[1] ;

    char buf[256] = "OK\n";

    bufs[0].iov_base = buf ;

    bufs[0].iov_len = strlen(buf) ;

    

    my_msg.msg_name = NULL ;

    my_msg.msg_namelen = 0 ;

    my_msg.msg_iov = bufs ;

    my_msg.msg_iovlen = 1 ;

    my_msg.msg_flags = 0 ;



    struct cmsghdr *p  ;

    int cmsg_len = CMSG_LEN(sizeof(int)) ;    

    p = (struct cmsghdr*)calloc(1, cmsg_len) ;

    p -> cmsg_len = cmsg_len ;

    p -> cmsg_level = SOL_SOCKET ;

    p -> cmsg_type = SCM_RIGHTS ;

    *(int*)CMSG_DATA(p) = fd_file ;

    

    my_msg.msg_control = p ;

    my_msg.msg_controllen = cmsg_len ;

    

    sendmsg(sfd, &my_msg, 0);

    

}

void recv_fd(int sfd, int* fd_file) 

{

    struct msghdr msg ;

    

    struct iovec iov[1] ;

    char buf1[256]="" ;

    iov[0].iov_base = buf1 ;

    iov[0].iov_len = 31 ;



    msg.msg_name = NULL ;

    msg.msg_namelen = 0 ;

    msg.msg_iov = iov ;

    msg.msg_iovlen = 2 ;

    msg.msg_flags = 0 ;

    struct cmsghdr *p  ;
    int cmsg_len = CMSG_LEN(sizeof(int)) ;
    p = (struct cmsghdr*)calloc(1, cmsg_len) ;
    msg.msg_control = p ;
    msg.msg_controllen = cmsg_len ;
        int recvn ;
    recvn = recvmsg(sfd, &msg, 0);
    *fd_file = *(int*)CMSG_DATA((struct cmsghdr*)msg.msg_control);  
}

 
