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
#include "../Common/Defines.h"
#include "../Common/Threads.cpp"


#define DEFAULT_BUFLEN 512
#define MESSAGE_SEND_BUFFER_SIZE 1024
#define MESSAGE_RECIEVE_BUFFER_SIZE 512
#define DEFAULT_PORT 27016

// define needed for window size
#define _WIN32_WINNT 0x0500

#pragma region functions

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

/*
    SendMessageToServer
    -------------------------------------------------------------------
    SendMessageToServer - salje poruku ka serveru
    -------------------------------------------------------------------
    Povratna vrednost: nema
*/
DWORD WINAPI SendMessageToServer(LPVOID lpParam);

/*
    ReceiveMessageFromServer
    -------------------------------------------------------------------
    ReceiveMessageFromServer - prima poruku od servera i vrsi ovradu poruke
    -------------------------------------------------------------------
    Povratna vrednost: nema
*/
DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam);

/*
    counterFunc
    -------------------------------------------------------------------
    counterFunc - vodi racuna o vremenu koje je preostalo za potez
    -------------------------------------------------------------------
    Povratna vrednost: nema
*/
DWORD WINAPI counterFunc(LPVOID lpParam);

#pragma endregion

char messageToSend[MESSAGE_SEND_BUFFER_SIZE] = "";    
char receivedMessage[MESSAGE_RECIEVE_BUFFER_SIZE] = "";
SOCKET connectSocket = INVALID_SOCKET;
HANDLE errorSemaphore, hCounterThread;
DWORD hCounterThreadID;
CRITICAL_SECTION csTimer;

char board[10][10];
char opponentBoard[10][10];
ActionPlayer player;
bool isMyTurn = false;
bool waitingScreenSet, stopCounter = false;
char userInput[256] = "";
int counter = 30;

bool reconnected = false;
bool reconnectedCounter = false;

int main(int argc, char** argv)
{
    // window resize block
    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

    setWindowSize();

    int iResult;

    DWORD threadSenderID, threadReceiverID, hCounterThreadID;
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
        if ((strcmp(receivedMessage, "true1") == 0) || (strcmp(receivedMessage, "true2") == 0))
        {
            startingMenu();
            int gameType = chooseGameType();
            message msg;
            if (receivedMessage[4] == '1')
                msg = FormatMessageStruct(CHOOSE_GAME, FIRST, gameType + '0', 0);
            else
                msg = FormatMessageStruct(CHOOSE_GAME, SECOND, gameType + '0', 0);

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

                if (msg.argument[0] == 1)
                    isMyTurn = true;
                else
                    isMyTurn = false;

                if (msg.type == READY) {
                    tableInitialization(board[0]);
                }  
                else if (msg.type == RECONNECT) {
                    printf("Reconnectig...\n");
                    reconnected = true;
                }
                else
                {
                    gameInProgress();
                    pressEnterToContinue();
                    closesocket(connectSocket);
                    return 5;
                }
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
    if (!reconnected) {
        waiting();
        message msg = FormatMessageStruct(PLACE_BOAT, player, board);
        int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);

        iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            receivedMessage[iResult] = '\0';
            message msg = *(message*)receivedMessage;

            if (msg.type != PLAY) {
                system("cls");
                printf("Something went wrong, press enter to exit\n");
                pressEnterToContinue();
                closesocket(connectSocket);
                return 5;
            }
        }
        else
        {
            system("cls");
            printf("Something went wrong, press enter to exit\n");
            pressEnterToContinue();
            closesocket(connectSocket);
            return 5;
        }
    }

    //thread sender
    gameStartUI(board[0]);
    threadSender = CreateThread(NULL, 0, &SendMessageToServer, NULL, 0, &threadSenderID);
    threadReceiver = CreateThread(NULL, 0, &ReceiveMessageFromServer, NULL, 0, &threadReceiverID);
    hCounterThread = CreateThread(NULL, 0, &counterFunc, &counter, 0, &hCounterThreadID);

    InitializeCriticalSection(&csTimer);

    WaitForSingleObject(errorSemaphore, INFINITE);

    // cleanup
    SAFE_DELETE_HANDLE(threadSender);
    SAFE_DELETE_HANDLE(threadReceiver);
    SAFE_DELETE_HANDLE(hCounterThread);
    DeleteCriticalSection(&csTimer);
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
    bool set = false;
    bool stop = false;
    while (!stopCounter) {
        if (isMyTurn) {

            memset(userInput, 0, 4);
            set = false;

            if(!reconnectedCounter)
                counter = 30;

            myTurn();
            stop = false;
            resumeThread(hCounterThread);
            while (true)
            {
                userInputFunction(userInput, &counter);
                if (counter == 0) {
                    stop = true;
                    Sleep(200);
                    break;
                }
                else {
                    if (strlen(userInput) == 2) {
                        if ((userInput[0] < 'A' || userInput[0] > 'J') || (userInput[1] < '1' || userInput[1] > '9')) {
                            memset(userInput, 0, 4);
                            resetInput();
                            continue;
                        }
                        if (opponentBoard[userInput[1] - 49][userInput[0] - 65] != 0) {
                            tryAgain(userInput);
                            memset(userInput, 0, 4);
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        if ((userInput[0] < 'A' || userInput[0] > 'J') || userInput[1] != '1' || userInput[2] != '0') {
                            memset(userInput, 0, 4);
                            resetInput();
                            continue;
                        }
                        if (opponentBoard[9][userInput[0] - 65] != 0) {
                            tryAgain(userInput);
                            memset(userInput, 0, 4);
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                }
            }
            
            if (strcmp(userInput, "exit") == 0) {
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }

            if (!stop)
            {
                message msg;
                if (strlen(userInput) == 2)
                {
                    msg = FormatMessageStruct(AIM_BOAT, player, atoi(&userInput[1]) - 1, userInput[0] - 65);
                }
                else
                {
                    msg = FormatMessageStruct(AIM_BOAT, player, 9, userInput[0] - 65);
                }

                int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);

                if (iResult == SOCKET_ERROR)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    ReleaseSemaphore(errorSemaphore, 1, NULL);
                    break;
                }
            }
            memset(userInput, 0, strlen(userInput));
            //printf("Bytes Sent: %ld\n", iResult);          
        }
       
        isMyTurn = false;

        if (!isMyTurn && !set)
        {
            pauseThread(hCounterThread);
            Sleep(100);
            opponentsTurn();
            set = true;
        }

        Sleep(10);
    }

    resumeThread(hCounterThread);
    return 0;
}

