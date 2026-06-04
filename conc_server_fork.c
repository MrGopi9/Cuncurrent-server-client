//program 3 concurrent server-using fork()
#include"headers.h"

void sig_chld(int signo)
{
	pid_t pid;
	int status;
	pid=wait(&status);
	printf("child %d terminated with status %d \n",pid,(status>>8));
}

void client_info(int cfd,struct sockaddr_in caddr)
{
	char *ip=(char *)inet_ntoa(caddr.sin_addr);
	unsigned short portNo=ntohs(caddr.sin_port);
	printf("Whose IP is %s\n",ip);
	printf("and port is %d\n",portNo);
	printf("using descriptor %d\n",cfd);
}

size_t mystrlen(char *p)
{
	size_t i=0;
	while(p[i++]);
	
	return i-1;
}


int main(int argc,char **argv)
{
	signal(SIGCHLD,sig_chld);

	if(argc<2)
	{ 
		puts("input : server_executable port_number\n");
		exit(0); 
	}

	puts("creating server socketn");
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		perror("socket"); 
		exit(0); 
	}
	puts("server socket created succussfully");


	int val=1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&val,4)<0)
	{
		perror("setsockopt");
		exit(0);
	}
	puts("setsockopt success");

	puts("binding...");
	struct sockaddr_in saddr;
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("0.0.0.0");
	saddr.sin_port=htons(atoi(argv[1]));
	socklen_t slen=sizeof(saddr);
	if(bind(sfd,(const struct sockaddr *)&saddr,sizeof(saddr))<0)
	{
		perror("bind");
		exit(0); 
	}
	puts("bind success");

	puts("preparing connection queue");
	if(listen(sfd,1)<0)
	{ 
		perror("listen"); 
		exit(0);
	}
	puts("listen success\n");

	struct sockaddr_in caddr;
	int cfd;
	socklen_t clen=sizeof(caddr);

	while(1)
	{
		puts("server waiting for connection....\n");
		cfd=accept(sfd,(struct sockaddr *)&caddr,&clen);
		if(cfd<0)
		{
			perror("accept");
			exit(0); 
		}
		printf("serve... %d accepted connection from client with CFD: %d \n",getpid(),cfd);
		client_info(cfd,caddr);
		if(fork()==0)
		{
			char buff[100];
			int status;
			size_t len;
			while(1)
			{
				printf("server ... %d waiting for message from client...\n",getpid());
				status=recv(cfd,buff,100,0);
				if(status<0)
				{ 
					perror("recv");
					break; 
				}
				else if(status==0)
				{
					puts("client exited abnormally\n"); 
					break;
				}
				printf("server recv: %s\n",buff);
				


				if(strcmp(buff,"quit")==0)
				{ 
					puts("client terminating formally\n");
					break; 
				}
				
				len=mystrlen(buff);
				send(cfd,&len,sizeof len,0);
			


			}
			close(cfd);
			exit(cfd);
		}
	}
	close(sfd);
}  


