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
#include <sstream>
#include <atomic>
#include <exception>

/*STATIC IPv6 ADDRESS*/
#define IPV6_AGENT "fe80::eea3:3ace:f5bd:af93"//jarcin:"fe80::a00:27ff:fee9:fd39" 
#define IPV6_SYSTEM "fe80::eea3:3ace:f5bd:af93"
#define PORT_AGENT 7777
#define PORT_ALARM 8888
#define LOCAL_INTERFACE_INDEX 2

void resetParameters(std::string whichParam);
int agentInitialization(int mainSocket, char* buffer);
int newConnection(int newSocket, int mainSocket);
std::string receiveInstructions(int socket, char* buffer);
int sendMsg(int socket, char* buffer);
std::string receiveMsg(int socket, char* buffer);
int closeSocketSafe(int socket);

void sendToSystem();
int setSendSocket();
int sendInstructions(int socket, char* buffer, std::string info);
void sendFile(int socket, char* buffer);


std::atomic<bool> connectedWithSystem (false);
std::atomic<bool> runAgent (true);

int main()
{
	int mainSocket, newSocket;
	char buffer[1024];
	std::string msg;	
	
	resetParameters("all");
	mainSocket = agentInitialization(mainSocket, buffer);
	std::thread reader (sendToSystem);	

	while(runAgent)
	{
		std::cout << "Waiting for Serwer...\n";
		//resetParameters("all");
		newSocket = newConnection(newSocket, mainSocket);
		connectedWithSystem = true;
		std::cout << "Connected!\n";

		do {
			msg = receiveInstructions(newSocket, buffer);

			if(msg == "end")
				runAgent = false;

		} while (msg != "quit\n");

		
		closeSocketSafe(newSocket);
		connectedWithSystem = false;
		std::cout << "Disconnect!\n";
	}

	sleep(1);
	reader.join();
	closeSocketSafe(mainSocket);

}

void resetParameters(std::string whichParam)
{
	if(whichParam == "all" or whichParam == "in")
	{
		std::ofstream o("src/in");
		o << "0" << std::endl;
		o.close();
	}
	if(whichParam == "all" or whichParam == "out")
	{
		std::ofstream p("src/out");
		p << "0" << std::endl;
		p.close();
	}
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

	if(listen(mainSocket,5) != 0)
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

std::string receiveInstructions(int socket, char* buffer)
{
	std::string instructions;

	instructions = receiveMsg(socket, buffer);

	if(instructions == "quit\n" or instructions == "end")
		return instructions;

	std::ofstream o("src/in");
	o << instructions << std::endl;
	o.close();

	if(instructions[0] == '3')
		strcpy(buffer,"Startstat\n");
	else if(instructions[0] == '4')
		strcpy(buffer,"Endstat\n");

	sendMsg(socket, buffer);
	
	return instructions;	
}

int sendMsg(int socket, char* buffer)
{
	int sendSize;
	int msgSize;
	sendSize = send(socket,buffer,1024,0);
	msgSize = 1024;
	
	if(sendSize == -1)
	{
		perror("send message");
		exit(1);
	}
	
	std::cout << "WAŻNE!!!!    " << msgSize << "=?=" << sendSize << std::endl;
	if(sendSize != msgSize)
		exit(0);

	return 0;
}

std::string receiveMsg(int socket, char* buffer)
{
	std::string message;

	if (recv(socket, buffer, 1024, 0) == -1)
	{
		perror("Receiving problem");
		exit(1);
	}
	
	message = buffer;
	return message;
}

int closeSocketSafe(int socket)
{
	if (close(socket) == -1)
	{
		perror("Closing problem!!!");
		exit(1);
	}
	return 0;
}

void sendToSystem()
{
	int sendSocket;
	char buffer[1024];

	do{
		std::ifstream fileToRead;
		fileToRead.open("src/out");
		char output;
		
		if (fileToRead.is_open())
		{
			fileToRead >> output;
			if (output == '1')
			{
				sendSocket = setSendSocket();	
				sendInstructions(sendSocket, buffer, "up\n");
				closeSocketSafe(sendSocket);
			}
			else if (output == '2')
			{
				sendSocket = setSendSocket();
				sendInstructions(sendSocket, buffer, "down\n");
				closeSocketSafe(sendSocket);
			}
			else if (output == '3')
			{
				sendSocket = setSendSocket();
				sendInstructions(sendSocket, buffer, "ok\n");
				resetParameters("out");
				closeSocketSafe(sendSocket);
			}
			else if (output == '4')
			{
				sendSocket = setSendSocket();
				sendInstructions(sendSocket, buffer, "xml1\n");
				sleep(1);
				sendFile(sendSocket, buffer);
				resetParameters("all");
				closeSocketSafe(sendSocket);
			}
		}

		fileToRead.close();
		sleep(1);
	}while(runAgent);	


}

int setSendSocket()
{
	int socketIni;
	struct sockaddr_in6 agentAddr;    					
	socklen_t addr_size;
	
	socketIni = socket(AF_INET6, SOCK_STREAM, 0);
	if(socketIni == -1)
	{
		perror("opening stream socket");
		exit(1);
	}			
  
	/* Uzupełnianie struktury sockaddr_in6 */
	memset(&agentAddr, 0, sizeof(agentAddr));				
	agentAddr.sin6_family = AF_INET6;					   
	agentAddr.sin6_port = htons(PORT_ALARM);
	agentAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;
	inet_pton(AF_INET6,IPV6_SYSTEM, &agentAddr.sin6_addr);
	
  
	if(connect(socketIni, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("Connection failed!\n");
		return 1; 
	}
	else
		std::cout << "Connected!" << std::endl;

	return socketIni;
}

int sendInstructions(int socket, char* buffer, std::string info)
{
	strcpy(buffer,info.c_str());
	sendMsg(socket, buffer);
	return 0;
}

void sendFile(int socket, char* buffer)
{
	std::ifstream o("src/data.xml");
	std::string xmlFile;
	std::string xmlLine;
	
	while(!o.eof())
	{
		getline(o,xmlLine);
		xmlLine.append("\n");
		xmlFile.append(xmlLine);
	}

	std::cout << xmlFile << std::endl;
	o.close();
	strcpy(buffer, xmlFile.c_str());
	
	sendMsg(socket, buffer);
}