DWORD WINAPI ReceiveMessageFromServer(LPVOID lpParam) {
    while (true) {
        int iResult = recv(connectSocket, receivedMessage, DEFAULT_BUFLEN, 0);

        receivedMessage[iResult] = '\0';
        message* recvmsg = (message*)receivedMessage;

        if (iResult > 0)
        {
            //printf("\nMessage received from service\n");

            if (recvmsg->player != player && recvmsg->type == MISS)
            {
                changeTableField(1, recvmsg->argument[0], recvmsg->argument[1], board[0], 1);
                reconnectedCounter = false;
                isMyTurn = true;
            }
            else if (recvmsg->player == player && recvmsg->type == HIT)
            {
                changeTableField(2, recvmsg->argument[0], recvmsg->argument[1], opponentBoard[0], 2);
                reconnectedCounter = false;
                isMyTurn = true;
            }
            else if (recvmsg->player == player && recvmsg->type == MISS) {
                changeTableField(2, recvmsg->argument[0], recvmsg->argument[1], opponentBoard[0], 1);
                reconnectedCounter = false;
                isMyTurn = false;
            }
            else  if (recvmsg->player != player && recvmsg->type == HIT) {
                changeTableField(1, recvmsg->argument[0], recvmsg->argument[1], board[0], 2);
                reconnectedCounter = false;
                isMyTurn = false;
            }
            else if (recvmsg->player == player && recvmsg->type == VICTORY) {
                stopCounter = true;
                isMyTurn = false;
                Sleep(100);
                victory();
                pressEnterToContinue();
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }
            else if (recvmsg->player == player && recvmsg->type == DEFEAT) {
                stopCounter = true;
                isMyTurn = false;
                Sleep(100);
                defeat();
                pressEnterToContinue();
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }
            else if (recvmsg->player == player && recvmsg->type == PLACE_BOAT_CLIENT) {
                for (int i = 0; i < 10; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (recvmsg->matrixArgumetn[i][j] == 1) {
                            //promasaj
                            changeTableField(1, i, j, board[0], 1);
                            //board[i][j] = 1;
                        }
                        else if (recvmsg->matrixArgumetn[i][j] == 2) {
                            //pogodak
                            //board[i][j] = 2;
                            changeTableField(1, i, j, board[0], 2);
                        }
                        else if (recvmsg->matrixArgumetn[i][j] == 3) {
                            //mesto broda
                            //board[i][j] = 3;
                            changeTableField(1, i, j, board[0], 3);
                        }
                        else {
                            //board[i][j] = 0;
                            changeTableField(1, i, j, board[0], 0);
                        }
                    }
                }
            }
            else if (recvmsg->player == player && recvmsg->type == PLACE_BOAT_CLIENT_OPONENT) {
                for (int i = 0; i < 10; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (recvmsg->matrixArgumetn[i][j] == 1) {
                            //promasaj
                            changeTableField(2, i, j, opponentBoard[0], 1);
                            //board[i][j] = 1;
                        }
                        else if (recvmsg->matrixArgumetn[i][j] == 2) {
                            //pogodak
                            //board[i][j] = 2;
                            changeTableField(2, i, j, opponentBoard[0], 2);
                        }
                    }
                }
            }
            else if (recvmsg->player == player && recvmsg->type == TURN_PLAY) {
                counter = recvmsg->argument[0];
                reconnectedCounter = true;
                isMyTurn = true;
            }
            else if (recvmsg->player == player && recvmsg->type == TURN_WAIT) {
                isMyTurn = false;
            }
            else {
                isMyTurn = false;
            }

        }
        else {
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }

        Sleep(10);
    }

    return 0;
}

DWORD WINAPI counterFunc(LPVOID lpParam) {
    int* counter = (int*)lpParam;
    int pom = 0;
    while (!stopCounter) {
        while (pom != 100) {
            Sleep(10);
            if (stopCounter)
                return 0;
            pom++;
        }

        EnterCriticalSection(&csTimer);
        if (*counter == 0) {
            LeaveCriticalSection(&csTimer);
            continue;
        }
        *counter = (*counter)--;
        updateTimerUI(*counter);
        LeaveCriticalSection(&csTimer);
        pom = 0;
    }

    return 0;
}