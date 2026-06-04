//tcp_client.c
#include "headers.h"

int main(int argc,char **argv)
{
	char buff[100];
	int status;

	if(argc<3)
	{
		puts("input format @cmdline : client.exe server_port_no server_ip_address");
		return 0;    
	}

	puts("Creating client Socket...");
	int fd;
	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd<0)
	{
		perror("socket");
		return 0;
	}	
	puts("client socket created successfully");

	puts("Binding & Connecting: ");
	puts("Mapping the file descripted with Server's IP & Server Port ( Remote Socket Address)");
	puts("Also mapping With Client's IP & Ephemeral Port (Local Socket Address)");
	puts("Initiating 3-Way Handshake for Connection Establishement With Server...");
	struct sockaddr_in addr;
	int len=sizeof(addr);
	size_t length;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(argv[1]));
	addr.sin_addr.s_addr=inet_addr(argv[2]);
	if(connect(fd,(struct sockaddr *)&addr,len)<0)
	{
		perror("connect");
		return 0;     
	}
	puts("Connection Success With Server");


	while(1)
	{
		puts("Enter some message to send to server...");
		__fpurge(stdin);
		fgets(buff,100,stdin);
		if(buff[strlen(buff)-1]=='\n')
			buff[strlen(buff)-1]='\0';    

		printf("sending message to server... : %s\n",buff);
		status=send(fd,buff,strlen(buff),0);
		if(status<0)
		{
			perror("send"); 	     
		}     
		if(strcmp(buff,"quit")==0)
			break;
		puts("Message Sent");
		recv(fd,&length,sizeof length,0);
		printf("%lu",length);
	}

	puts("Client Terminating Connection With Server");
	close(fd);

}	
