#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "global.h"
#include "scratch_server.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void scratch_server(struct shared_t *shared) {

   SOCKET sock = init_connection("192.168.11.46");
   char buffer[BUF_SIZE];

   fd_set rdfs;

   /* send our name */
   //write_server(sock, name);

   while(1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
	/*
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         fgets(buffer, BUF_SIZE - 1, stdin);
         {
            char *p = NULL;
            p = strstr(buffer, "\n");
            if(p != NULL)
            {
               *p = 0;
            }
            else
            {
               // fclean
               buffer[BUF_SIZE - 1] = 0;
            }
         }
         write_server(sock, buffer);
      }
      else*/ if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
	 int size, i, msg_nb, msg_offset;
	 char *data;
	 data = malloc(20);
         /* server down */
         if(n == 0)
         {
            printf("Server disconnected !\n");
            break;
         }
         //puts(buffer);

	 msg_nb     = 0;
	 msg_offset = 0;
	
	 	size = *(buffer+0)<<24 | *(buffer+1)<<16 | *(buffer+2)<<8 | *(buffer+3);
	 	printf("size data: %d\n\r", size);
	 	for (i=4;i<n;i++)
			printf("%c", *(buffer+i));

	 	printf("\n\rsize_recv: %d\n\r", n);
	 	printf("size data: %d\n\r", size);

	for (i=15;*(buffer+i) != '"';i++) {
		*(data+i-15) = *(buffer+i);

	}
	*(data+i-15) = 0;

	if (strcmp(data, "more") == 0) {
		printf("MORE!!! \n\r");
		if (shared->wifibot_cmd->speed_l >= 0)
			shared->wifibot_cmd->speed_l += 500;
		if (shared->wifibot_cmd->speed_r >= 0)
			shared->wifibot_cmd->speed_r += 500;
	}
	
	if (strcmp(data, "less") == 0) {
		printf("LESS!!! \n\r");
		if (shared->wifibot_cmd->speed_l < 65000)
			shared->wifibot_cmd->speed_l -= 500;
		if (shared->wifibot_cmd->speed_r >= 0)
			shared->wifibot_cmd->speed_r -= 500;
	}

	printf("youp=%d\n\r", shared->wifibot_cmd->speed_l);

      }
   }

   end_connection(sock);
}

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}



static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      //exit(errno);
      pthread_exit(&errno);
   }
   else {
	   printf("Scratch detected !\n");
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}

static void write_server(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}
