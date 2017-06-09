//
// Created by rbeal on 5/15/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include "global.h"
#include "tcp_server.h"
#include "rs232.h"

#define MAXPENDING 5

void tcp_server(struct shared_t *shared) {

    int socket_tcp;
    struct sockaddr_in myaddr_in;
    struct sockaddr_in myaddr_out;
    
    struct sockaddr_in myaddr_tcp;

    struct sockaddr_in client_in;
    struct sockaddr_in client_out;

    static unsigned int clntLen;
    static unsigned short echoServPort=15000;
    static int clnSock;
    static char recvMsgSize=0;

    static struct sockaddr_in echoClntAddr;

    static int clntSock;
    static int servSock;

    static int connected=0;
    int dog = 0;
    int watchdog=0;
    int cycles=0;
    int simu=0;
    int debug=0;

    static char buffso_rcv[32];
    static char buffso_send[30];

    pthread_mutex_t	mutex_send = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t	mutex_rcv = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t	mutex_raw_out = PTHREAD_MUTEX_INITIALIZER;

    printf("listenTCP \n");

    /* Create socket for incoming connections */
    if ((socket_tcp = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        printf("socket create error\n");

    /* Construct local address structure */
    memset(&myaddr_tcp, 0, sizeof(myaddr_tcp));   /* Zero out structure */
    myaddr_tcp.sin_family = AF_INET;                /* Internet address family */
    myaddr_tcp.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myaddr_tcp.sin_port = htons(15020);      /* Local port */

    /* Bind to the local address */
    int autorisation=1;
    setsockopt(socket_tcp,SOL_SOCKET,SO_REUSEADDR,&autorisation,sizeof(int));

    if (bind(socket_tcp, (struct sockaddr *) &myaddr_tcp, sizeof(myaddr_tcp)) < 0)
        printf("bind error\n");

    for (;;) /* Run forever */
    {
        printf("listenTCP \n\r");

        /* Mark the socket so it will listen for incoming connections */
        if (listen(socket_tcp, MAXPENDING) < 0)
            printf("listen error\n");

        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        /* Wait for a client to connect */
        if ((clntSock = accept(socket_tcp, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) printf("accept error\n");
        printf("listenOK \n\r");

        /* clntSock is connected to a client! */
        do{
            if ((recvMsgSize = recv(clntSock, shared->buffso_Raw_Data_In, 30, 0)) < 1) {
		    if (debug==3) 
			    printf("recvMsgSize<1 : %d \n",recvMsgSize);
		    shutdown(clntSock,1);}

            else{
                //pthread_mutex_lock (& mutex_dog);
                connected=1;
                dog =0;
                //pthread_mutex_unlock (& mutex_dog);
                if (debug==0)
                {
			shared->cmd_mode = 1;
			rs232_write(shared->buffso_Raw_Data_In, recvMsgSize);
			//printf("size: %d\n\r", recvMsgSize);

                    watchdog = 0;
                    cycles = 3;
                    pthread_mutex_lock (& mutex_raw_out);
                    send(clntSock, shared->buff_data_in,21,0); //pb crach soft
                    pthread_mutex_unlock (& mutex_raw_out);
                    if (debug==3) printf("I2C passed OK\n");
                }
                else if ((debug==1)||(debug==3))
                {
                    buffso_send[0]=simu;simu++;if (simu==160) simu=0;
                    send(clntSock,buffso_send,7,0);//pb crach soft
                }
            }
        }while(recvMsgSize>0);

        if (debug==3) printf("recvMsgSize<=0 : %d \n",recvMsgSize);
        connected=0;
        shutdown(clntSock,1);
        close(clntSock);
        // sleep(1);
    }//end for(;;)
}
