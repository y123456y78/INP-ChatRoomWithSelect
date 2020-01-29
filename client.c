#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define buf_size 1024
int main(int argc,char**argv){
	if(argc!=3)
		{
			printf("Invaild input");
			return 0;
		}
	struct sockaddr_in servaddr;	
	int sockfd,maxfd,check,portNum=atoi(argv[2]),len,nready;
	char data[buf_size+1];
	fd_set allset,rset;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(portNum);
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	if((check=connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))==-1)
	{
		printf("connect_ERR");
		return 0;
	}
	
	FD_ZERO(&allset);
	FD_SET(sockfd,&allset);
	FD_SET(0,&allset);
	maxfd=sockfd;
	while(1)
	{
		rset=allset;
        nready=select(maxfd+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&rset))
		{
            memset(data,'\0',sizeof(char)*buf_size);
            check=recv(sockfd,data,buf_size,0);

            if(check>0)
                printf("%s", data);
            else
                printf("ERREO:sever send NULL\n");
            if(--nready<=0)continue;
		}
         
           
        if(FD_ISSET(0,&rset))
        {
            memset(data,'\0',sizeof(char)*buf_size);
            fgets(data,buf_size,stdin);
            if(data[0]=='e' && data[1]=='x' && data[2]=='i' && data[3]=='t')
            {
                close(sockfd);
                return 0;
            }
            else
            {
                len=strlen(data);
                send(sockfd,data,len,0);
            }

        }
	}
		return 0;
}