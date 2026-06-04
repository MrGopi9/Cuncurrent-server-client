/* conc_server_thread.c */
//concurrent server-using threads
//To Compile : cc conc_server_thread.c -o server
/* Advantage:
   In Threaded model for concurrent server, thread creation takes
   very less time compared to process creation.This reduces the time
   between two consecutive accept() calls and increases number of 
   connection request handled by the server

Disadvantage:
If any thread which handles client gets segmentation fault due to
some software bug,all client connections are disconnected as the
server process itself will crash and dump core.
*/


#include"headers.h"


void* threadFunc(void *arg)
{
	char buff[100];
	int ret;
	int fd=*(int *)arg;
	printf("\nThread created to handle client\n");
	while(1)
	{
		puts("thread waiting for message....\n");
		bzero(buff,100);
		ret=recv(fd,buff,100,0);
		if(ret<0)
		{
			perror("recv");
			break;
		} 
		if(ret==0)
		{
			printf("client abnormally terminated\n");
			break;
		}  
		printf("thread of server read: %s\n",buff);
		if(strcmp(buff,"quit")==0)
			break;

		ret=send(fd,buff,100,0);
		if(ret==-1)
		{
			printf("unable to send the response\n");
			break;
		} 
		printf("thread of server echoed: %s\n",buff);
		if(strcmp(buff,"exit")==0)
			break;
	}
	close(fd);
	pthread_exit(0);
}

int main(int argc,char **argv)
{

	if(argc!=2)
	{
		puts("input: server_executable known_server_port_no\n");
		exit(0);
	}

	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		perror("socket");
		exit(0);
	}
	puts("server socket created\n");

	
	puts("changing socket options\n");
	int optval=1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,4)<0)
	{
		perror("setsockopt");
		exit(0);
	}
	puts("setsockopt success\n");

	
	struct sockaddr_in saddr;
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("0.0.0.0");
	saddr.sin_port=htons(atoi(argv[1]));


	if(bind(sfd,(struct sockaddr*)&saddr,sizeof(saddr))<0)
	{
		perror("bind");
		close(sfd);
		exit(0);
	}
	puts("bind success\n");
	
	if(listen(sfd,1)<0)
	{
		perror("listen");
		exit(0);
	}
	puts("listen success\n");

	
	struct sockaddr_in caddr;
	int cfd,clen=sizeof(caddr);
	pthread_t thr;
	while(1)
	{
		puts("\nserver waiting for connection from client...\n");
		cfd=accept(sfd,(struct sockaddr*)&caddr,&clen);
		
		char *ip=inet_ntoa(caddr.sin_addr);
		unsigned short pNo=ntohs(caddr.sin_port);
		if(cfd<0)
		{
			perror("accept\n");
			close(sfd);
			exit(0);
		}
		printf("server accepted connection from client with IP and port : \n");
		printf("%s : %d",ip,pNo);
		pthread_create(&thr,NULL,threadFunc,&cfd);
	}
	close(sfd);
	return 0;
}



