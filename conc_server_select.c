// concurrent_tcp_echo_ server.c (using select())
#include"headers.h"
int sfd,cfd;
struct sockaddr_in saddr,caddr;
int slen;
char buff[100];
int ret,newfd,fd,maxfd,cnt=0;
struct sockaddr_in cliAddr;
int length,clen;
fd_set avl_fds,ready_fds;

int main(int argc,char **argv)
{
	if(argc!=2)
	{
		puts("input: server_exe well_known_port_no");
		return 0;
	} 

	sfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sfd<0)
	{
		perror("socket");
		return 0;
	}
	puts("server socket created successfully");

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr=INADDR_ANY;
	saddr.sin_port = htons(atoi(argv[1]));
	slen=sizeof saddr;
	puts("binding...");
	if(bind(sfd, (struct sockaddr *)&saddr,slen)<0)
	{
		perror("bind");
		close(sfd);
		return 0;
	}
	puts("bind success");

	puts("listen: creating connection queue..");
	if(listen(sfd,1)<0)
	{
		perror("listen");
		close(sfd);
		return 0;
	}
	puts("listen success");

	FD_ZERO(&avl_fds);
	FD_SET(sfd,&avl_fds);
	maxfd=sfd;

	while(1)
	{
		ready_fds=avl_fds;
		printf("server waiting...\n");
		printf("maxfd : %d\n",maxfd);
		printf("no of clients connected: %d\n",cnt);
		printf("range of file descriptors:%d<%d\n",sfd,maxfd+1);
		ret = select(10,&ready_fds,NULL,NULL,NULL);
		system("clear");
		if(ret < 0)
		{
			perror("select");
			break;
		}
		else if(ret==0)
		{
			puts("time out");
			continue;
		}


		for(fd=0;fd<10;fd++)
		{
			if(FD_ISSET(fd,&ready_fds))
			{        
				if((sfd==fd))
				{
					clen=sizeof cliAddr;
					newfd = accept(sfd,(struct sockaddr *)&cliAddr,&clen);
					if(newfd<0)
					{
						perror("accept");
					} 
					FD_SET(newfd,&avl_fds);
					cnt++;
					maxfd=(newfd>maxfd)?newfd:maxfd;
					puts("new client connected");
				}
				else        
				{
					ret=recv(fd,buff,100,0);
					length=strlen(buff);	

					if(strcmp(buff,"quit")==0)
					{
						printf("formally closing connection\n");
						printf("removing client on socket fd %d\n",fd);
						FD_CLR(fd,&avl_fds);
						close(fd);
						cnt--;
						if(fd==maxfd)
						{
							maxfd--;
						}
					}
					send(fd,&length,sizeof(int),0);

				}
			}
		}
	}

	close(sfd);
}


