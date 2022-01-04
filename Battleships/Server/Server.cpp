#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"
#include "../Common/Message.h"
#include "../Common/MessageFormater.cpp"
#include "GameHelper.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define MAX_CLIENTS 2

bool InitializeWindowsSockets();
DWORD WINAPI Requests(LPVOID lpParam);
DWORD WINAPI ProducerForClients(LPVOID lpParam);
DWORD WINAPI SendToPlayer1(LPVOID lpParam);
DWORD WINAPI SendToPlayer2(LPVOID lpParam);
DWORD WINAPI RecvFromPlayer1(LPVOID lpParam);
DWORD WINAPI RecvFromPlayer2(LPVOID lpParam);
DWORD WINAPI counterFunc(LPVOID lpParam);
 
SOCKET listenSocket = INVALID_SOCKET;

SOCKET acceptedSocket[MAX_CLIENTS];
SOCKET acceptedSocketRefuse;

int curentClientCount = 0;
int iResult;

element* rootQueueRecv;
element* rootQueuePlayer1;
element* rootQueuePlayer2;
char recvbuf[DEFAULT_BUFLEN];

HANDLE errorSemaphore, threadSendPlayer1, threadSendPlayer2;
DWORD threadSendPlayer1ID, threadSendPlayer2ID;
HANDLE threadRecvPlayer1, threadRecvPlayer2;
DWORD threadRecvPlayer1ID, threadRecvPlayer2ID;
HANDLE hCounterThread;
DWORD hCounterThreadID;

CRITICAL_SECTION csTimer;
int counter = 31;

bool gameInProgress = false;
bool gameInitializationInProgress = false;

int connCountPlayer1 = 0; //broj puta koji su se konektovali kljenti
int connCountPlayer2 = 0;

char boardPlayer1[10][10];
char boardPlayer2[10][10];

int main(void) {
    DWORD threadRequestID, threadProducerID;
    HANDLE threadRequest, threadProducer;

    errorSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

    InitQueue(&rootQueueRecv);
    InitQueue(&rootQueuePlayer1);
    InitQueue(&rootQueuePlayer2);

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
    threadProducer = CreateThread(NULL, 0, &ProducerForClients, NULL, 0, &threadProducerID);
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

    int socketSelected = 0;

    while (true) {
        if (curentClientCount < MAX_CLIENTS + 1)
            FD_SET(listenSocket, &readfds);

        //for (int i = 0; i < curentClientCount; i++)
        //{
        //    FD_SET(acceptedSocket[i], &readfds);
        //}

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

                    iResult = send(acceptedSocketRefuse, "false", (int)strlen("false") + 1, 0);

                    if (iResult == SOCKET_ERROR)
                    {
                        printf("send failed with error: %d\n", WSAGetLastError());
                        closesocket(acceptedSocketRefuse);
                        WSACleanup();
                        return 1;
                    }

                    closesocket(acceptedSocketRefuse);

                    printf("3. client is refused.");

                    continue;
                }

                if (acceptedSocket[0] == INVALID_SOCKET) {
                    acceptedSocket[0] = accept(listenSocket, NULL, NULL);
                    connCountPlayer1++;
                    socketSelected = 0;
                }
                else if (acceptedSocket[1] == INVALID_SOCKET) {
                    acceptedSocket[1] = accept(listenSocket, NULL, NULL);
                    connCountPlayer2++;
                    socketSelected = 1;
                }

                if (acceptedSocket[socketSelected] == INVALID_SOCKET)
                {
                    printf("accept failed with error: %d\n", WSAGetLastError());
                    closesocket(listenSocket);
                    WSACleanup();
                    ReleaseSemaphore(errorSemaphore, 1, NULL);
                    return 1;
                }

                unsigned long mode = 1;
                iResult = ioctlsocket(acceptedSocket[socketSelected], FIONBIO, &mode);

                //welcome message
                iResult = send(acceptedSocket[socketSelected], "true", (int)strlen("true") + 1, 0);

                if (iResult == SOCKET_ERROR)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(acceptedSocket[curentClientCount]);
                    WSACleanup();
                    return 1;
                }

                curentClientCount++;

                if (socketSelected == 0)
                {
                    threadSendPlayer1 = CreateThread(NULL, 0, &SendToPlayer1, NULL, 0, &threadSendPlayer1ID);
                    threadRecvPlayer1 = CreateThread(NULL, 0, &RecvFromPlayer1, NULL, 0, &threadRecvPlayer1ID);
                    hCounterThread = CreateThread(NULL, 0, &counterFunc, &counter, 0, &hCounterThreadID);
                    SuspendThread(hCounterThread);
                    InitializeCriticalSection(&csTimer);
                }
                if (socketSelected == 1)
                {
                    threadSendPlayer2 = CreateThread(NULL, 0, &SendToPlayer2, NULL, 0, &threadSendPlayer2ID);
                    threadRecvPlayer2 = CreateThread(NULL, 0, &RecvFromPlayer2, NULL, 0, &threadRecvPlayer2ID);
                }
            }
        }

        FD_ZERO(&readfds);

        Sleep(10);
    }

    DeleteCriticalSection(&csTimer);

    return 0;
}

