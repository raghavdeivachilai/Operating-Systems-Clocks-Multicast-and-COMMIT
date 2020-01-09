//Raghav Deivchilai
//ZG7051
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#define p1 9060
#define p2 9063
#define p3 9074

char sendBuffer[1024];
char recvBuffer[1024];

int main(int argc, char *argv[])
{
	//Bonus Assignment File locking and updating
	printf("hello");
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&lock);
	int file_value;
	FILE *f = fopen("/home/raghav/Documents/OperatingSystems-Clocks-Multicast-and-Commit/filecounter.txt", "r+");
	if (f == NULL)
	{
		printf("Cannot open file to read");
		return 0;
	}
	else
	{
		char buffer[256];
		fgets(buffer, sizeof(buffer), f);
		file_value = atoi(buffer);
		printf("%d",file_value);
		printf("Current value in file: %d ", file_value);
		file_value++;
	}
	fclose(f);
	FILE *f1 = fopen("/home/raghav/Documents/OperatingSystems-Clocks-Multicast-and-Commit/filecounter.txt", "w+");
	if (f1 == NULL)
	{
		printf("Cannot open file to write");
		return 0;
	}
	else
	{
		// printf("hii");
		fprintf(f1, "%d\n", file_value);
		// printf("%d",file_value);
		printf("Updated value: %d", file_value);
	}
	fclose(f1);
	pthread_mutex_unlock(&lock);

	uint32_t portList[3] = {p1, p2, p3};
	uint8_t clocks[3] = {0, 0, 0};
	int total = 0;

	size_t numberOfPorts = 3;
	uint16_t port = atoi(argv[1]);

	time_t timer;
	srand((unsigned)time(&timer));
	int clock = rand() % 20; // generates a random number between 1 and 20

	/* Check the User Provided Process ID, if it is number 0, then it is the Time Daemon */
	if (atoi(argv[2]) == 0)
	{
		printf("\nI am the Time Daemon\n");

		for (int processIndex = 0; processIndex < numberOfPorts; ++processIndex)
		{
			struct sockaddr_in serverAddress;
			/*Set the server's parameters such as host address and port number.*/
			memset(&serverAddress, '\0', sizeof(struct sockaddr_in));

			serverAddress.sin_family = AF_INET;
			serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddress.sin_port = htons(portList[processIndex]);
			int sockfd = -1;
			int opt = 1;
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

			memset(&serverAddress, '\0', sizeof(serverAddress));

			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(portList[processIndex]);

			inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

			connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

			// printf("Sending Request...\n");
			memset(recvBuffer, '\0', sizeof(recvBuffer));

			strcpy(sendBuffer, "clock: 1001");  // value to be sent
			write(sockfd, sendBuffer, strlen(sendBuffer)); // sending operation
			// Get response...
			memset(recvBuffer, '\0', sizeof(recvBuffer));
			read(sockfd, recvBuffer, sizeof(recvBuffer) - 1); // reading response
			printf("\nResponse for clock value: %s\n", recvBuffer);
			sleep(2);
			clocks[processIndex] = atoi(recvBuffer);
			total += clocks[processIndex];
		}
		// All clock values have been received at this point then calculate average
		int average = (total / numberOfPorts);

		// new request to send updated clock offset values
		for (int processIndex = 0; processIndex < numberOfPorts; ++processIndex)
		{
			/* Initiate Server Parameters */
			struct sockaddr_in serverAddress;
			memset(&serverAddress, '\0', sizeof(struct sockaddr_in));

			serverAddress.sin_family = AF_INET;
			serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddress.sin_port = htons(portList[processIndex]);
			int sockfd = -1;
			int opt = 1;
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

			memset(&serverAddress, '\0', sizeof(serverAddress));

			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(portList[processIndex]);

			inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

			connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
			clock = average - clocks[processIndex]; // calculate clock offset
			memset(recvBuffer, '\0', sizeof(recvBuffer));
			printf("\nnew clock offset: %d\n", clock);
			sprintf(sendBuffer, "set_timestamp: %d", clock);				  // new value to be sent...
			int numBytes = write(sockfd, sendBuffer, strlen(sendBuffer)); // sending operation

			// Get response...
			memset(recvBuffer, '\0', sizeof(recvBuffer));
			numBytes = read(sockfd, recvBuffer, sizeof(recvBuffer) - 1); // reading response
			printf("\nServer Response for clock offset: %s\n", recvBuffer);
			sleep(2);
		}
	}
	else
	{

		const int CONN_BACKLOG_NUM = 1; // Max length of queue of pending connections for var sockfd.

		/* Initiate Server Parameters */
		struct sockaddr_in myAddr;
		memset(&myAddr, '\0', sizeof(struct sockaddr_in));

		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myAddr.sin_port = htons(port);

		while (1)
		{
			int sockfd = -1;
			int opt = 1;
			sockfd = socket(AF_INET, SOCK_STREAM, 0);

			//set master socket to allow multiple connections , this is just a good habit, it will work without this
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

			//inform user of socket number - used in send and receive commands
			bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));

			listen(sockfd, CONN_BACKLOG_NUM);
			/* The accept call blocks until a connection is found */
			int connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);
			read(connfd, recvBuffer, sizeof(recvBuffer) - 1); // This is where server gets input from client
			char *token = strtok(recvBuffer, " ");			  

			/* Determine the type of request: is it a request for my current clock OR to set my clock? */
			if (strstr(token, "clock:"))
			{							   // This is the first request asking for my current clock
				token = strtok(NULL, " "); // fetch the next token in recvBuffer

				/* Data for client... */
				memset(sendBuffer, '\0', sizeof(sendBuffer)); // clear send buffer before sending response data
				sprintf(sendBuffer, "%d", clock);			  
				write(connfd, sendBuffer, strlen(sendBuffer));
				printf("\nMy Clock Value: %d\n", clock);
			}

			if (strstr(token, "set_timestamp:"))
			{							   // This is the second request to update my clock.
				token = strtok(NULL, " "); // fetch the next token in recvBuffer, i.e. the <value> in [key: value]

				int offset = atoi(token); // convert new clock value to int
				clock = clock + offset;   // set new value for clock.
				*(&clock) = clock;

				/* Data for client... */
				memset(sendBuffer, '\0', sizeof(sendBuffer)); // clear send buffer before sending response data
				sprintf(sendBuffer, "%d", clock);
				write(connfd, sendBuffer, strlen(sendBuffer));
				printf("My Updated Clock Value After Berkeley Algorithm : %d\n", clock);
			}

			memset(recvBuffer, '\0', sizeof(recvBuffer)); // after retrieving value, reset recvBuffer for next request.

			close(sockfd);
		}
	}

	return 0;
}
/*
References:
https://www.geeksforgeeks.org/berkeleys-algorithm/
https://github.com/n0nzy/time-sync/blob/master/main.c
*/
