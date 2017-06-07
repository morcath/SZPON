#include <stdio.h>
#include <unistd.h>
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

/**
 * Serwer nadzorujący działanie agentów kontrolujących maszynę
 * @author Aleksander Tym
 * @zespol: Aleksander Tym, Marcin Janeczko, Aleksandra Rybak, Katarzyna Romasevska, Bartlomiej Przewozniak
 * @data: kwiecien-maj 2017
 * Program jest czescia projektu SZPON
 */

#define END_WORK 0
#define CONNECT_WITH_AGENT 1
#define SEND_REQUEST_TO_AGENT 2
#define CLOSE_CONNECTION_WITH_AGENT 3
#define INTEGRATION_TESTS 4
#define FULL_TESTS 5


/*STATIC IPv6 ADDRESS*/
#define IPV6_SYSTEM "92.60.136.194 "
#define IPV6_AGENT1 "92.60.136.194 "
#define IPV6_AGENT2 "92.60.136.194 "
#define IPV6_AGENT3 "92.60.136.194 "
#define PORT_AGENT 7777
#define PORT_ALARM 8888
#define LOCAL_INTERFACE_INDEX 2

void welcomeScreen();
int menu();
int chooseAgent(int agentNo);
int connectToAgent(int agentNo);
int sendMsg(int socket, char* buffer);
std::string receiveMsg(int socket, char* buffer);
int startInstruction(int socket, char* buffer, std::string min, std::string max);
int stopInstruction(int socket, char* buffer);
int sendInstructions(int socket, char* buffer);
std::string menuToInstructions();
int sendToAgentCloseConnection(int socket, char* buffer);
int closeSocketSafe(int socket);

int responseFromAgents();
int socketInitialization(int socketIni);
int waitForAgent (int responseSocket, int mainSocket);
void receiveXML(int alarmAppearSocket, char* buffer, std::string msg);

