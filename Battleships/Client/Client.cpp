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
DWORD WINAPI counterFunc(LPVOID lpParam);

char messageToSend[MESSAGE_SEND_BUFFER_SIZE] = "";    
char receivedMessage[MESSAGE_RECIEVE_BUFFER_SIZE] = "";
SOCKET connectSocket = INVALID_SOCKET;
HANDLE errorSemaphore, hCounterThread;
DWORD hCounterThreadID;
CRITICAL_SECTION csTimer;

char board[10][10];
char opponentBoard[10][10];
ActionPlayer player;
bool isMyTurn, waitingScreenSet;
char userInput[256] = "";
int counter = 30;

bool reconnected = false;

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

                if (msg.type == READY) {
                    // nakon sto se vrati iz ove funkcije board je popunjen jedinicama i nulama
                    // moze da se doradi funkcija da vraca nesto sto olaksava pravljenje poruka ka
                    // serveru ali je preporuka ne otvarati pandorinu kutiju
                    tableInitialization(board[0]);
                    _getch();
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

                    _getch();
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
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                opponentBoard[i][j] = 0;
                char feld = board[i][j];
                if (feld == 3) {
                    message msg = FormatMessageStruct(PLACE_BOAT, player, i, j);

                    int iResult = send(connectSocket, (char*)&msg, sizeof(msg), 0);

                    if (iResult == SOCKET_ERROR)
                    {
                        printf("send failed with error: %d\n", WSAGetLastError());
                        return -1;
                    }

                    //printf("Bytes Sent: %ld\n", iResult);

                    Sleep(20);
                }
            }
        }
    }

    //thread sender
    gameStartUI(board[0]);
    threadSender = CreateThread(NULL, 0, &SendMessageToServer, NULL, 0, &threadSenderID);
    threadReceiver = CreateThread(NULL, 0, &ReceiveMessageFromServer, NULL, 0, &threadReceiverID);
    hCounterThread = CreateThread(NULL, 0, &counterFunc, &counter, 0, &hCounterThreadID);
    //pauseCounterThread(hCounterThread);
    InitializeCriticalSection(&csTimer);
    
    
    /*
    while (true) {
        myTurn();
        resumeCounterThread(hCounterThread);

        userInputFunction(userInput);
        //_getch();
        pauseCounterThread(hCounterThread);
        changeTableField(FIRST, 0, 0, board[0], 1);

        opponentsTurn();
        _getch();

        counter = 30;
    }
    */

    WaitForSingleObject(errorSemaphore, INFINITE);

    // cleanup
    CloseHandle(threadSender);
    CloseHandle(threadReceiver);
    CloseHandle(hCounterThread);
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
    while (true) {
        if (isMyTurn) {
            /*
            printf("Type cordinate to attack enemy boat; format [0-9, 0-9]: ");
            scanf("%s", messageToSend);
            */
            memset(userInput, 0, strlen(userInput));
            set = false;
            counter = 30;
            myTurn();
            resumeCounterThread(hCounterThread);
            userInputFunction(userInput);

            if (strcmp(userInput, "exit") == 0) {
                ReleaseSemaphore(errorSemaphore, 1, NULL);
                break;
            }

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

            memset(userInput, 0, strlen(userInput));
            //printf("Bytes Sent: %ld\n", iResult);          
        }
       
        isMyTurn = false;

        if (!isMyTurn && !set)
        {
            pauseCounterThread(hCounterThread);
            Sleep(100);
            opponentsTurn();
            set = true;
        }

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
            //printf("\nMessage received from service\n");

            if (recvmsg->player != player && recvmsg->type == MISS)
            {
                changeTableField(1, recvmsg->argument[0], recvmsg->argument[1], board[0], 1);
                isMyTurn = true;
            }
            else if (recvmsg->player == player && recvmsg->type == HIT)
            {
                changeTableField(2, recvmsg->argument[0], recvmsg->argument[1], opponentBoard[0], 2);
                isMyTurn = true;
            }
            else if (recvmsg->player == player && recvmsg->type == MISS) {
                changeTableField(2, recvmsg->argument[0], recvmsg->argument[1], opponentBoard[0], 1);
                isMyTurn = false;
            }
            else  if (recvmsg->player != player && recvmsg->type == HIT) {
                changeTableField(1, recvmsg->argument[0], recvmsg->argument[1], board[0], 2);
                isMyTurn = false;
            }
            else if (recvmsg->player == player && recvmsg->type == VICTORY) {
                isMyTurn = false;
                Sleep(100);
                victory();
                pressEnterToContinue();
                ReleaseSemaphore(errorSemaphore, 1, NULL);
            }
            else if (recvmsg->player == player && recvmsg->type == DEFEAT) {
                isMyTurn = false;
                Sleep(100);
                defeat();
                pressEnterToContinue();
                ReleaseSemaphore(errorSemaphore, 1, NULL);
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
    while (true) {
        while (pom != 100) {
            Sleep(10);
            pom++;
        }

        EnterCriticalSection(&csTimer);
        *counter = (*counter)--;
        updateTimerUI(*counter);
        LeaveCriticalSection(&csTimer);
        pom = 0;
    }

    return 0;
}