/*
Janicecia Nobles
winsock chatClient
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

int main(int argc, char **argv){
	
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	char name[256];
	char serverName[256];

	// Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    WSADATA wsaData;
	//S1// Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

	//The addrinfo structure is used by the getaddrinfo function.
	struct addrinfo *result = NULL, *ptr = NULL, hints;
   
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if(iResult != 0){
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
  
	SOCKET ConnectSocket = INVALID_SOCKET;
	// keep trying to connect
	for(ptr=result;ptr != NULL ;ptr=ptr->ai_next){  

		//S2// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		//S3// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	(void) printf("\nChat Connection Established.\n");

	(void) printf("\nEnter client name: ");
	(void) scanf("%s", name); cout<<endl;

	//get server name
	iResult =recv(ConnectSocket, serverName, sizeof(serverName), 0);
	//Send client name
	send(ConnectSocket, name, sizeof(name), 0);

	//S4// Send and receive data
	char message[512];
	int turn =1;
	int start =0;
	while(iResult !=SOCKET_ERROR)
	{
		 while(message != "/q")
		{
			if (turn ==0){
				iResult =recv(ConnectSocket, message, sizeof(message), 0);

				if(message[0] =='/' && message[1] =='q'){
					shutdown(ConnectSocket, SD_BOTH);
					return 0;
				}
				if(strlen(message) >0)
					cout << serverName<< "> "<< message << endl;
				turn =1;
				
			}
			if(turn ==1){
				if(start >0)
					cout <<name <<">";
				cin.getline(message, sizeof(message));
				if(message[0] =='/' && message[1] =='q'){
					shutdown(ConnectSocket, SD_BOTH);
					return 0;
				}
				
				send(ConnectSocket, message, recvbuflen, 0);
				start++;
				turn =0;
			}
		
		}
	} 

    //S5// shutdown the connection since we're done
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // cleanup
	printf("\nChat Closed!\n");
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}