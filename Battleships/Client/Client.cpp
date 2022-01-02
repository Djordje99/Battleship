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
#include "../Common/Message.h"
#include "../Common/MessageFormater.cpp"

#define DEFAULT_BUFLEN 512
#define MESSAGE_SEND_BUFFER_SIZE 1024
#define MESSAGE_RECIEVE_BUFFER_SIZE 512
#define DEFAULT_PORT 27016

// define needed for window size
#define _WIN32_WINNT 0x0500

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

DWORD WINAPI SendMessageToServer(LPVOID lpParam);
DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam);

char messageToSend[MESSAGE_SEND_BUFFER_SIZE] = "";    
char receivedMessage[MESSAGE_RECIEVE_BUFFER_SIZE] = "";
SOCKET connectSocket = INVALID_SOCKET;
HANDLE errorSemaphore;

char board[10][10];
ActionPlayer player;
bool isMyTurn;

int main(int argc, char** argv)
{
    // window resize block
    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

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
            int gameType = chooseGameType();
            //message msg = FormatMessageStruct(CHOOSE_GAME, NONE, (char*)gameType, NULL);
            message msg{
                msg.type = CHOOSE_GAME,
                msg.player = NONE,
                //msg.argumet = htonl(gameType),
                msg.argument[0] = 0,
                //msg.aditionalArgumet[0] = 0,
            };
            sprintf(msg.argument, "%d", gameType);
            int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                pressEnterToContinue();
                closesocket(connectSocket);
                return 5;
            }

            iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);
            if (iResult > 0)
            {
                receivedMessage[iResult] = '\0';
                msg = *(message*)receivedMessage;
                player = msg.player;

                if (player == FIRST)
                    isMyTurn = true;
                else
                    isMyTurn = false;

                if (msg.type == READY)
                    tableInitialization(board[0]);  // nakon sto se vrati iz ove funkcije board je popunjen jedinicama i nulama
                                                    // moze da se doradi funkcija da vraca nesto sto olaksava pravljenje poruka ka
                                                    // serveru ali je preporuka ne otvarati pandorinu kutiju
                else
                {
                    gameInProgress();
                    pressEnterToContinue();
                    closesocket(connectSocket);
                    return 5;
                }
                _getch();
            }
            else
            {
                gameInProgress();
                pressEnterToContinue();
                closesocket(connectSocket);
                return 5;
            }
        }
        else {
            gameInProgress();
            pressEnterToContinue();
            closesocket(connectSocket);
            return 5;
        }
    }

    //send to server matrix
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {   
            char feld = board[i][j];
            if(feld == '\x1') {
                message msg = FormatMessageStruct(PLACE_BOAT, player, i, j);

                int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);

                if (iResult == SOCKET_ERROR)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    return -1;
                }

                printf("Bytes Sent: %ld\n", iResult);

                Sleep(20);
            }
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
        if (isMyTurn) {
            printf("Type cordinate to attack enemy boat; format [0-9, 0-9]: ");
            scanf("%s", messageToSend);

            if (strcmp(messageToSend, "exit") == 0) {
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }

            message msg = FormatMessageStruct(AIM_BOAT, player, atoi(&messageToSend[0]), atoi(&messageToSend[2]));

            int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);

            if (iResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }

            printf("Bytes Sent: %ld\n", iResult);
        }
       
        isMyTurn = false;

        Sleep(10);
    }

    return 0;
}

DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam) {
    while (true) {
        int iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);

        receivedMessage[iResult] = '\0';
        message* recvmsg = (message*)receivedMessage;

        if (iResult > 0)
        {
            printf("\nMessage received from service\n");

            if (recvmsg->player != player && recvmsg->type == MISS)
                isMyTurn = true;
            else if(recvmsg->player == player && recvmsg->type == HIT)
                isMyTurn = true;
            else if (recvmsg->player == player && recvmsg->type == VICTORY) {
                isMyTurn = false;
                printf("VICTORY!");
            }
            else if (recvmsg->player == player && recvmsg->type == DEFEAT) {
                isMyTurn = false;
                printf("DEFEAT :(");
            }
            else
                isMyTurn = false;

        }
        else {
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }

        Sleep(10);
    }

    return 0;
}
