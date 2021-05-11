#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 10240
#define PORT 9999
#define SA struct sockaddr
void resp(int s, char*, long int);
int execute(int s,char* cmd, char* dstip, int count)
{
	char statement[200];
	char response[10240], msg[10240];
	int length;
	if(strcmp(cmd,"TCP-SYN")==0)
	{
		sprintf(statement,"hping3 %s --syn -c %d",dstip, count);

	}
	else if(strcmp(cmd, "TCP-ACK") == 0)
	{
		sprintf(statement,"hping3 %s --ack -c %d",dstip, count);
	}
	else if(strcmp(cmd, "TCP-XMAS") == 0)
	{
		sprintf(statement,"hping3 %s --xmas -c %d",dstip, count);
	}
	else
	{
		sprintf(msg,"Incorrect command type passed");
		resp(s,msg, sizeof(msg));
		return;
	}
	//printf("%s,statement")
	FILE* fp = popen(statement,"r");
	//sleep(10);
	if(fp == NULL)
	{
		printf("\nUnable to Spawn process");
		return -1;
	}
	bzero(msg,sizeof(msg));
	while(fgets(response,sizeof(response),fp)!=NULL)
	{
		strcat(msg,response);
	}
	//free(response);
	//printf("%s",msg);
	resp(s,msg,sizeof(msg));
	pclose(fp);
	//system(cmd);
	return;
}
int parse(int s, char* msg)
{
	/*char* cmd = strchr(msg,':');
	if(cmd == NULL)
	{
		printf("Incorrect format");
		return -1;
	}
	cmd++;
	printf("\n%s\n",cmd);
	execute(s,cmd);*/
	char string[50];
	char* cmd, *dstip, *token;
	int count, n =0;
	strcpy(string, msg);
	token = strtok(string, " ");
	while(token!=NULL)
	{
		token = strtok(NULL," ");
		switch(n)
		{
			case 0:
				cmd = strdup(token);
				break;
			case 1:
				dstip = strdup(token);
				break;
			case 2:
				count = atoi(token);
				break;
		}
		n++;
	}
	if(n!=4)
	{
		strcpy(msg,"Incorrect argument ");
		write(s,msg,strlen(msg));
		return;
	}
	else
		execute(s,cmd,dstip,count);

}
void resp(int s, char* msg,long int size)
{
	//bzero(msg,sizeof(msg));
	write(s,msg,size);
}

void receive(int s, char*msg)
{
	bzero(msg,sizeof(msg));
	read(s,msg,sizeof(msg));
	FILE* fp = fopen("storage.txt","w");
	fprintf(fp,"%s",msg);
	fclose(fp);
	//exit(0);
}
void func(int sockfd)
{
	char buff[MAX];
	char *temp = NULL;
	int n=5;
	bzero(buff,sizeof(buff));
	sprintf(buff,"received");
	resp(sockfd,buff,sizeof(buff));
	for (;;) {
		bzero(buff, sizeof(buff));
		/*printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;*/
		//write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		//receive(sockfd,buff);
		
		//FILE*fp  = fopen("storage.txt","r");
		//bzero(temp,sizeof(temp));
		//fscanf(fp,"%s",temp);
		//fscanf(fp,"%[^\n]*c",temp);
		//fclose(fp);
		printf("From Server,%d : %s", sizeof(buff),buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
		parse(sockfd,buff);
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

