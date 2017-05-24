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

void resetParameters();
int agentInitialization(int, char*);
int closeAgent(int);
int newConnection(int, int);
std::string receiveInformation(int, char*);
int sendInformation(int, char*);
int sendInstructions(int, char*, std::string);
int closeConnection(int);
void scanDocument();
int setAlarmSocket();
void sendFile(int, char*);

std::atomic<bool> connectedWithSystem (false);


int main(){
	int mainSocket, newSocket;
	char buffer[1024];
	std::string msg;
	int x = 0;
	
	mainSocket = agentInitialization(mainSocket, buffer);	
	while(1)
	{
		std::cout << "GOGOGOG!!!\n";
		resetParameters();
	
		newSocket = newConnection(newSocket, mainSocket);

		std::thread reader (scanDocument);
	
		do {
			msg = receiveInformation(newSocket, buffer);
			sleep(1);
			if(msg != "quit")
				connectedWithSystem = true;
			std::cout << "siano!\n";
		} while (msg != "quit");

		std::cout << "FINISH!\n";

		connectedWithSystem = false;

		reader.join();
		closeConnection(newSocket);

		std::cout << "FINISH!\n";
	}
	closeAgent(mainSocket);
}

void resetParameters()
{
	std::ofstream o("src/in");
	o << "0" << std::endl;
	o.close();
	std::ofstream p("src/out");
	p << "0" << std::endl;
	p.close();
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
	strcpy(buffer,"Received!\n");
	sendInformation(newSocket, buffer);
	return answer;
}

int sendInformation(int newSocket, char* buffer)
{
	if (send(newSocket,buffer,1024,0) == -1)
	{
		perror("Problem with sending information");
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
	int alarmSocket;
	char buffer[1024];
	int i;
	i = 0;
	std::cout<<"Jestem sobie nowym watkiem, nananananana";
	alarmSocket = setAlarmSocket();
	do{
		std::ifstream fileToRead;
		fileToRead.open("src/out");
		char output;
		if (fileToRead.is_open())
		{
			fileToRead >> output;
			if (output == '1')	
				sendInstructions(alarmSocket, buffer, "up\n");
			else if (output == '2')
				sendInstructions(alarmSocket, buffer, "down\n");
			else if (output == '3')
				sendInstructions(alarmSocket, buffer, "ok\n");
			else if (output == '4' and i == 0)
			{
				i = 1;
				sendInstructions(alarmSocket, buffer, "xml\n");
				std::cout << "NO ELO\n";
				sleep(1);
				sendFile(alarmSocket, buffer);
			}
		}
		fileToRead.close();
		sleep (1);
		std::cout<<"Zyje!";
		//std::this_thread::sleep_for (std::chrono::seconds(1));
	}while(!i);

}

int setAlarmSocket()
{
	int alarmSocket;
	struct sockaddr_in6 agentAddr;    					
	socklen_t addr_size;
	
	alarmSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if(alarmSocket == -1)
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

	while(!connectedWithSystem)
		;	
  
	if(connect(alarmSocket, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("Connection failed!\n");
		return 1; 
	}
	else
		std::cout << "Connected!" << std::endl;

	return alarmSocket;
}

int sendInstructions(int alarmSocket, char* buffer, std::string info)
{
	strcpy(buffer,info.c_str());
	sendInformation(alarmSocket, buffer);
}

void sendFile(int alarmSocket, char* buffer)
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
	
	sendInformation(alarmSocket, buffer);
}

