#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <thread>
#include <atomic>

#define END_WORK 0
#define CONNECT_WITH_AGENT 1
#define SEND_REQUEST_TO_AGENT 2
#define CLOSE_CONNECTION_WITH_AGENT 3
#define CLOSE_AGENT 4

/*STATIC IPv6 ADDRESS*/
#define IPV6_SYSTEM "fe80::eea3:3ace:f5bd:af93"
#define IPV6_AGENT1 "fe80::eea3:3ace:f5bd:af93"//jarcin:"fe80::a00:27ff:fee9:fd39"
#define IPV6_AGENT2 "fe80::eea3:3ace:f5bd:af93"
#define IPV6_AGENT3 "fe80::eea3:3ace:f5bd:af93"
#define PORT_AGENT 7777
#define PORT_ALARM 8888
#define LOCAL_INTERFACE_INDEX 2

void welcomeScreen();
int menu();
int chooseAgent(int agentNo);
int connectToAgent(int agentNo);
int sendMsg(int socket, char* buffer);
std::string receiveMsg(int socket, char* buffer);
int sendInstructions(int socket, char* buffer);
std::string menuToInstructions();
int sendToAgentCloseConnection(int socket, char* buffer);
int closeSocketSafe(int socket);

int responseFromAgents();
int socketInitialization(int socketIni);
int waitForAgent (int responseSocket, int mainSocket);
void receiveXML(int alarmAppearSocket, char* buffer, std::string msg);

std::atomic<bool> systemIsRunning (true);
std::atomic<int> numberOfConnectedAgents (0);

int main(void)
{
	int systemSocket; 		//gniazdo systemowe do wydawania poleceń agentowm
	char buffer[1024]; 		//bufor z poleceniami dla agentów
	int activity; 			//pozycja wybrana z menu
	int agentNo = 0;		//identyfikator agenta, któremu system wydaje polecenia, 0-oznacza, że żaden agent nie jest podłączony

	welcomeScreen();		//ekran powitalny

	std::thread reader (responseFromAgents);

	// główna pętla programu
	do {
		activity = menu();

		if(activity == CONNECT_WITH_AGENT)
		{
			if(!agentNo)
			{
				agentNo = chooseAgent(agentNo);
				systemSocket = connectToAgent(agentNo);
				if(systemSocket == -1)
					agentNo = 0;
			}
			else
				std::cout << "You are connected with Agent!" << std::endl;		
		}
		else if(activity == SEND_REQUEST_TO_AGENT)
		{
			if(agentNo)
				sendInstructions(systemSocket, buffer);
			else
				std::cout << "You are not connected with Agent!" << std::endl;
		}
		else if(activity == CLOSE_CONNECTION_WITH_AGENT)
		{
			if(agentNo)
			{
				sendToAgentCloseConnection(systemSocket, buffer);
				closeSocketSafe(systemSocket);
				agentNo = 0;
			}
			else
				std::cout << "You are not connected wit Agent" << std::endl;
		}
		

	}while(activity);

	systemIsRunning = false;
	sleep(1);
	reader.join();
	std::cout << "Good bye!" << std::endl;
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
	std::string menu;
	
	std::cout << std::endl;
	std::cout << "           *** Terminal roboczy ***" << std::endl;
	std::cout << "0 - Exit" << std::endl;
	std::cout << "1 - Connect with Agent" << std::endl;
	std::cout << "2 - Send request to Agent" << std::endl;
	std::cout << "3 - Disconnect with Agent" << std::endl;
	std::cout << std::endl;
	std::cout << "Activity: ";
	std::cin >> menu;
	std::cout << std::endl;
	while(menu != "0" and menu != "1" and menu != "2" and menu != "3")
	{
		std::cout << "Activity: ";
		std::cin >> menu;
	}

	activity = atoi(menu.c_str());
	
	return activity;
}

int chooseAgent(int agentNo)
{
	std::string in;
	int agent;
	do {	

		std::cout << std::endl;
		std::cout << "Wybierza agenta - 1,2 lub 3" << std::endl;
		std::cin >> in;
	}while(in != "1" and in != "2" and in != "3");
	agent = atoi(in.c_str());

	return agent;	

}

