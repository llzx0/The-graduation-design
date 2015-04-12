#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
int main()
{
int sock;
struct sockaddr_in server;
char buf[1024];
sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(sock<0){
perror("socket");}
else{printf("sockt created\n");}

server.sin_family=AF_INET;
server.sin_addr.s_addr=INADDR_ANY;
server.sin_port=htons(8888);//?????????????????????????


if(connect(sock,&server,sizeof(server))<0){
perror("connect");
return 1;
}
else{
printf("conneted\n");}

}
