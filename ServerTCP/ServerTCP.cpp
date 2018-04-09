// ServerTCP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#define _MULTI_THREADED
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>


#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

int main()
{
	WSADATA wsaData;
	int wsaerr;
	SOCKET sockId, socketCliId;
	int on = 1;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	char buf[BUF_SIZE];
	wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaerr != 0) {
		printf("Could not initialize Winsock.\n");
		return -1;
	}
	printf("Winsock Initialized!\n");
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);

	sockId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockId < 0) {
		printf("Error creating socket\n");
		WSACleanup();
		return -1;
	}	std::cout << "Socket Created Id: " << sockId<<std::endl;		setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (bind(sockId, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		printf("Error binding port\n");
		WSACleanup();
		return -1;
	}

	std::cout << "Socket "<<sockId <<" Binded to Server Port: " << SERVER_PORT << std::endl;

	if (listen(sockId, QUEUE_SIZE) < 0) {
		printf("listen failed\n");
		WSACleanup();
		return -1;
	}

	std::cout << "Listening on Socket Id: " << sockId << std::endl;
	std::cout << "Waiting for Client... \n" << std::endl;

	socklen_t sock_len = sizeof(client);
	memset(&client, 0, sizeof(client));
	socketCliId = accept(sockId, (struct sockaddr *)&client, &sock_len);
	std::cout << "Client Accepted!\n" << std::endl;
	
	while (1) {		
		if (socketCliId == INVALID_SOCKET)
			printf("Can't accept client:%d\n", WSAGetLastError());
		else {
			
			int rec = 0;
			int totalRecv = 0;
			do {
				std::cout << "Waiting...\n" << std::endl;
				rec = recv(socketCliId, buf + totalRecv, BUF_SIZE, 0);	
				if (rec <= 0) {
					break;
				}
				totalRecv += rec;
				//printf("rec:%d | totalRecv: %d\n", rec, totalRecv);
			} while (buf[totalRecv - 1] != '\0');
			char ipClient[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &client.sin_addr, ipClient, sizeof(ipClient));

			if (rec <= 0) {
				printf("Client Disconnected!\n");
				getchar();
				break;
			}
			else {
				printf("Client:%s Port:%d\n", ipClient, ntohs(client.sin_port));
				printf("Message Length: %d\n", totalRecv);
				printf("Message: %s\n", buf);
			}
			
		}
		
	}
	closesocket(socketCliId);
	closesocket(sockId);
	WSACleanup();
	
	return 0;
}