int connectToAgent(int agentNo)
{
	int systemSocket;
	
	struct sockaddr_in6 agentAddr;    					
	socklen_t addr_size;
	
	systemSocket = socket(AF_INET6, SOCK_STREAM, 0);
	
	if(systemSocket == -1)
	{
		perror("opening stream socket");
		return -1;
	}
	
  
	/* Uzupełnianie struktury sockaddr_in6 */
	memset(&agentAddr, 0, sizeof(agentAddr));				
	agentAddr.sin6_family = AF_INET6;					   
	agentAddr.sin6_port = htons(PORT_AGENT);
	agentAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;

	if (agentNo == 1)
		inet_pton(AF_INET6,IPV6_AGENT1, &agentAddr.sin6_addr);
	else if (agentNo == 2)
		inet_pton(AF_INET6,IPV6_AGENT2, &agentAddr.sin6_addr);
	else if (agentNo == 3)
		inet_pton(AF_INET6,IPV6_AGENT3, &agentAddr.sin6_addr);
	else
		std::cout << "no way\n";
	  
	if(connect(systemSocket, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("Connection failed!\n");
		return -1; 
	}
	else
		std::cout << "Connected!" << std::endl;

	return systemSocket;
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

int sendInstructions(int socket, char* buffer)
{
	std::string request;
	
	request = menuToInstructions();
	
	if (request == "1")
	{
		request = "3 ";
		std::string min, max;
		std::cout<<"Podaj minimalna oraz maksymalna temperature: ";
		std::cin>>min>>max;
		request += min;
		request += " ";
		request += max;
	}
	else if (request == "2")
		request = "4";
	else 
		return 0;

	strcpy(buffer, request.c_str());
	sendMsg(socket,buffer);

	request = receiveMsg(socket, buffer);
	if(request == "Startstat\n")
		numberOfConnectedAgents += 1;

	return 0;
}

std::string menuToInstructions()
{
	std::string action;
	do {	

		std::cout << std::endl;
		std::cout << "Podaj akcje:\n1 - Rozpoczecie pomiaru\n2 - Zakonczenie pomiaru\nAktiwiti: ";
		std::cin >> action;
	}while(action != "1" and action != "2");
	
	return action;
}

int sendToAgentCloseConnection(int socket, char* buffer)
{
	std::string request;
	request = "quit\n";
	strcpy(buffer, request.c_str());
	sendMsg(socket, buffer);
	return 0;
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

int responseFromAgents()
{
	int mainSocket, responseSocket;
	char buffer[1024];
	int errorRate = 0;
	std::string msg;

	mainSocket = socketInitialization(mainSocket);

	while (systemIsRunning)
	{
		if(numberOfConnectedAgents)
		{

			do {
				responseSocket = waitForAgent(responseSocket, mainSocket);
				msg = receiveMsg(responseSocket, buffer);

				std::cout << msg << std::endl;

				if(msg == "up\n")
				{
					errorRate += 1;
					//@TODO chłodzenie		
				}
				else if(msg == "down\n")
				{
					errorRate += 1;
					//@TODO grzanie
				}
				else if(msg == "ok\n")
				{
					errorRate = 0;
					closeSocketSafe(responseSocket);
					break;
				}
	
				else if(msg[0] == 'x')
				{
					receiveXML(responseSocket, buffer, msg);
					closeSocketSafe(responseSocket);
					break;
				}
				closeSocketSafe(responseSocket);

				if(errorRate > 10)
				{
					std::cout << "SHUTDOWN" << std::endl;
					exit(1);
				}	
			} while(numberOfConnectedAgents > 0);

		}
	}

	closeSocketSafe(mainSocket);

	return 0;
}

int socketInitialization(int socketIni)
{
	struct sockaddr_in6 serverAddr;
	socklen_t addr_size;

	socketIni = socket(AF_INET6, SOCK_STREAM, 0);
	if (socketIni == -1)
	{
		perror("opening stream socket");
		exit(1);
	}
	memset(&serverAddr, 0, sizeof(serverAddr));				
	serverAddr.sin6_family = AF_INET6;					
	serverAddr.sin6_port = htons(PORT_ALARM);
	serverAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;
	inet_pton(AF_INET6,IPV6_SYSTEM, &serverAddr.sin6_addr);

	if (bind(socketIni, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
	{
		perror("binding stream socket");
		exit(1);
	}

	if(listen(socketIni,5) != 0)
		printf("Error: listen function\n");

	return socketIni;
}

int waitForAgent (int responseSocket, int mainSocket)
{
	responseSocket = accept(mainSocket, NULL, NULL);
	if (responseSocket == -1)
	{
		perror("accept");
		exit(1);
	}
	
	return responseSocket;
}

void receiveXML(int socket, char* buffer, std::string msg)
{
	numberOfConnectedAgents -= 1;
	
	std::string xmlFile;
	std::string nameFile = "data";
	nameFile += msg[3];
	nameFile.append(".xml");
	
	
	xmlFile = receiveMsg(socket, buffer);
	
	std::cout << "Pakiet xml: " << xmlFile << std::endl;
	
	std::ofstream o(nameFile);
	o << xmlFile << std::endl;
	o.close();
}
