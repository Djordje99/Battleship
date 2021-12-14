#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "UIFunctions.h"

#define DEFAULT_BUFLEN 512
#define MESSAGE_SEND_BUFFER_SIZE 1024
#define MESSAGE_RECIEVE_BUFFER_SIZE 8
#define DEFAULT_PORT 27016

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

DWORD WINAPI SendMessageToServer(LPVOID lpParam);
DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam);

char messageToSend[MESSAGE_SEND_BUFFER_SIZE] = "";    
char receivedMessage[MESSAGE_RECIEVE_BUFFER_SIZE] = "";
SOCKET connectSocket = INVALID_SOCKET;
HANDLE errorSemaphore;

int main(int argc, char** argv)
{
    setWindowSize();

    int iResult;

    DWORD threadSenderID, threadReceiverID;
    HANDLE threadSender, threadReceiver;

    errorSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }

    startUpScreen();

    //welcome message from server
    iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);
    if (iResult > 0)
    {
        if (strcmp(receivedMessage, "true") == 0)
        {
            startingMenu();
            chooseGameType();
        }
        else {
            gameInProgress();
            pressEnterToContinue();
            closesocket(connectSocket);
            return 5;
        }
    }

    //thread sender
    threadSender = CreateThread(NULL, 0, &SendMessageToServer, NULL, 0, &threadSenderID);
    threadReceiver = CreateThread(NULL, 0, &ReceiveMessageFromServer, NULL, 0, &threadReceiverID);
    WaitForSingleObject(errorSemaphore, INFINITE);

    // cleanup
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

DWORD WINAPI SendMessageToServer(LPVOID lpParam) {
    while (true) {
        printf("Type message to server: ");
        scanf("%s", messageToSend);

        if (strcmp(messageToSend, "exit") == 0) {
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }

        int iResult = send(connectSocket, messageToSend, (int)strlen(messageToSend) + 1, 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }

        printf("Bytes Sent: %ld\n", iResult);

        Sleep(10);
    }

    return 0;
}

DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam) {
    while (true) {
        int iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            printf("\nMessage received from service: %s \n", receivedMessage);
        }
        else {
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }

        Sleep(10);
    }

    return 0;
}
