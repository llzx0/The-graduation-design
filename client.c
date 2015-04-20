#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 4096

void DieWithError(char *errorMessage){
perror(errorMessage);
exit(1);
}

int main(int argc,char *argv[]){
int sock;
struct sockaddr_in echoserver;
char buffer[BUFFSIZE];
unsigned int echolen;
int received=0;
if(argc!=4){
DieWithError("USAGE:simple_client <server_ip> <port> <request>\n");
}
if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
DieWithError("failed to creat socket");
}
memset(&echoserver,0,sizeof(echoserver));
echoserver.sin_family=AF_INET;
echoserver.sin_addr.s_addr=inet_addr(argv[1]);
echoserver.sin_port=htons(atoi(argv[2]));

if(connect(sock,(struct sockaddr *)&echoserver,sizeof(echoserver))<0){
DieWithError("failed to connect with server");
}
char *msg=argv[3];
echolen=strlen(msg);
if(send(sock,msg,echolen,0)!=echolen){
DieWithError("send package error");
}
printf("received HTTP response from %s:\n",argv[2]);
int bytes;
while((bytes=recv(sock,buffer,BUFFSIZE-1,0))>0){
buffer[bytes]='\0';
printf("%s\n",buffer);
}
close(sock);
exit(0);
}
