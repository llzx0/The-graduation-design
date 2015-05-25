#ifndef __SERVER_H__

#define __SERVER_H__

#include <sys/stat.h>

#include <fcntl.h>

#include <sys/time.h>

#include <sys/select.h>

#include <sys/uio.h>

#include <sys/wait.h>

#include <errno.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#define ST_BUSY 1

#define ST_IDLE 2

#define SIZE 8192

#define MSG_SIZE (SIZE - 4)



typedef struct tag_mag

{

    int msg_len ; 

    char msg_buf[MSG_SIZE];

}MSG, *pMSG;



typedef struct tag_chd

{

    int s_sfd ;//socketpair 句柄

    int s_state ;

}NODE, *pNODE;



extern int errno ;

void createchild(pNODE arr, int cnt);

void childloop(int sfd) ;

void handle_request(int sfd);

void send_fd(int sfd, int fd_file) ;

void recv_fd(int sfd, int* fd_file) ;

 

#endif
