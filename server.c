#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
int main()
{
int sock,namelen;
struct sockaddr_in server;
char msgsock;
char buf[1024];
sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);// socket( int af, int type, int protocol)
if(sock<0){
perror("socket");// printf+error describtion
}
else{printf("socket created\n");}//socket creation

server.sin_family=AF_INET;
server.sin_addr.s_addr=INADDR_ANY;
server.sin_port=htons(8888);

if(bind(sock,&server,sizeof(server))<0){
perror("bind");
}
else{printf("bind success\n");}//bind

if(listen(sock,5)<0){
perror("listen");
}//listen

printf("ready for connection");

namelen=sizeof(server);
if((msgsock=accept(sock,&server,&namelen))<0){
perror("accept");
}

printf("connection accepted");
return 0;
}
