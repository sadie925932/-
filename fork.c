#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/sendfile.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

char webpage[]=
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>HELLO</title>\r\n"
"<style>body { background-color: #bcdefa }</style></head>\r\n"
"<body><center><h1 style=\"font-family:courier;\">GREENISLAND</h1><br>\r\n"
"<p style=\"font-family:courier;\">2018 SUMMER</p>\r\n"
"<img src=\"greenisland.jpg\" style=\"weight:500px;height:600px;\"></center></body></html>\r\n";


int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t sin_len=sizeof(client_addr);
	int fd_server , fd_client;
	char buf[2100];
	int fdimg;
	int on=1;

	fd_server=socket(AF_INET, SOCK_STREAM,0);
	if(fd_server<0)
	{
		perror("socket");
		exit(1);
	}
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(8088);

	if(bind(fd_server,(struct sockaddr*)&server_addr, sizeof(server_addr))==-1)
	{
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server,10) ==-1)
	{
		perror("listen");
		close(fd_server);
		exit(1);
	}

	while(1)
	{
		fd_client=accept(fd_server,(struct sockaddr*)&client_addr,&sin_len);

		if(fd_client==-1)
		{
			perror("Connection failed\n");
			continue;
		}
		printf("Got connrction\n");
		if(!fork())
		{
			/*child process*/
			close(fd_server);
			memset(buf,0,2099);
			read(fd_client,buf,2099);

			printf("%s\n",buf);

			if(!strncmp(buf,"GET /hw1.png", 12))
			{
				fdimg=open("hw1.png",O_RDONLY);
				sendfile(fd_client, fdimg,NULL,4000);
				close(fdimg);
			}

			else if(!strncmp(buf,"GET /greenisland.jpg",20))
			{
				fdimg=open("greenisland.jpg",O_RDONLY);
				sendfile(fd_client,fdimg,NULL,20000000);
				close(fdimg);
			}
			else
				write(fd_client,webpage,sizeof(webpage)-1);

			close(fd_client);
			printf("closing\n");
			exit(0);
		}
		/*parent process*/
		close(fd_client);
	}

	return 0;
}

