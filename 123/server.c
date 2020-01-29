#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define buf_size 1024
#define setSize 10
struct client_str
{
	char cli_ipstr[INET_ADDRSTRLEN];
	int cli_port;
	char cli_name[12];
	int cli_fd;
};
int main(int argc,char *argv[])
{
	if(argc!=2)
		return 0;
	int portNum=atoi(argv[1]);
	int listenfd,connfd,sockfd,i,maxi,maxfd,len;
	int nready;
	struct client_str client[setSize+1];
	ssize_t n;
	char line[buf_size+1],reply[buf_size+1];
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	fd_set rset,allset;
	void *addr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(portNum);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,20);

	maxfd=listenfd;
	maxi=-1;
	for(int i=0;i<10;i++)
		client[i].cli_fd=-1; 	//initialize
	FD_ZERO(&allset);			
	FD_SET(listenfd,&allset);	

	while(1)
	{
		rset=allset;
		//printf("before select\n");
		nready=select(maxfd+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(listenfd,&rset))
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
			for(i=0;i<setSize;i++)							//try to find a place for new client
			{
				if(client[i].cli_fd<0)
					{
						client[i].cli_fd=connfd;
						sprintf(client[i].cli_name,"anonymous");
						break;
					}
			}

			if(i==setSize)									//Didn't find
				{
					printf("too many clientss"); 
					continue;
				} 

			addr=&(cliaddr.sin_addr);						//handle new client's connection
			inet_ntop(AF_INET,addr,client[i].cli_ipstr,sizeof(client[i].cli_ipstr));
			client[i].cli_port=ntohs(cliaddr.sin_port);
			FD_SET(connfd, &allset);

			memset(reply,'\0',sizeof(char)*buf_size);		//say hello to new client
			sprintf(reply,"[Server] Hello, %s! From: %s:%d\n",client[i].cli_name,client[i].cli_ipstr,client[i].cli_port);
			n=strlen(reply);
			write(connfd,reply,n);

			if (i > maxi)									//update maxi && maxfd if need
        		maxi = i; 
       		 if (connfd > maxfd)
        		maxfd = connfd;  

			memset(line,'\0',sizeof(char)*buf_size);		//told others the new friend is come in
			sprintf(line,"[Server] Someone is coming!\n");
			n=strlen(line);
			for(int j=0;j<=maxi;j++)
				if(client[j].cli_fd!=-1 && client[j].cli_fd!=connfd)
					write(client[j].cli_fd,line,n);						
			                      
        	if (--nready <= 0)								//if no more readable data, back to wait
         		continue;
        }         

        for(i=0;i<=maxi;i++)								//handle client's message
        {
        	if((sockfd=client[i].cli_fd)<0)					//skip empty in client array
        		continue;

        	if(FD_ISSET(sockfd,&rset))						
        	{
        		memset(line,'\0',sizeof(char)*buf_size);
        		len=recv(sockfd, line, buf_size, 0);
        		
        		if(len<=0)									//someone is leave
        		{
        			
        			close(sockfd);							//handle the leaving
        			FD_CLR(sockfd,&allset);
        			client[i].cli_fd=-1;
        			if(sockfd==maxfd)maxfd--;
        			if(i==maxi)maxi--;

        			memset(reply,'\0',sizeof(char)*buf_size);	//told others
        			sprintf(reply,"[Server] %s is offline.\n",client[i].cli_name);
        			len=strlen(reply);
        			for(int j=0;j<=maxi;j++)
        				if(client[j].cli_fd>-1 && j!=i)
        					send(client[j].cli_fd,reply,len,0);
        			
        		}
        		else
        		{
        			line[--len]='\0';
        			//printf("message is <%s>, len is %d\n",line,len);

        			if(line[0]=='w' && line[1]=='h' && line[2]=='o')
        			{
        				for(int j=0;j<=maxi;j++)
        				{
        					//printf("who at %d\n",j );
        					if(client[j].cli_fd<0)continue;
        					if(i==j)
        						sprintf(reply,"[Server] %s %s:%d ->me\n",client[j].cli_name,client[j].cli_ipstr,client[j].cli_port);
        					else
        						sprintf(reply,"[Server] %s %s:%d\n",client[j].cli_name,client[j].cli_ipstr,client[j].cli_port);
        					n=strlen(reply);
        					send(client[i].cli_fd,reply,n,0);

        				}
        			}
        			else if(line[0]=='n' && line[1]=='a' && line[2]=='m' && line[3]=='e')
        			{	
        				int newName_len;
        				char *p;
        				p=strtok(line," ");
        				p=strtok(NULL," ");

        				newName_len=strlen(p);
        				p[newName_len]='\0';

        				//printf("newname is <%s>, len is %d\n",p,newName_len);
        				int if_number=0;
        				for(int j=0;j<newName_len;j++)
        				{
        					//printf("test\n");
        					if(p[j]>='0' && p[j]<='9')
        					{
        						if_number=1;
        						break;
        					}
        				}
        				//printf("testend");
        				memset(reply,'\0',sizeof(char)*buf_size);
        				if(newName_len<2 || newName_len>12 || if_number==1)
        				{
        					//printf("can only consists of 2~12 English letters.\n");
        					sprintf(reply,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
        					len=strlen(reply);
        					send(client[i].cli_fd,reply,len,0);
        				}
        				else if(strcmp(p,"anonymous")==0)
        				{
        					//printf("cannot be anonymous.\n");
        					sprintf(reply,"[Server] ERROR: Username cannot be anonymous.\n");
        					len=strlen(reply);
        					send(client[i].cli_fd,reply,len,0);
        				}
        				else
        				{
        					int flag=0;
        		
        					for(int j=0;j<=maxi;j++)
        					{
        						if(client[j].cli_fd<0 || i==j)continue;
        						if(!strcmp(client[j].cli_name,p))
        						{
        							sprintf(reply,"[Server] ERROR: %s has been used by others.\n",p);
        							len=strlen(reply);
        							send(client[i].cli_fd,reply,len,0);
        							flag=1;
        							break;
        						}
        					}
        					if(!flag)
        						{
        							sprintf(reply,"[Server] %s is now known as %s.\n",client[i].cli_name,p);
        							len=strlen(reply);
        							for(int j=0;j<=maxi;j++)
        							{
        								if(client[j].cli_fd>-1 && i!=j)
        									send(client[j].cli_fd,reply,len,0);
        							}
        							for(int j=0;j<12;j++){
        								if(j<newName_len)
        								client[i].cli_name[j]=p[j];
        								else client[i].cli_name[j]='\0';
        							}
        							newName_len=strlen(client[i].cli_name);
        							//printf("now client newname is <%s>, len is %d\n",client[i].cli_name,newName_len );

        							memset(reply,'\0',sizeof(char)*(buf_size+1));
        							sprintf(reply,"[Server] You're now known as %s.\n",client[i].cli_name);
        							len=strlen(reply);
        							send(client[i].cli_fd,reply,len,0);
        						}
        				}
        				
        			}
        			else if(line[0]=='y' && line[1]=='e' && line[2]=='l' && line[3]=='l')
        			{
        				char *message;
        				message=strtok(line," ");
        				message=strtok(NULL,"\0");
        				memset(reply,'\0',sizeof(char)*(buf_size+1));
        				sprintf(reply,"[Server] %s yell %s\n",client[i].cli_name,message);
        				len=strlen(reply);
        				for(int j=0;j<=maxi;j++)
        					if(client[j].cli_fd>-1)
        						send(client[j].cli_fd,reply,len,0);

        			}
        			else if(line[0]=='t' && line[1]=='e' && line[2]=='l' && line[3]=='l')
        			{
        				char *destName,*message,*count;
        				destName=strtok(line," ");
        				destName=strtok(NULL," ");
        				message=strtok(NULL,"\0");
        				memset(reply,'\0',sizeof(char)*(buf_size+1));
        				if(!strcmp(destName,"anonymous"))
        				{
        					sprintf(reply,"[Server] ERROR: The client to which you sent is anonymous.\n");
        					len=strlen(reply);
        					send(client[i].cli_fd,reply,len,0);
        				}
        				else if(!strcmp(client[i].cli_name,"anonymous"))
        				{
        					sprintf(reply,"[Server] ERROR: You are anonymous.\n");
        					len=strlen(reply);
        					send(client[i].cli_fd,reply,len,0);
        				}
        				else
        				{
        					int flag=0,destfd;
        					for(int j=0;j<=maxi;j++)
        					{
        						if(client[j].cli_fd<0)
        							continue;
        						if(!strcmp(client[j].cli_name,destName))
        							{
        								flag=1;
        								destfd=client[j].cli_fd;
        								break;
        							}
        					}
        					if(!flag)
        					{
        					sprintf(reply,"[Server] ERROR: The receiver doesn't exist.\n");
        					len=strlen(reply);
        					send(client[i].cli_fd,reply,len,0);
        					}
        					else
        					{
        					sprintf(reply,"[Server] SUCCESS: Your message has been sent.\n");
							len=strlen(reply);
							send(client[i].cli_fd,reply,len,0);
							memset(reply,'\0',sizeof(char)*(buf_size+1));
        					sprintf(reply,"[Server] %s tell you %s\n",client[i].cli_name,message);
        					len=strlen(reply);
							send(destfd,reply,len,0);							
        					}
        				}
        			}
        			else
        			{
        				memset(reply,'\0',sizeof(char)*(buf_size+1));
        				sprintf(reply,"[Server] ERROR: Error command.\n");
        				len=strlen(reply);
        				//reply[++len]='\0';
        				send(client[i].cli_fd,reply,len,0);
        			}


        		}
        		if(--nready<=0)
        		break;
        	}
        }
	}

}