#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define END_WORK 0
#define CONNECT_WITH_AGENT 1
#define SEND_REQUEST_TO_AGENT 2
#define CLOSE_CONNECTION_WITH_AGENT 3

/*Tymczasowo*/
#define IPV6_AGENT "fe80::a00:27ff:fedf:2f0b" 
#define PORT_AGENT 7777
#define LOCAL_INTERFACE_INDEX 2

void welcomeScreen();
int menu();
int connectToAgent();
int receiveInformation(int, char*);
int sendInformation(int, char*);
int closeConnectionWithAgent(int);

int main(void)
{
	int systemSocket;
	char buffer[1024];
	int activity;	
	
	welcomeScreen();
	do {
		activity = menu();
		if(activity == CONNECT_WITH_AGENT)
			systemSocket = connectToAgent();
		else if(activity == SEND_REQUEST_TO_AGENT)
			sendInformation(systemSocket, buffer);
		else
			activity = END_WORK;

	} while(activity != END_WORK);

	closeConnectionWithAgent(systemSocket);
	std::cout << "BYE!" << std::endl;
	return 0;
}

void welcomeScreen()
{
	std::cout << std::endl;
	std::cout << "*-------------------------------------------------*" << std::endl;
	std::cout << "*--------------- Welcome in SZPON! ---------------*" << std::endl;
	std::cout << "*-------------------------------------------------*" << std::endl;
}

int menu()
{
	int activity;
	std::cout << std::endl;
	std::cout << "           *** Terminal roboczy ***" << std::endl;
	std::cout << "0 - Exit" << std::endl;
	std::cout << "1 - Connect with Agent" << std::endl;
	std::cout << "2 - Send request to Agent" << std::endl;
	std::cout << std::endl;
	std::cout << "Activity: ";
	std::cin >> activity;
	
	return activity;
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
	agentAddr.sin6_port = htons(PORT_AGENT);
	agentAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;
	inet_pton(AF_INET6,IPV6_AGENT, &agentAddr.sin6_addr);	
  
	if(connect(systemSocket, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("Connection failed!\n");
		return 1; 
	}
	else
		std::cout << "Connected!" << std::endl;

	return systemSocket;
}

int receiveInformation(int systemSocket, char* buffer)
{
	
	recv(systemSocket, buffer, 1024, 0);
	printf("%s",buffer);
	return 0;
}

int sendInformation(int systemSocket, char* buffer)
{
	std::string request;
	do {
		std::cout << ">>> ";
		std::cin >> request;
		strcpy(buffer, request.c_str());
		send(systemSocket,buffer,request.size()+1,0);

		receiveInformation(systemSocket, buffer);
	} while (request != "quit");

	return 0;
}

int closeConnectionWithAgent(int systemSocket)
{
	close(systemSocket);
	return 0;
}

