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

pthread_mutex_t lock;

// Function designed for chat between client and server.
void* socketThread( void* connfd)
{
    func(*((int*)connfd));
	pthread_exit(NULL);
    return NULL;
}
void func(int sockfd)
{
	char buff[MAX], hostname[20], ip[15];
	int n;
	bzero(buff, MAX);
	read(sockfd, buff, sizeof(buff));
	//printf("From client: %s\t To client : ", buff);
	strcpy(hostname, buff);
	bzero(buff, MAX);
	read(sockfd, buff, sizeof(buff));
	strcpy(ip,buff);
	for (;;) 
	{
		
        pthread_mutex_lock(&lock);
        printf("Command for (hostname: %s,ip address: %s, socket identifier: %d): ", hostname, ip, sockfd);
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
		write(sockfd, buff, sizeof(buff));
        printf("Command executed\n", sockfd);

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
        pthread_mutex_unlock(&lock);
		sleep(1);
	}
	pthread_mutex_unlock(&lock);
}

// Driver function
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
	// Accept the data packet from client and verification
	while(1)
    {
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
        
        printf("server acccept failed...\n");
        exit(0);
        }
        
        else
        {
            if(pthread_create(&thread[i++], NULL, socketThread, &connfd) != 0)
            {
                printf("Failed to create thread\n");
            }
            /*else
            {
                pthread_join(thread, NULL);
            } */     
        }      


        // Function for chatting between client and server
    }
	// After chatting close the socket
	close(sockfd);
}