DWORD WINAPI ProducerForClients(LPVOID lpParam) {
    ActionPlayer hisTurn = FIRST;
    int usedFileds = 0;
    while (true) {
        if (QueueCount(rootQueueRecv) != 0) {
            message* msg = Dequeue(&rootQueueRecv);
            int arg1;
            int arg2;

            //PLACE_BOAT init switch
            switch (msg->type)
            {
                case PLACE_BOAT:
                {
                    hisTurn = FIRST;

                    for (int i = 0; i < 10; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            if (msg->matrixArgumetn[i][j] == 3) {
                                //mesto broda
                                if (msg->player == FIRST)
                                    boardPlayer1[i][j] = 3;
                                if (msg->player == SECOND)
                                    boardPlayer2[i][j] = 3;

                                usedFileds++;
                            }
                            else {
                                if (msg->player == FIRST)
                                    boardPlayer1[i][j] = 0;
                                if (msg->player == SECOND)
                                    boardPlayer2[i][j] = 0;
                            }
                        }
                    }

                    if (!gameInitializationInProgress && gameInProgress)
                    {
                        //bot game
                    }
                    if (gameInitializationInProgress)
                    {
                        if (usedFileds == 34) {
                            message msg1 = FormatMessageStruct(PLAY, FIRST, 0, 0);
                            message msg2 = FormatMessageStruct(PLAY, SECOND, 0, 0);
                            Enqueue(&msg1, &rootQueuePlayer1);
                            Enqueue(&msg2, &rootQueuePlayer2);
                            usedFileds = 0;
                            ResumeThread(hCounterThread);
                        }
                    }
                    break;
                }

                case AIM_BOAT:
                {
                    EnterCriticalSection(&csTimer);
                    arg1 = msg->argument[0];
                    arg2 = msg->argument[1];
                    bool hit = false;

                    if (msg->player == FIRST) {
                        hit = boardPlayer2[arg1][arg2] == 3 ? true : false;

                        if (hit) {
                            boardPlayer2[arg1][arg2] = 2;
                            message msgHit = FormatMessageStruct(HIT, FIRST, arg1, arg2);
                            if (GameOver(boardPlayer2)) {
                                message msgV = FormatMessageStruct(VICTORY, FIRST, arg1, arg2);
                                message msgD = FormatMessageStruct(DEFEAT, SECOND, arg1, arg2);
                                Enqueue(&msgV, &rootQueuePlayer1);
                                Enqueue(&msgD, &rootQueuePlayer2);

                                gameInitializationInProgress = false;
                                gameInProgress = false;
                                connCountPlayer1 = 0;
                                connCountPlayer2 = 0;

                                CloseHandle(hCounterThread);
                                counter = 31;
                            }
                            else {
                                hisTurn = FIRST;
                                Enqueue(&msgHit, &rootQueuePlayer1);
                                Enqueue(&msgHit, &rootQueuePlayer2);
                            }
                        }
                        else {
                            boardPlayer2[arg1][arg2] = 1;
                            hisTurn = SECOND;
                            message msgMiss = FormatMessageStruct(MISS, FIRST, arg1, arg2);
                            Enqueue(&msgMiss, &rootQueuePlayer1);
                            Enqueue(&msgMiss, &rootQueuePlayer2);
                        }
                    }

                    if (msg->player == SECOND) {
                        hit = boardPlayer1[arg1][arg2] == 3 ? true : false;

                        if (hit) {
                            boardPlayer1[arg1][arg2] = 2;
                            message msgHit = FormatMessageStruct(HIT, SECOND, arg1, arg2);
                            if (GameOver(boardPlayer1)) {
                                message msgV = FormatMessageStruct(VICTORY, SECOND, arg1, arg2);
                                message msgD = FormatMessageStruct(DEFEAT, FIRST, arg1, arg2);
                                Enqueue(&msgD, &rootQueuePlayer1);
                                Enqueue(&msgV, &rootQueuePlayer2);

                                gameInitializationInProgress = false;
                                gameInProgress = false;
                                connCountPlayer1 = 0;
                                connCountPlayer2 = 0;

                                CloseHandle(hCounterThread);
                                counter = 31;
                            }
                            else {
                                hisTurn = SECOND;
                                Enqueue(&msgHit, &rootQueuePlayer1);
                                Enqueue(&msgHit, &rootQueuePlayer2);
                            }
                        }
                        else {
                            boardPlayer1[arg1][arg2] = 1;
                            hisTurn = FIRST;
                            message msgMiss = FormatMessageStruct(MISS, SECOND, arg1, arg2);
                            Enqueue(&msgMiss, &rootQueuePlayer1);
                            Enqueue(&msgMiss, &rootQueuePlayer2);
                        }
                    }

                    counter = 31;
                    LeaveCriticalSection(&csTimer);

                    break;
                }               
                case READY:
                {
                    if (msg->player == FIRST)
                        Enqueue(msg, &rootQueuePlayer1);
                    if (msg->player == SECOND)
                        Enqueue(msg, &rootQueuePlayer2);
                    break;
                }
                case BUSY:
                {
                    if (msg->player == FIRST)
                        Enqueue(msg, &rootQueuePlayer1);
                    if (msg->player == SECOND)
                        Enqueue(msg, &rootQueuePlayer2);
                    break;
                }
                case PLACE_BOAT_CLIENT:
                {
                    if (msg->player == FIRST)
                        Enqueue(msg, &rootQueuePlayer1);
                    if (msg->player == SECOND)
                        Enqueue(msg, &rootQueuePlayer2);
                    break;
                }
                case PLACE_BOAT_CLIENT_OPONENT:
                {
                    if (msg->player == FIRST) {
                        Enqueue(msg, &rootQueuePlayer1);
                        if (hisTurn == FIRST) {
                            message msgPlay = FormatMessageStruct(TURN_PLAY, FIRST, counter, 0);
                            Enqueue(&msgPlay, &rootQueuePlayer1);
                        }
                        else {
                            message msgWait = FormatMessageStruct(TURN_WAIT, FIRST, 0, 0);
                            Enqueue(&msgWait, &rootQueuePlayer1);
                        }
                    }
                    if (msg->player == SECOND) {
                        Enqueue(msg, &rootQueuePlayer2);
                        if (hisTurn == SECOND) {
                            message msgPlay = FormatMessageStruct(TURN_PLAY, SECOND, counter, 0);
                            Enqueue(&msgPlay, &rootQueuePlayer2);
                        }
                        else {
                            message msgWait = FormatMessageStruct(TURN_WAIT, SECOND, 0, 0);
                            Enqueue(&msgWait, &rootQueuePlayer2);
                        }
                    }
                    break;
                }
                case RECONNECT:
                {
                    if (msg->player == FIRST)
                        Enqueue(msg, &rootQueuePlayer1);
                    if (msg->player == SECOND)
                        Enqueue(msg, &rootQueuePlayer2);
                    break;
                }
                case DEFEAT:
                {
                    if (hisTurn == FIRST)
                    {
                        message msgV = FormatMessageStruct(VICTORY, SECOND, 0, 0);
                        message msgD = FormatMessageStruct(DEFEAT, FIRST, 0, 0);
                        Enqueue(&msgD, &rootQueuePlayer1);
                        Enqueue(&msgV, &rootQueuePlayer2);

                        gameInitializationInProgress = false;
                        gameInProgress = false;
                        connCountPlayer1 = 0;
                        connCountPlayer2 = 0;

                        CloseHandle(hCounterThread);
                        counter = 31;
                    }
                    else
                    {
                        message msgV = FormatMessageStruct(VICTORY, FIRST, 0, 0);
                        message msgD = FormatMessageStruct(DEFEAT, SECOND, 0, 0);
                        Enqueue(&msgV, &rootQueuePlayer1);
                        Enqueue(&msgD, &rootQueuePlayer2);

                        gameInitializationInProgress = false;
                        gameInProgress = false;
                        connCountPlayer1 = 0;
                        connCountPlayer2 = 0;

                        CloseHandle(hCounterThread);
                        counter = 31;
                    }
                    break;
                }
            }
        }
                

        Sleep(10);
    }
}

