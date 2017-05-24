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
int connectToAgent(int);
std::string receiveInformation(int, char*);
int sendInformation(int, char*);
int closeConnectionWithAgent(int);
void waitForAlarm();
int socketAlarmInitialization(int);
int newAlarm(int, int);
void waitForXML(int, char*);
int chooseAgent();

std::atomic<int> numberOfAgentConnected (0);
std::atomic<bool> systemIsRunning (true);

int main(void)
{
	int systemSocket;
	char buffer[1024];
	int activity;	
	int agentNo;
	bool agentConnected = false;
	std::string request;
	std::thread reader (waitForAlarm);
	
	welcomeScreen();
	
	do {
		activity = menu();
		
		if(activity == CONNECT_WITH_AGENT)
		{
			if (agentConnected)
				std::cout << "You are connected with Agent" << std::endl;
			else
			{
				agentNo = chooseAgent();
				systemSocket = connectToAgent(agentNo);
				agentConnected = true;
			}
		}
		
		else if(activity == SEND_REQUEST_TO_AGENT)
		{
			if(agentConnected)
				sendInformation(systemSocket, buffer);
			else
				std::cout << "Firstly connect with Agent" << std::endl;
		}

		else if(activity == CLOSE_CONNECTION_WITH_AGENT)
		{
			if(agentConnected)
			{
				request = "quit";
				strcpy(buffer, request.c_str());
				send(systemSocket,buffer,request.size()+1,0);
				closeConnectionWithAgent(systemSocket);
				agentConnected = false;
			}
			else
				std::cout << "You are not connected wit Agent" << std::endl;
		}
		
		else
			activity = END_WORK;

	} while(activity != END_WORK);

	if(agentConnected)
	{
		closeConnectionWithAgent(systemSocket);
	}
	
	systemIsRunning = false;
	sleep(1);
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
	std::cout << "3 - Disconnect with Agent" << std::endl;
	std::cout << std::endl;
	std::cout << "Activity: ";
	std::cin >> activity;
	
	return activity;
}

int connectToAgent(int agentNo)
{
	int systemSocket;
	std::string agentAddress;
	struct sockaddr_in6 agentAddr;    					
	socklen_t addr_size;
	
	systemSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if(systemSocket == -1)
	{
		perror("opening stream socket");
		exit(1);
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
	{
		perror("Conncection failed!");
		exit(1);
	}	
  
	if(connect(systemSocket, (struct sockaddr *) &agentAddr, sizeof agentAddr) == -1)
	{
		printf("Connection failed!\n");
		return 1; 
	}
	else
		std::cout << "Connected!" << std::endl;

	return systemSocket;
}

std::string receiveInformation(int systemSocket, char* buffer)
{
	std::string information;
	if (recv(systemSocket, buffer, 1024, 0) == -1)
	{
		perror("Receiving problem");
		exit(1);
	}
	information = buffer;
	return information;
}

int sendInformation(int systemSocket, char* buffer)
{
	std::string request;
	
	std::cout << "Podaj akcje:\n1 - Rozpoczecie pomiaru\n2 - Zakonczenie pomiaru\nAktiwiti: ";
	std::cin >> request;
	if (request == "1")
	{
		request = "3 ";
		std::string min, max;
		std::cout<<"Podaj minimalna oraz maksymalna temperature: ";
		std::cin>>min>>max;
		request += min;
		request += " ";
		request += max;
		numberOfAgentConnected += 1;
	}
	else if (request == "2")
	{
		request = "4";
	}
	else 
		return 0;

	strcpy(buffer, request.c_str());
	send(systemSocket,buffer,request.size()+1,0);

	receiveInformation(systemSocket, buffer);

	if (request == "4")
	{
		printf("Waiting for statistics...\n");
		//receiveInformation(systemSocket, buffer);
		printf("Statistics received!");
	}

	return 0;
}

int closeConnectionWithAgent(int systemSocket)
{
	if (close(systemSocket) == -1)
	{
		perror("Closing problem!!!");
		exit(1);
	}
	return 0;
}

void waitForAlarm()
{
	int alarmSocket, alarmAppearSocket;
	char buffer[1024];
	int errorRate = 0;
	std::string msg;

	while (systemIsRunning)
	{
		if(numberOfAgentConnected > 0)
		{
			std::cout << "1\n";
			alarmSocket = socketAlarmInitialization(alarmSocket);
			std::cout << "2\n";
			alarmAppearSocket = newAlarm(alarmAppearSocket, alarmSocket);
			std::cout << "3\n";
	
			do {
				msg = receiveInformation(alarmAppearSocket, buffer);
				std::cout << "msg\n";
				if(msg == "up\n")
				{
					errorRate += 1;
				}
				else if(msg == "down\n")
				{
					errorRate += 1;
				}
				else if(msg == "ok\n")
					errorRate = 0;
				else if(msg == "xml\n")
				{
					waitForXML(alarmAppearSocket, buffer);
					numberOfAgentConnected -= 1;

				}
				if(errorRate > 10)
				{
					std::cout << "SHUTDOWN" << std::endl;
					numberOfAgentConnected = 0;
				}	
			} while(numberOfAgentConnected > 0);

			std::cout << "No Agents left\n";
	
			close(alarmAppearSocket);
			close(alarmSocket);
		}
	}
	exit(0);

}

int socketAlarmInitialization(int alarmSocket)
{
	struct sockaddr_in6 serverAddr;
	socklen_t addr_size;

	alarmSocket = socket(AF_INET6, SOCK_STREAM, 0);
	if (alarmSocket == -1)
	{
		perror("opening stream socket");
		exit(1);
	}
	memset(&serverAddr, 0, sizeof(serverAddr));				
	serverAddr.sin6_family = AF_INET6;					
	serverAddr.sin6_port = htons(PORT_ALARM);
	serverAddr.sin6_scope_id = LOCAL_INTERFACE_INDEX;
	inet_pton(AF_INET6,IPV6_SYSTEM, &serverAddr.sin6_addr);

	if (bind(alarmSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
	{
		perror("binding stream socket");
		exit(1);
	}

	if(listen(alarmSocket,5)==0)
		;
	else
		printf("Error: listen function\n");

	return alarmSocket;
}

int newAlarm(int alarmAppearSocket, int alarmSocket)
{
	alarmAppearSocket = accept(alarmSocket, NULL, NULL);
	if (alarmAppearSocket == -1)
	{
		perror("accept");
		exit(1);
	}
	
	return alarmAppearSocket;
}

void waitForXML(int alarmAppearSocket, char* buffer)
{
	std::string xmlFile;
	if (recv(alarmAppearSocket, buffer, 1024, 0) == -1)
	{
		perror("Receiving problem");
		exit(1);
	}
	xmlFile = buffer;
	std::cout << "Pakiet xml: " << xmlFile << std::endl;
	std::ofstream o("data.xml");
	o << xmlFile << std::endl;
	o.close();
}

int chooseAgent()
{
	int agent;
	do {	

		std::cout << std::endl;
		std::cout << "Wybierza agenta - 1,2 lub 3" << std::endl;
		std::cin >> agent;
	}while(agent != 1 and agent != 2 and agent != 3);

	return agent;	

}

