#include "UIFunctions.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>


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

void showcursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = TRUE;
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

int chooseGameType() {
    hidecursor();
    COORD c;
    c.X = 59;
    State s = UP;

    char input;
    while (true)
    {
        if (_kbhit()) {

            // Stores the pressed key in ch
            input = _getch();
            if (input == 13) {
                break;
            }
            else if (input == 0 || input == -32) {
                input = _getch();

                if (input == 80 && s == UP) {
                    c.Y = 6;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
                    printf("  ");

                    c.Y = 7;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
                    printf("->");
                    s = DOWN;
                    continue;
                }
                if (input == 72 && s == DOWN) {
                    c.Y = 7;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
                    printf("  ");

                    c.Y = 6;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
                    printf("->");
                    s = UP;
                    continue;
                }

            }
        }
    }

    showcursor();
    system("cls");
    return s;
}

void gameInProgress() {
    hidecursor();
    COORD c;

    c.X = 59;
    c.Y = 6;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("GAME ALREADY IN PROGRESS");

    c.Y = 7;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    printf("       PLEASE WAIT        ");

}

void pressEnterToContinue() {
    char input;
    while (true)
    {
        if (_kbhit()) {

            input = _getch();
            if (input == 13) {
                break;
            }
        }
    }
    system("cls");
}