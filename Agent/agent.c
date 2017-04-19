#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int agentInitialization(int, char*);
int closeAgent(int);
int newConnection(int, int);
int receiveInformation(int, char*);
int sendInformation(int, char*);
int closeConnection(int);

int main(){
	int mainSocket, newSocket;
	char buffer[1024];

	mainSocket = agentInitialization(mainSocket, buffer);
	newSocket = newConnection(newSocket, mainSocket);

	sendInformation(newSocket, buffer);
	receiveInformation(newSocket, buffer);

	closeConnection(newSocket);
	closeAgent(mainSocket);
}

int agentInitialization(int mainSocket, char* buffer)
{
	struct sockaddr_in6 serverAddr;
	socklen_t addr_size;

	mainSocket = socket(AF_INET6, SOCK_STREAM, 0);
	
	memset(&serverAddr, 0, sizeof(serverAddr));				
	serverAddr.sin6_family = AF_INET6;					
	serverAddr.sin6_port = htons(7777);
	serverAddr.sin6_scope_id = 2;					
	inet_pton(AF_INET6,"fe80::a00:27ff:fedf:2f0b", &serverAddr.sin6_addr);

	bind(mainSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	if(listen(mainSocket,5)==0)
		printf("Listening\n");
	else
		printf("Error\n");

	return mainSocket;
}

int newConnection(int newSocket, int mainSocket)
{
	newSocket = accept(mainSocket, NULL, NULL);
	return newSocket;
}

int receiveInformation(int newSocket, char* buffer)
{
	
	recv(newSocket, buffer, 1024, 0);
	printf("Data received: %s",buffer);
	return 0;
}

int sendInformation(int newSocket, char* buffer)
{
	strcpy(buffer,"Herlo World\n");
	send(newSocket,buffer,13,0);
	return 0;
}

int closeConnection(int newSocket)
{
	close(newSocket);
}

int closeAgent(int mainSocket)
{
	close(mainSocket);
	exit(0);
}

