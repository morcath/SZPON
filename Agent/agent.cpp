#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <thread>

/*Tymczasowo*/
#define IPV6_AGENT "fe80::eea3:3ace:f5bd:af93"//jarcin:"fe80::a00:27ff:fee9:fd39" 
#define PORT_AGENT 7777
#define LOCAL_INTERFACE_INDEX 2

int agentInitialization(int, char*);
int closeAgent(int);
int newConnection(int, int);
std::string receiveInformation(int, char*);
int sendInformation(int, char*);
int closeConnection(int);
void scanDocument();

int main(){
	int mainSocket, newSocket;
	char buffer[1024];
	std::string msg;
	//reset wartosci w pliku przy uruchomieniu systemu
	std::ofstream o("src/in");
	o << "0" << std::endl;
	o.close();
	std::ofstream p("src/out");
	p << "0" << std::endl;
	p.close();
	std::thread reader (scanDocument);
	
	mainSocket = agentInitialization(mainSocket, buffer);
	newSocket = newConnection(newSocket, mainSocket);
	
	do {
		msg = receiveInformation(newSocket, buffer);
	} while (msg != "quit");

	closeConnection(newSocket);
	closeAgent(mainSocket);
}

int agentInitialization(int mainSocket, char* buffer)
{
	struct sockaddr_in6 serverAddr;
	socklen_t addr_size;

	mainSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if (mainSocket == -1)
	{
		perror("opening stream socket");
		exit(1);
	}
	memset(&serverAddr, 0, sizeof(serverAddr));				
	serverAddr.sin6_family = AF_INET6;					
	serverAddr.sin6_port = htons(PORT_AGENT);
	serverAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;
	inet_pton(AF_INET6,IPV6_AGENT, &serverAddr.sin6_addr);

	if (bind(mainSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
	{
		perror("binding stream socket");
		exit(1);
	}

	if(listen(mainSocket,5)==0)
		printf("Waiting...\n");
	else
		printf("Error: listen function\n");

	return mainSocket;
}

int newConnection(int newSocket, int mainSocket)
{
	newSocket = accept(mainSocket, NULL, NULL);
	if (newSocket == -1)
	{
		perror("accept");
		exit(1);
	}
	
	return newSocket;
}

std::string receiveInformation(int newSocket, char* buffer)
{
	std::string answer;
	if (recv(newSocket, buffer, 1024, 0) == -1)
	{
		perror("Receiving!");
		exit(1);
	}
	answer = buffer;
	std::ofstream o("src/in");
	std::cout << answer << std::endl;
	o << answer << std::endl;
	o.close();
	sendInformation(newSocket, buffer);
	return answer;
}

int sendInformation(int newSocket, char* buffer)
{
	strcpy(buffer,"Received!\n");
	if (send(newSocket,buffer,13,0) == -1)
	{
		perror("Problem with sending information about receiving");
		exit(1);
	}
	return 0;
}

int closeConnection(int newSocket)
{
	if (close(newSocket) == -1)
	{
		perror("Closing problem!!!");
		exit(1);
	}
}

int closeAgent(int mainSocket)
{
	if (close(mainSocket) == -1)
	{
		perror("Closing problem!!!");
		exit(1);
	}
	exit(0);
}

void scanDocument()
{
	std::cout<<"Jestem sobie nowym watkiem, nananananana";
	do{
		std::ifstream fileToRead;
		fileToRead.open("src/out");
		char output;
		if (fileToRead.is_open())
		{
			fileToRead >> output;
			if (output == '1')
				;//TU KOMUNIKAT O TYM ŻE JEST ZA GORĄCO I TRZEBA CHŁODZIĆ
			else if (output == '2')
				;//TU KOMUNIKAT O TYM ŻE JEST ZA ZIMNO I TRZEBA GRZAĆ
			else if (output == '3')
				;//TU KOMUNIKAT O TYM ŻE JEST JUŻ OK I ŻEBY SERWER PRZESTAŁ ODLICZAĆ DO EMERGENCY SHUTDOWN
			else if (output == '4')
				;//TU WYSŁANIE PLIKU xml
		}
		fileToRead.close();
		sleep (1);
		std::cout<<"Zyje!";
		//std::this_thread::sleep_for (std::chrono::seconds(1));
	}while(1);
}