#pragma region SendToPlayer

DWORD WINAPI SendToPlayer1(LPVOID lpParam) {
    while (true && acceptedSocket[0] != INVALID_SOCKET) {
        if (QueueCount(rootQueuePlayer1) != 0) {
            message sendP1 = *Dequeue(&rootQueuePlayer1);

            iResult = send(acceptedSocket[0], (char*)&sendP1, sizeof(sendP1), 0);

            if (iResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSocket[0]);
                WSACleanup();
                return 1;
            }
        }
        Sleep(40);
    }

    return 0;
}

DWORD WINAPI SendToPlayer2(LPVOID lpParam) {
    while (true && acceptedSocket[1] != INVALID_SOCKET) {
        if (QueueCount(rootQueuePlayer2) != 0) {
            message sendP2 = *Dequeue(&rootQueuePlayer2);

            iResult = send(acceptedSocket[1], (char*)&sendP2, sizeof(sendP2), 0);

            if (iResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSocket[1]);
                WSACleanup();
                return 1;
            }
        }
        Sleep(40);
    }

    return 0;
}

#pragma endregion

#pragma region RecieveFromPlayer

DWORD WINAPI RecvFromPlayer1(LPVOID lpParam) {
    message* recvmsg;
    message* msgToSend;
    fd_set readfds;
    FD_ZERO(&readfds);

    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;
    while (true && acceptedSocket[0] != INVALID_SOCKET) {
        FD_SET(acceptedSocket[0], &readfds);

        int result = select(0, &readfds, NULL, NULL, &timeVal);

        if (result == 0) {
            // vreme za cekanje je isteklo
            //Sleep(10);
            continue;
        }
        else if (result == SOCKET_ERROR) {
            //desila se greska prilikom poziva funkcije
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }
        else
        {
            if (FD_ISSET(acceptedSocket[0], &readfds)) {
                iResult = recv(acceptedSocket[0], recvbuf, DEFAULT_BUFLEN, 0);
                if (iResult > 0)
                {
                    recvbuf[iResult] = '\0';
                    recvmsg = (message*)recvbuf;

                    if (recvmsg->type == CHOOSE_GAME)
                    {
                        if (gameInProgress == false)
                        {
                            if (strcmp(recvmsg->argument, "1") == 0 && gameInitializationInProgress == false)
                                //if (ntohl(recvmsg->argumet) == 1 && gameInitializationInProgress == false)
                            {
                                gameInProgress = true;
                                msgToSend = (message*)malloc(sizeof(message));
                                msgToSend->type = READY;
                                msgToSend->player = FIRST;

                                Enqueue(msgToSend, &rootQueueRecv);

                            }
                            else if (strcmp(recvmsg->argument, "2") == 0)
                                //if (ntohl(recvmsg->argumet) == 2)
                            {
                                msgToSend = (message*)malloc(sizeof(message));
                                if (gameInitializationInProgress == false)
                                    gameInitializationInProgress = true;
                                else
                                    gameInProgress = true;

                                msgToSend->player = FIRST;
                                msgToSend->type = READY;
                                msgToSend->argument[0] = 1;
                                Enqueue(msgToSend, &rootQueueRecv);

                            }
                            else {
                                msgToSend = (message*)malloc(sizeof(message));
                                msgToSend->type = BUSY;
                                msgToSend->player = FIRST;
                                Enqueue(msgToSend, &rootQueueRecv);
                            }
                        }
                        else if (connCountPlayer1 > 1 && gameInProgress) {
                            message msgConn = FormatMessageStruct(RECONNECT, FIRST, 0, 0);
                            Enqueue(&msgConn, &rootQueueRecv);

                            Sleep(20);

                            message msgMatrix = FormatMessageStruct(PLACE_BOAT_CLIENT, FIRST, boardPlayer1);
                            Enqueue(&msgMatrix, &rootQueueRecv);

                            Sleep(20);

                            message msgMatrixOponent = FormatMessageStruct(PLACE_BOAT_CLIENT_OPONENT, FIRST, boardPlayer2);
                            Enqueue(&msgMatrixOponent, &rootQueueRecv);

                            Sleep(20);
                        }
                        else
                        {
                            msgToSend = (message*)malloc(sizeof(message));
                            msgToSend->type = BUSY;
                            msgToSend->player = FIRST;
                            Enqueue(msgToSend, &rootQueueRecv);
                        }
                        printf("Message received from client and queued in queue\n");
                    }
                    else
                    {
                        //stavljanje u queue
                        Enqueue(recvmsg, &rootQueueRecv);
                        printf("Message received from client and queued in queue!\n");
                    }
                }
                else if (iResult == 0)
                {
                    // connection was closed gracefully
                    printf("Connection with client closed.\n");
                    closesocket(acceptedSocket[0]);
                    acceptedSocket[0] = INVALID_SOCKET;
                    curentClientCount--;
                }
                else
                {
                    // there was an error during recv
                    printf("recv failed with error: %d\n", WSAGetLastError());
                    closesocket(acceptedSocket[0]);
                    acceptedSocket[0] = INVALID_SOCKET;
                    curentClientCount--;
                    break;
                }
            }
            Sleep(10);

        }
        FD_ZERO(&readfds);
    }

    return 0;
}

