/*
Janicecia Nobles
winsock chatServer
*/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "25001"

int main(){

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	char name[256];
	char clientName[256];

    WSADATA wsaData;
	//S1// Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

	//The addrinfo structure is used by the getaddrinfo function.
	struct addrinfo *result = NULL, *ptr = NULL, hints;
   
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0){
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
  
	SOCKET ListenSocket = INVALID_SOCKET;
	//S2// Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //S3 Bind the socket// Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
	//free the memory allocated by the getaddrinfo function
    freeaddrinfo(result); 

	(void) printf("Enter server name: ");
	(void) scanf("%s", name);

	(void) printf("\nChat Server Waiting For Connection...\n");

	//S4// listen on a socket for a client
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

	//S5// Accept a client socket
	SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    // No longer need server socket
    closesocket(ListenSocket);

	(void) printf("Chat Connection Established.\n\n");

	//send server name
	send(ClientSocket, name, sizeof(name), 0);
	//get client name
	iResult =recv(ClientSocket, clientName, sizeof(clientName), 0);
	

    //S6// Receive and send data
	//Receive until the peer shuts down the connection
	char message[512];
	int turn =0;
	int start =-1;
	while(iResult!=SOCKET_ERROR)
	{
        while(message != "/q")
		{
			if (turn ==0){
				iResult =recv(ClientSocket, message, sizeof(message), 0);

				if(message[0] =='/' && message[1] =='q'){
					shutdown(ClientSocket, SD_BOTH);
					return 0;
				}
				if(strlen(message)>0)
					cout << clientName<< "> "<< message << endl;
				start++;
				turn =1;
			}
			if(turn ==1){
				if(start >0)
					cout <<name <<">";
				cin.getline(message, sizeof(message));
				if(message[0] =='/' && message[1] =='q'){
					shutdown(ClientSocket, SD_BOTH);
					return 0;
				}
				send(ClientSocket, message, recvbuflen, 0);
				turn =0;
			}
		
		}
	}
    //S7// shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    // cleanup
	printf("\nChat Closed!\n");
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