//testy
int test1(int systemSocket, char* buffer, int agentNo);
int test2(int systemSocket, char* buffer, int agentNo);
int test3(int systemSocket, char* buffer, int agentNo);
int test4(int systemSocket, char* buffer, int agentNo);
int test5(int systemSocket, char* buffer, int agentNo);
int test6(int systemSocket, char* buffer, int agentNo);

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
		else if(activity == INTEGRATION_TESTS)
		{
			test2(systemSocket, buffer, agentNo);
		}
		else if(activity == FULL_TESTS)
		{
			
			if (test1(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);

			if (test2(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);

			if (test3(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);

			/*if (test4(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);*/

			if (test5(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);

			if (test6(systemSocket, buffer, agentNo) == 0)
				break;
			sleep(1);

			
		}
		else
			activity = END_WORK;
		

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
	std::cout << "4 - Integration tests" << std::endl;
	std::cout << "5 - Full tests" << std::endl;
	std::cout << std::endl;
	std::cout << "Activity: ";
	std::cin >> menu;
	std::cout << std::endl;
	while(menu != "0" and menu != "1" and menu != "2" and menu != "3" and menu != "4" and menu != "5")
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
		std::cout << "Wybierz agenta - 1,2 lub 3" << std::endl;
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

int startInstruction(int socket, char* buffer, std::string min, std::string max)
{
	std::string request = "3 ";
	//std::string min = "5", max = "50";
	request += min;
	request += " ";
	request += max;

	strcpy(buffer, request.c_str());
	sendMsg(socket,buffer);

	request = receiveMsg(socket, buffer);
	if(request == "Startstat\n")
		numberOfConnectedAgents += 1;

	return 0;
}

int stopInstruction(int socket, char* buffer)
{
	std::string request = "4";

	strcpy(buffer, request.c_str());
	sendMsg(socket,buffer);

	request = receiveMsg(socket, buffer);
	if(request == "Endstat\n")
		return 0;
		
	return 1;
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
	int errorRate1 = 0;
	int errorRate2 = 0;
	int errorRate3 = 0;
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

				if(msg[0] == 'u')
				{
					if(msg [2] == '1')
					{
						errorRate1 += 1;
						errorRate = errorRate1;

						if(errorRate >= 10)
							errorRate1 = 0;

					}
					else if(msg [2] == '2')
					{
						errorRate2 += 1;
						errorRate = errorRate2;

						if(errorRate >= 10)
							errorRate2 = 0;

					}
					else if(msg [2] == '3')
					{
						errorRate3 += 1;
						errorRate = errorRate3;

						if(errorRate >= 10)
							errorRate3 = 0;

					}
					
					if (errorRate < 10)
					{		
						msg = "1";
						strcpy(buffer, msg.c_str());
						sendMsg(responseSocket, buffer);
					}
					else
					{
						std::cout << "SHUTDOWN" << std::endl;
						msg = "end\n";
						strcpy(buffer, msg.c_str());
						sendMsg(responseSocket, buffer);
						numberOfConnectedAgents -= 1;
						closeSocketSafe(responseSocket);
						break;
					}	
				}
				else if(msg[0] == 'd')
				{

					if(msg [4] == '1')
					{
						errorRate1 += 1;
						errorRate = errorRate1;

						if(errorRate >= 10)
							errorRate1 = 0;

					}
					else if(msg [4] == '2')
					{
						errorRate2 += 1;
						errorRate = errorRate2;

						if(errorRate >= 10)
							errorRate2 = 0;

					}
					else if(msg [4] == '3')
					{
						errorRate3 += 1;
						errorRate = errorRate3;

						if(errorRate >= 10)
							errorRate3 = 0;

					}					

					if(errorRate < 10)
					{
						msg = "2";
						strcpy(buffer, msg.c_str());
						sendMsg(responseSocket, buffer);
					}
					else
					{
						std::cout << "SHUTDOWN" << std::endl;
						msg = "end\n";
						strcpy(buffer, msg.c_str());
						sendMsg(responseSocket, buffer);
						numberOfConnectedAgents -= 1;
						closeSocketSafe(responseSocket);
						break;
					}
				}
				else if(msg[0] == 'o')
				{
					if(msg [2] == '1')
						errorRate1 = 0;
					else if(msg [2] == '2')
						errorRate2 = 0;
					else if(msg [2] == '3')
						errorRate3 = 0;
					

					msg = "3";
					strcpy(buffer, msg.c_str());
					sendMsg(responseSocket, buffer);
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

	std::ofstream o(nameFile);
	
	while(true)
	{
		xmlFile = receiveMsg(socket, buffer);
		if(xmlFile == "endXml\n")
			break;

		o << xmlFile;
	}
	

	o.close();
}

int test1(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 1: Connect and Disconnect\n" << std::endl;

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "1. Connect = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect = NEGATIVE" << std::endl;
		return 0;
	}
		
	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "2. Disconnect = POSITIVE" << std::endl;
	}
	else{
		std::cout << "2. Disconnect = NEGATIVE" << std::endl;
		return 0;
	}
	return 1;
}

int test2(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 2: connect, start measurement, stop measurement, disconnect\n" << std::endl;

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "1. Connect = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "5", "50") == 0)
		std::cout << "2. Start measurement = POSITIVE" << std::endl;
	else{
		std::cout << "2. Start measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "3. Stop measurement = POSITIVE" << std::endl;
	else{
		std::cout << "3. Stop measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "4. Disconnect = POSITIVE" << std::endl;
	}
	else{
		std::cout << "4. Disconnect = NEGATIVE" << std::endl;
		return 0;
	}
	return 1;
}

int test3(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 3: connect, start measurement, warning, stop measurement, disconnect\n" << std::endl;

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "1. Connect = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "10", "15") == 0)
		std::cout << "2. Start measurement = POSITIVE" << std::endl;
	else{
		std::cout << "2. Start measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(20);

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "3. Stop measurement = POSITIVE" << std::endl;
	else{
		std::cout << "3. Stop measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "4. Disconnect = POSITIVE" << std::endl;
	}
	else{
		std::cout << "4. Disconnect = NEGATIVE" << std::endl;
		return 0;
	}
	return 1;
}

int test4(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 4: Connect with 2 agents\n" << std::endl;

	systemSocket = connectToAgent(2);
	if (systemSocket != -1)
		std::cout << "1. Connect Agent 2 = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "10", "20") == 0)
		std::cout << "2. Start measurement Agent 2 = POSITIVE" << std::endl;
	else{
		std::cout << "2. Start measurement Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}
	sleep(3);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "3. Disconnect Agent 2 = POSITIVE" << std::endl;
	}
	else{
		std::cout << "3. Disconnect Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}


	sleep(1);

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "4. Connect Agent 1 = POSITIVE" << std::endl;
	else {
		std::cout << "4. Connect Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "10", "20") == 0)
		std::cout << "5. Start measurement Agent 1 = POSITIVE" << std::endl;
	else{
		std::cout << "5. Start measurement Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}
	sleep(3);
	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "6. Disconnect Agent 1 = POSITIVE" << std::endl;
	}
	else{
		std::cout << "6. Disconnect Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(20);

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "7. Connect Agent 1 = POSITIVE" << std::endl;
	else {
		std::cout << "7. Connect Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "8. Stop measurement Agent 1 = POSITIVE" << std::endl;
	else{
		std::cout << "8. Stop measurement Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "9. Disconnect Agent 1 = POSITIVE" << std::endl;
	}
	else{
		std::cout << "9. Disconnect Agent 1 = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(3);

	systemSocket = connectToAgent(2);
	if (systemSocket != -1)
		std::cout << "10. Connect Agent 2 = POSITIVE" << std::endl;
	else {
		std::cout << "10. Connect Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "11. Stop measurement Agent 2 = POSITIVE" << std::endl;
	else{
		std::cout << "11. Stop measurement Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "12. Disconnect Agent 2 = POSITIVE" << std::endl;
	}
	else{
		std::cout << "12. Disconnect Agent 2 = NEGATIVE" << std::endl;
		return 0;
	}

	return 1;
}

int test5(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 5: almost SHUTDOWN\n" << std::endl;

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "1. Connect = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "10", "11") == 0)
		std::cout << "2. Start measurement = POSITIVE" << std::endl;
	else{
		std::cout << "2. Start measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(3);

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "3. Stop measurement = POSITIVE" << std::endl;
	else{
		std::cout << "3. Stop measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "4. Disconnect = POSITIVE" << std::endl;
	}
	else{
		std::cout << "4. Disconnect = NEGATIVE" << std::endl;
		return 0;
	}
	return 1;
}

int test6(int systemSocket, char* buffer, int agentNo){
	std::cout << "\nScenario 5: SHUTDOWN\n" << std::endl;

	systemSocket = connectToAgent(1);
	if (systemSocket != -1)
		std::cout << "1. Connect = POSITIVE" << std::endl;
	else {
		std::cout << "1. Connect = NEGATIVE" << std::endl;
		return 0;
	}

	if(startInstruction(systemSocket, buffer, "10", "11") == 0)
		std::cout << "2. Start measurement = POSITIVE" << std::endl;
	else{
		std::cout << "2. Start measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(20);

	if(stopInstruction(systemSocket, buffer) == 0)
		std::cout << "3. Stop measurement = POSITIVE" << std::endl;
	else{
		std::cout << "3. Stop measurement = NEGATIVE" << std::endl;
		return 0;
	}

	sleep(5);

	sendToAgentCloseConnection(systemSocket, buffer);
	if (closeSocketSafe(systemSocket) == 0){
		agentNo = 0;
		std::cout << "4. Disconnect = POSITIVE" << std::endl;
	}
	else{
		std::cout << "4. Disconnect = NEGATIVE" << std::endl;
		return 0;
	}
	return 1;
}
