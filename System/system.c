#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int menu();
int connectToAgent();
int receiveInformation(int, char*);
int sendInformation(int, char*);
int closeConnectionWithAgent(int);

int main(void)
{
	int systemSocket;
	char buffer[1024];	
	
	menu();
	systemSocket = connectToAgent();
	receiveInformation(systemSocket,buffer);
	sendInformation(systemSocket, buffer);
	closeConnectionWithAgent(systemSocket);
	return 0;
}

int menu()
{
	printf("\n");
	printf("*-------------------------------------------------*\n");
	printf("*------------ Witaj w systemie SZPON! ------------*\n");
	printf("*-------------------------------------------------*\n");
	printf("\n");
	printf("*** Terminal roboczy ***");
	printf("1 - uruchom pomiar");
	return 0;
}

int connectToAgent()
{
	int systemSocket;
	struct sockaddr_in6 agentAddr;    					
	socklen_t addr_size;
	
	systemSocket = socket(AF_INET6, SOCK_STREAM, 0);			
  
	/* Uzupełnianie struktury sockaddr_in6 */
	memset(&agentAddr, 0, sizeof(agentAddr));				
	agentAddr.sin6_family = AF_INET6;					   
	agentAddr.sin6_port = htons(7777);
	agentAddr.sin6_scope_id = 2;					
	inet_pton(AF_INET6,"fe80::a00:27ff:fedf:2f0b", &agentAddr.sin6_addr);	
  
	if(connect(systemSocket, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("connection failed\n");
		return 1; 
	}

	return systemSocket;
}

int receiveInformation(int systemSocket, char* buffer)
{
	
	recv(systemSocket, buffer, 1024, 0);
	printf("Data received: %s",buffer);
	return 0;
}

int sendInformation(int systemSocket, char* buffer)
{
	strcpy(buffer,"Herlo World\n");
	send(systemSocket,buffer,13,0);
	return 0;
}

int closeConnectionWithAgent(int systemSocket)
{
	close(systemSocket);
	return 0;
}

