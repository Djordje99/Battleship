#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define MAX_CLIENTS 2

int curentClientCount = 0;

bool InitializeWindowsSockets();
DWORD WINAPI Requests(LPVOID lpParam);

// Socket used for listening for new clients 
SOCKET listenSocket = INVALID_SOCKET;
// Socket used for communication with client
SOCKET acceptedSocket[MAX_CLIENTS];
SOCKET acceptedSocketRefuse;
// variable used to store function return value
int iResult;
// Buffer used for storing incoming data
element* rootQueueRecv;
char recvbuf[DEFAULT_BUFLEN];

HANDLE errorSemaphore;

int main(void) {
    DWORD threadRequestID;
    HANDLE threadRequest;

    errorSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

    InitQueue(&rootQueueRecv);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        acceptedSocket[i] = INVALID_SOCKET;
    }

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address famly
        SOCK_STREAM,  // stream socket
        IPPROTO_TCP); // TCP

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address 
    // to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    unsigned long mode = 1;
    iResult = ioctlsocket(listenSocket, FIONBIO, &mode);

    printf("Server initialized, waiting for clients.\n");

    //thread for requests made in select function
    threadRequest = CreateThread(NULL, 0, &Requests, NULL, 0, &threadRequestID); 
    WaitForSingleObject(errorSemaphore, INFINITE);

    // shutdown the connection since we're done
    iResult = shutdown(acceptedSocket[0], SD_SEND);

    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket[0]);
        WSACleanup();
        return 1;
    }

    iResult = shutdown(acceptedSocket[1], SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket[1]);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(listenSocket);
    closesocket(acceptedSocket[0]);
    closesocket(acceptedSocket[1]);
    WSACleanup();

    return 0;
}

bool InitializeWindowsSockets() {
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

DWORD WINAPI Requests(LPVOID lpParam) {
    fd_set readfds;
    FD_ZERO(&readfds);

    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;
    while (true) {
        if (curentClientCount < MAX_CLIENTS)
            FD_SET(listenSocket, &readfds);

        for (int i = 0; i < curentClientCount; i++)
        {
            FD_SET(acceptedSocket[i], &readfds);
        }

        int result = select(0, &readfds, NULL, NULL, &timeVal);

        if (result == 0) {
            // vreme za cekanje je isteklo
            continue;
        }
        else if (result == SOCKET_ERROR) {
            //desila se greska prilikom poziva funkcije
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }
        else {
            // rezultat je jednak broju soketa koji su zadovoljili uslov
            if (FD_ISSET(listenSocket, &readfds)) {
                if (curentClientCount == MAX_CLIENTS) {

                    //there is 3. client wwnt to connect, accept connection and then close it
                    acceptedSocketRefuse = accept(listenSocket, NULL, NULL);

                    if (acceptedSocketRefuse == INVALID_SOCKET)
                    {
                        printf("accept failed with error: %d\n", WSAGetLastError());
                        closesocket(listenSocket);
                        WSACleanup();
                        ReleaseSemaphore(errorSemaphore, 1, NULL);
                        return 1;
                    }

                    closesocket(acceptedSocketRefuse);
                    shutdown(acceptedSocketRefuse, SD_SEND);

                    printf("3. client is refused.");

                    continue;
                }

                acceptedSocket[curentClientCount] = accept(listenSocket, NULL, NULL);

                if (acceptedSocket[curentClientCount] == INVALID_SOCKET)
                {
                    printf("accept failed with error: %d\n", WSAGetLastError());
                    closesocket(listenSocket);
                    WSACleanup();
                    ReleaseSemaphore(errorSemaphore, 1, NULL);
                    return 1;
                }

                unsigned long mode = 1;
                iResult = ioctlsocket(acceptedSocket[curentClientCount], FIONBIO, &mode);
                curentClientCount++;
            }

            for (int i = 0; i < curentClientCount; i++) {
                if (FD_ISSET(acceptedSocket[i], &readfds)) {
                    iResult = recv(acceptedSocket[i], recvbuf, DEFAULT_BUFLEN, 0);
                    if (iResult > 0)
                    {
                        //stavljanje u queue
                        Enqueue(recvbuf, &rootQueueRecv);
                        printf("Message received from client and qnqueued in queue\n");
                        printf(Dequeue(&rootQueueRecv));
                    }
                    else if (iResult == 0)
                    {
                        // connection was closed gracefully
                        printf("Connection with client closed.\n");
                        closesocket(acceptedSocket[i]);

                        if (i == 0 && acceptedSocket[1] != INVALID_SOCKET) {
                            acceptedSocket[0] = acceptedSocket[1];
                            acceptedSocket[1] = INVALID_SOCKET;
                        }
                        curentClientCount--;
                    }
                    else
                    {
                        // there was an error during recv
                        printf("recv failed with error: %d\n", WSAGetLastError());
                        closesocket(acceptedSocket[i]);
                    }
                }
            }
        }

        FD_ZERO(&readfds);
    }

    return 0;
}