DWORD WINAPI RecvFromPlayer2(LPVOID lpParam) {
    message* recvmsg;
    message* msgToSend;
    fd_set readfds;
    FD_ZERO(&readfds);

    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;
    while (true && acceptedSocket[1] != INVALID_SOCKET) {
        FD_SET(acceptedSocket[1], &readfds);

        int result = select(0, &readfds, NULL, NULL, &timeVal);

        if (result == 0) {
            // vreme za cekanje je isteklo
            Sleep(10);
            continue;
        }
        else if (result == SOCKET_ERROR) {
            //desila se greska prilikom poziva funkcije
            ReleaseSemaphore(errorSemaphore, 1, NULL);
            break;
        }
        else
        {
            if (FD_ISSET(acceptedSocket[1], &readfds)) {
                iResult = recv(acceptedSocket[1], recvbuf, DEFAULT_BUFLEN, 0);
                if (iResult > 0)
                {
                    recvbuf[iResult] = '\0';
                    recvmsg = (message*)recvbuf;

                    if (recvmsg->type == CHOOSE_GAME)
                    {
                        if (gameInProgress == false)
                        {
                            if (strcmp(recvmsg->argument, "1") == 0 && gameInitializationInProgress == false)
                                //if (ntohl(recvmsg->argument) == 1 && gameInitializationInProgress == false)
                            {
                                gameInProgress = true;
                                msgToSend = (message*)malloc(sizeof(message));
                                msgToSend->type = READY;
                                msgToSend->player = SECOND;

                                Enqueue(msgToSend, &rootQueueRecv);

                            }
                            else if (strcmp(recvmsg->argument, "2") == 0)
                                //if (ntohl(recvmsg->argument) == 2)
                            {
                                msgToSend = (message*)malloc(sizeof(message));
                                if (gameInitializationInProgress == false)
                                    gameInitializationInProgress = true;
                                else
                                    gameInProgress = true;
                                msgToSend->player = SECOND;
                                msgToSend->type = READY;
                                Enqueue(msgToSend, &rootQueueRecv);
                            }
                            else {
                                msgToSend = (message*)malloc(sizeof(message));
                                msgToSend->type = BUSY;
                                msgToSend->player = SECOND;
                                Enqueue(msgToSend, &rootQueueRecv);
                            }
                        }
                        else if (connCountPlayer2 > 1 && gameInProgress) {
                            message msgConn = FormatMessageStruct(RECONNECT, SECOND, 0, 0);
                            Enqueue(&msgConn, &rootQueueRecv);

                            Sleep(20);

                            message msgMatrix = FormatMessageStruct(PLACE_BOAT_CLIENT, SECOND, boardPlayer2);
                            Enqueue(&msgMatrix, &rootQueueRecv);

                            Sleep(20);

                            message msgMatrixOponent = FormatMessageStruct(PLACE_BOAT_CLIENT_OPONENT, SECOND, boardPlayer1);
                            Enqueue(&msgMatrixOponent, &rootQueueRecv);

                            Sleep(20);
                        }
                        else
                        {
                            msgToSend = (message*)malloc(sizeof(message));
                            msgToSend->type = BUSY;
                            msgToSend->player = SECOND;
                            Enqueue(msgToSend, &rootQueueRecv);
                        }
                        printf("Message received from client and queued in queue\n");
                    }
                    else
                    {
                        //stavljanje u queue
                        Enqueue(recvmsg, &rootQueueRecv);
                        printf("Message received from client and queued in queue\n");
                    }
                }
                else if (iResult == 0)
                {
                    // connection was closed gracefully
                    printf("Connection with client closed.\n");
                    closesocket(acceptedSocket[1]);
                    acceptedSocket[1] = INVALID_SOCKET;
                    curentClientCount--;
                }
                else
                {
                    // there was an error during recv
                    printf("recv failed with error: %d\n", WSAGetLastError());
                    closesocket(acceptedSocket[1]);
                    acceptedSocket[1] = INVALID_SOCKET;
                    curentClientCount--;
                    break;
                }
            }
            Sleep(10);

        }
        FD_ZERO(&readfds);
    }

    return 0;
}

#pragma endregion

DWORD WINAPI counterFunc(LPVOID lpParam) {
    int* counter = (int*)lpParam;
    int pom = 0;
    while (true) {
        while (pom != 100) {
            Sleep(10);
            pom++;
        }

        EnterCriticalSection(&csTimer);
        if (*counter == 0) {
            message* msgEnd = (message*)malloc(sizeof(message));
            msgEnd->type = DEFEAT;
            Enqueue(msgEnd, &rootQueueRecv);
            break;
        }
        *counter = (*counter)--;
        LeaveCriticalSection(&csTimer);
        pom = 0;
    }

    return 0;
}