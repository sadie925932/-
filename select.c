/* include fig01 */

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
#include<string.h>

#define MAXLINE 10000

int main(int argc, char **argv)
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(8888);

	bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	listen(listenfd, 1000);

	maxfd = listenfd;                       /* initialize */
	maxi = -1;                                      /* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;                 /* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	/* end fig01 */

	/* include fig02 */

	for( ; ; )
	{

		rset = allset;          /* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {        /* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
#ifdef  NOTDEF
			printf("new client: %s, port %d\n",Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),ntohs(cliaddr.sin_port));
#endif

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;     /* save descriptor */
					break;
				}
			if (i == FD_SETSIZE)
				perror("too many clients"); /*err_quit*/

			FD_SET(connfd, &allset);        /* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;                 /* for select */
			if (i > maxi)
				maxi = i;                               /* max index in client[] array */

			if (--nready <= 0)
				continue;                               /* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {   /* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, buf, MAXLINE)) == 0) {
					/*4connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} 

				else if(!strncmp(buf,"GET /cat2.jpg",13))
                                {

                                        int img=open("cat2.jpg",O_RDONLY);
                                        sendfile(sockfd,img,NULL,10000);
                                        close(img);
					close(sockfd);
                                        FD_CLR(sockfd, &allset);
                                        client[i] = -1;

                                }

				else
				{
					memset(buf,'\0',MAXLINE);
					char buf[]=	
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html; charset=UTF-8\r\n\r\n"
						"<!DOCTYPE html>\r\n"
						"<html><head><title>HELLO</title>\r\n"
						"<style>body { background-color: #fff9c2 }</style></head>\r\n"
						"<body><center><h1 style=\"font-family:courier;\">FAT CUTE CAT</h1><br>\r\n"
						"<p style=\"font-family:courier;\">FATCATCOLLECTION</p></center>\r\n"
						"<center><img src=\"cat2.jpg\" style=\"weight:350px;height:420px;\"></body></center></html>\r\n";
					write(sockfd, buf, sizeof(buf)-1);
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;

				}

				if (--nready <= 0)
					break;                          /* no more readable descriptors */
			}
		}
	}
}

/* end fig02 */
