#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 4096
#define PIDPATH "/var/run/http_pid"
#define NEW 'n'
#define FINISH 'f'
#define EXIT 'e'
#define CONTINUE 'c'
int fd1[2], fd2[2];

const static char http_error_hdr[]="HTTP/1.1 404 Not Found\nServer: HTTP Server\nConnection: close\n";
const static char http_html_hdr[]="HTTP/1.1 200 OK\nServer: HTTP Server\nConnection: close\n";
const static char unhandle_hdr[]="HTTP/1.1 503 Service unavailable\n";
const static char http_index_html[]="this is response message to client.\n";


typedef struct{
pid_t pid;
char status;
}REPORT;
int handle_http_request(char *path, int sockfd){
if(!strcmp(path, "/")){
write(sockfd, http_html_hdr, strlen(http_html_hdr));
write(sockfd, http_index_html, strlen(http_index_html));
}
else{
write(sockfd, http_error_hdr, strlen(http_error_hdr));
}
close(sockfd);
return 0;
}


void answer(int listenfd){
int connfd;
char buf[BUFSIZE];
int count;
int pid=getpid();
struct sockaddr_in server;
int size=sizeof(server);
char comm;
REPORT rep;
rep.pid=pid;
while (1){
connfd=accept(listenfd, (struct sockaddr *)&server,(socklen_t *)&size);
rep.status=NEW;
if(write(fd1[1], &rep, sizeof(rep))<0){
perror("write pipe new failed");
exit(-1);
}
bzero(buf,BUFSIZE);
count=read(connfd,buf,BUFSIZE);
if(!strncmp(buf,"GET",3)){	
char *path=buf + 4;
char *space=strchr(path, ' ');
*space='\0';
handle_http_request(path, connfd);
}
else{
printf("Support GET request only!\n");
write(connfd,unhandle_hdr,strlen(unhandle_hdr));
close(connfd);
}
rep.status=FINISH;
if(write(fd1[1], &rep, sizeof(rep)) < 0){
perror("write pipe finish failed");
exit(-1);
}
if(read(fd2[0], &comm, 1) < 1){
perror("read pipe failed");
exit(-1);
}
if(comm == EXIT){
printf("process [%d] exit\n", pid);
exit(-1);
}
else if(comm == CONTINUE){
printf("[%d] continue to handle process\n", pid);
}
else{
printf("[%d] comm : %c illeagle\n", pid, comm);
}
}
}

int write_pid(){
int fd;
if((fd=open(PIDPATH, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR)) < 0){
perror("open pidfile faild");
return -1;
}
struct flock lock;
lock.l_type=F_WRLCK;
lock.l_start=0;
lock.l_whence=SEEK_SET;
lock.l_len=0;
if(fcntl(fd, F_SETLK, &lock)==-1){
int err=errno;
perror("fcntl faild");
if(err==EAGAIN){
printf("Another http-serv process is running now!\n");
}
return -1;
}
return 0;
}

int main(int argc, char **argv){
int listenfd;
struct sockaddr_in servaddr;
int PRECHILD,MAXCHILD;
PRECHILD=5;
MAXCHILD=50;
pid_t pid;
int port;
if(argc != 2){
printf("please enter the listen port:");
scanf("%d",&port); 
printf("please enter the pre-init process count:");
scanf("%d",&PRECHILD); 
printf("please enter the max process count:");
scanf("%d",&MAXCHILD);
if(PRECHILD>MAXCHILD){
perror("pre-init process greater than max count");
exit(-1);
}
}
else{
port=atoi(argv[1]);
}
signal(SIGCHLD, SIG_IGN);
if(write_pid() < 0){
return -1;
}
if(pipe(fd1) < 0){
perror("pipe failed");
exit(-1);
}
if(pipe(fd2) < 0){
perror("pipe failed");
exit(-1);
}
bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family=AF_INET;
servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
servaddr.sin_port=htons(port);
listenfd=socket(AF_INET, SOCK_STREAM, 0);
bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
listen(listenfd,1000);
int i;
for (i=0; i < PRECHILD ; i++){
if((pid=fork()) < 0){
perror("fork faild");
exit(3);
}
else if(pid == 0){
answer(listenfd);
}
else 
{
printf("have created child process %d\n", pid);
}
}
char e='e';
char c='c';
int req_num=0;
int child_num=PRECHILD;
REPORT rep;
while (1){
if(read(fd1[0], &rep, sizeof(rep)) < sizeof(rep)){
perror("parent read pipe failed");
exit(-1);
}
if(rep.status == 'n'){
req_num ++;
printf("parent: %d have receive new request\n", rep.pid);
if(req_num >= child_num && child_num <= MAXCHILD){
if((pid=fork()) < 0){
perror("fork faild");
exit(3);
}
else if(pid == 0){
answer(listenfd);
}
else{
printf("have create child %d\n", pid);
child_num ++;
}
}
}
else if(rep.status == 'f'){
req_num --;
if(child_num > (req_num + 1) && child_num > PRECHILD){
if(write(fd2[1], &e, sizeof(e)) < sizeof(e)){
perror("pa write pipe failed");
exit(-2);
}
child_num --;
}
else{
if(write(fd2[1], &c, sizeof(c)) < sizeof(c)){
perror("pa write pipe failed");
exit(-2);
}
}
}
}
return 0;
}


