#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>



#define SIZE 8192

#define MSG_SIZE (SIZE-4)



typedef struct tag_mag// 

{

    int msg_len ;

    char msg_buf[MSG_SIZE];

}MSG, *pMSG;



int main(int argc, char* argv[])

{

    int sock;

    struct sockaddr_in echoserver;

    unsigned int echolen;

    int received = 0;



    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 

    {

        perror("Failed to create socket");

        exit(1);

    }

    



    memset(&echoserver, 0, sizeof(echoserver));      

    echoserver.sin_family = AF_INET;               

    echoserver.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    echoserver.sin_port = htons(5656);

    bind(sock, (struct sockaddr *) &echoserver, sizeof(echoserver));

    if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) 

    {

        perror("Failed to connect with server");



        exit(1);

    }



    MSG my_msg ;

    memset(&my_msg, 0, sizeof(MSG));

    strcpy(my_msg.msg_buf, "connected to the server");

    my_msg.msg_len = strlen(my_msg.msg_buf);

    st_send(NULL, sock, &my_msg, 4 + my_msg.msg_len );

    memset(&my_msg, 0, sizeof(MSG));

    st_recv(NULL, sock, &my_msg, 4);

    st_recv(NULL, sock, &my_msg.msg_buf, my_msg.msg_len);

    printf("recv from server : %s \n", my_msg.msg_buf);

    

 

    memset(&my_msg, 0, sizeof(MSG));

    st_send(NULL, sock, &my_msg, 4 + my_msg.msg_len);

    close(sock);

}

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

   

 

    
