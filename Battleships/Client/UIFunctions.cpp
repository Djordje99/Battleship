#include "UIFunctions.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>


void setWindowSize() {
	SMALL_RECT windowSize = { 0, 0, 96, 15 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    COORD newSize;

    if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize))
    {
        printf("SetConsoleWindowInfo() failed with error %d\n", GetLastError());
        exit(-1);
    }

    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
    newSize.X = screenBufferInfo.srWindow.Right - screenBufferInfo.srWindow.Left + 1; // Columns
    newSize.Y = screenBufferInfo.srWindow.Bottom - screenBufferInfo.srWindow.Top + 1; // Rows

    // set the new screen buffer dimensions(removing scroll bars)
    if (!SetConsoleScreenBufferSize(hConsole, newSize))
    {
        printf("SetConsoleScreenBufferSize() failed with error %d\n",GetLastError());
        exit(-1);
    }
}

void hidecursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void startUpScreen() {
    system("cls");
    printf("                                                 |\n");
    printf("                                                 |\n");
    printf("                                                 |\n");
    printf("  _           _   _   _           _     _        |\n");
    printf(" | |         | | | | | |         | |   (_)       |\n");
    printf(" | |__   __ _| |_| |_| | ___  ___| |__  _ _ __   |\n");
    printf(" | '_ \\ / _` | __| __| |/ _ \\/ __| '_ \\| | '_ \\  |\n");
    printf(" | |_) | (_| | |_| |_| |  __/\\__ \\ | | | | |_) | |\n");
    printf(" |_.__/ \\__,_|\\__|\\__|_|\\___||___/_| |_|_| .__/  |\n");
    printf("                                         | |     |\n");
    printf("                                         |_|     |\n");
    printf("                                                 |\n");
    printf("                                                 |\n");
    printf("                                                 |\n");
    printf("                                                 |\n");
}

void startingMenu() {
    COORD c;

    c.X = 59;
    c.Y = 6;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("-> NEW GAME AGAINST BOT ");

    c.X = 62;
    c.Y = 7;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("NEW GAME AGAINST PLAYER");
}

void gameInProgress() {
    COORD c;

    c.X = 59;
    c.Y = 6;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("GAME ALREADY IN PROGRESS");

    c.Y = 7;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("       PLEASE WAIT        ");
}