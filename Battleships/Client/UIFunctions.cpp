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
	startUpScreen();
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

void printElement(char printEl) {
    switch (printEl)
    {
    case 0:
        printf("~");
        break;
    case 1:
        printf("\033[1;31m");
        printf("X");
        printf("\033[0m");
        break;
    case 2:
        printf("\033[0;32m");
        printf("O");
        printf("\033[0m");
        break;
	case 3:
		printf("#");
    default:
        break;
    }
}

bool checkFirstCoordinate(char* coordinate, int length) {
    if ((*coordinate <= 64 || *coordinate >= 75) && (*coordinate > 96 || *coordinate < 107))
        return false;
    if (length == 2)//A1
    {
        if (*(coordinate + 1) <= 47 || *(coordinate + 1) >= 58)
            return false;
    }
    else			//A10
    {
        if (*(coordinate + 1) != '1')
            return false;
        if (*(coordinate + 2) != '0')
            return false;
    }
    return true;
}

int checkSecondCoordinate(char* firstCoordinate, int lengthFirst, int boatLength, char* coordinate, int length) {
    if ((*coordinate <= 64 || *coordinate >= 75) && (*coordinate > 96 || *coordinate < 107))
        return -1;
    if (length == 2)//A1
    {
        if (*(coordinate + 1) <= 47 || *(coordinate + 1) >= 58)
            return -1;
    }
    else			//A10
    {
        if (*(coordinate + 1) != '1')
            return -1;
        if (*(coordinate + 2) != '0')
            return -1;
    }

    if (*coordinate != *firstCoordinate)
    {
        if ((*coordinate - *firstCoordinate != boatLength - 1) && (*firstCoordinate - *coordinate != boatLength - 1))
            return -2;
        if (lengthFirst != length)
            return -2;
        if (length == 2)
        {
            if (*(coordinate + 1) != *(firstCoordinate + 1))
                return -2;
        }
    }
    else
    {
        if (lengthFirst == 2 && length == 2)
        {
            if (abs(*(coordinate + 1) - *(firstCoordinate + 1)) != boatLength - 1)
                return -2;
        }
        else if (lengthFirst == 3)
        {
            if ((58 - *(coordinate + 1) != boatLength - 1) && (58 - *(coordinate + 1) != -(boatLength - 1)))
                return -2;
        }
        else if (length == 3)
        {
            if ((58 - *(firstCoordinate + 1) != boatLength - 1) && (58 - *(firstCoordinate + 1) != -(boatLength - 1)))
                return -2;
        }
    }

    return 0;
}

bool checkIfFieldIsAvailable(char* table, char* coordinate, int length) {
    int i, j;
    if (*coordinate >= 65 && *coordinate <= 74)
    {
        j = *coordinate - 65;
        if (length == 2)
        {
            i = atoi(coordinate + 1) - 1;
        }
        else
        {
            i = 9;
        }
    }
    else
    {
        if (length == 2)
        {
            i = atoi(coordinate) - 1;
            j = *(coordinate + 1) - 65;
        }
        else
        {
            i = 9;
            j = *(coordinate + 2) - 65;
        }
    }

    if (*(table + i * 10 + j) == 0)
        return true;
    else
        return false;
}

void randomTableInitialization(char* table) {
	COORD c;
	HCRYPTPROV   hCryptProv;
	BYTE         pbData[2];

	char helpTable[10][10];
	int i, j;
	for (int i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			helpTable[i][j] = 0;
		}
	}

	int caseTry = 0;
	int boats = 0;
	int length[5] = { 5,4,3,3,2 };
	char inputFirstCoord[4] = "";
	char inputSecondCoord[4] = "";
	int counter = 1;
	bool generateFirstAgain = false;

	CryptAcquireContext(
		&hCryptProv,
		NULL,
		(LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT);



	while (true)
	{
		//generisati 2 puta od 0 do 9 
		if (CryptGenRandom(hCryptProv, 2, pbData))
		{
			inputFirstCoord[0] = pbData[0] % 10 + 65;
			inputFirstCoord[1] = pbData[1] % 10 + 48;
			if (inputFirstCoord[1] == 48) {
				inputFirstCoord[1] = 49;
			}
		}
		if (!checkIfFieldIsAvailable(helpTable[0], inputFirstCoord, strlen(inputFirstCoord)))
		{
			continue;
		}

		caseTry = 0;
		while (true)
		{
			switch (caseTry)
			{
			case 0:
				caseTry++;
				if (inputFirstCoord[1] - length[boats] + 1 > 48) {
					inputSecondCoord[1] = inputFirstCoord[1] - length[boats] + 1;
					inputSecondCoord[0] = inputFirstCoord[0];
				}
				else {
					continue;
				}
				break;
			case 1:
				caseTry++;
				if (inputFirstCoord[1] + length[boats] - 1 < 58) {
					inputSecondCoord[1] = inputFirstCoord[1] + length[boats] - 1;
					inputSecondCoord[0] = inputFirstCoord[0];
				}
				else {
					continue;
				}
				break;
			case 2:
				caseTry++;
				if (inputFirstCoord[0] - length[boats] + 1 > 64) {
					inputSecondCoord[0] = inputFirstCoord[0] - length[boats] + 1;
					inputSecondCoord[1] = inputFirstCoord[1];
				}
				else {
					continue;
				}
				break;
			case 3:
				caseTry++;
				if (inputFirstCoord[0] + length[boats] - 1 < 75) {
					inputSecondCoord[0] = inputFirstCoord[0] + length[boats] - 1;
					inputSecondCoord[1] = inputFirstCoord[1];
				}
				else {
					continue;
				}
			}

			if (caseTry == 4) {
				break;
				generateFirstAgain = true;
			}

			if (!checkIfFieldIsAvailable(helpTable[0], inputSecondCoord, strlen(inputSecondCoord)))
			{
				continue;
			}

			break;
		}

		if (generateFirstAgain)
			continue;

		boats++;

		char number[3];
		strcpy_s(number, inputFirstCoord + 1);
		int firstCoordNumber = atoi(number);
		strcpy_s(number, inputSecondCoord + 1);
		int secondCoordNumber = atoi(number);


		if (inputFirstCoord[0] == inputSecondCoord[0])
		{
			c.X = 5 + (inputFirstCoord[0] % 65) * 4;
			c.Y = 5;
			if (secondCoordNumber > firstCoordNumber)
			{
				c.Y = 5 + secondCoordNumber - 1;
				for (i = secondCoordNumber; i >= firstCoordNumber; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][inputFirstCoord[0] % 65] = 1;
					if (i == secondCoordNumber) //prvi prolaz
					{
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else if (i == firstCoordNumber) //poslednji prolaz
					{
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else //srednji prolaz
					{
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					c.Y--;
				}
			}
			else
			{
				c.Y = 5 + secondCoordNumber - 1;
				for (i = firstCoordNumber; i >= secondCoordNumber; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][inputFirstCoord[0] % 65] = 1;
					if (i == secondCoordNumber) //prvi prolaz
					{
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else if (i == firstCoordNumber) //poslednji prolaz
					{
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else //srednji prolaz
					{
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					c.Y++;
				}
			}
		}
		else
		{
			c.Y = 5 + firstCoordNumber - 1;
			if (inputSecondCoord[0] > inputFirstCoord[0])
			{
				c.X = 5 + (inputSecondCoord[0] % 65) * 4;
				for (i = inputSecondCoord[0]; i >= inputFirstCoord[0]; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][i % 65] = 1;
					if (i == inputSecondCoord[0]) //prvi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][i % 65 + 1] = 3;
						}
						if (i % 65 + 1 <= 9)
						{
							helpTable[c.Y - 5][i % 65 + 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][i % 65 + 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}

					}
					else if (i == inputFirstCoord[0]) //drugi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][i % 65 - 1] = 3;
						}
						if (i % 65 - 1 >= 0)
						{
							helpTable[c.Y - 5][i % 65 - 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][i % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					else
					{
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					c.X -= 4;
				}
			}
			else
			{
				c.X = 5 + (inputFirstCoord[0] % 65) * 4;
				for (i = inputFirstCoord[0]; i >= inputSecondCoord[0]; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][i % 65] = 1;
					if (i == inputFirstCoord[0]) //prvi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][i % 65 + 1] = 3;
						}
						if (i % 65 + 1 <= 9)
						{
							helpTable[c.Y - 5][i % 65 + 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][i % 65 + 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}

					}
					else if (i == inputSecondCoord[0]) //drugi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][i % 65 - 1] = 3;
						}
						if (i % 65 - 1 >= 0)
						{
							helpTable[c.Y - 5][i % 65 - 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][i % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					else
					{
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					c.X -= 4;
				}
			}
		}

		c.X = 51;
		c.Y = 7 + counter;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		if (firstCoordNumber < secondCoordNumber)
			printf("Position of boat #%d : %s - %s                ", counter, inputFirstCoord, inputSecondCoord);
		else
			printf("Position of boat #%d : %s - %s                ", counter, inputSecondCoord, inputFirstCoord);
		counter++;

		if (counter == 6)
			break;
	}

	for (int i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			*(table + i * 10 + j) = helpTable[i][j] == 3 ? 0 : helpTable[i][j] == 1 ? 3 : 0;
		}
	}

	hidecursor();
}

void tableInitialization(char* table) {

	char helpTable[10][10];
	int i, j;
	for (int i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			helpTable[i][j] = 0;
		}
	}
	system("cls");
	showcursor();
	printf("                                     PLACE YOUR BOATS ON TABLE\n");
	printf("\n");
	printf("                   YOUR TABLE                    | RULES: To place a boat enter its start and\n");   //y = 2
	printf("     A   B   C   D   E   F   G   H   I   J       |        end coordinates(ex. A5). Coordinates\n"); //y = 3
	printf("   _________________________________________     |        must be in a straight line(vertical or\n");		//y = 4
	printf(" 1 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |        horizontal). Boats must be devided \n"); //y = 5
	printf(" 2 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |        by one field all around.\n");			//y = 6
	printf(" 3 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     | Enter 0 on this entry to get random placement\n"); //y = 7 ERROR line
	printf(" 4 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 8    										
	printf(" 5 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 9   
	printf(" 6 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 10   
	printf(" 7 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 11  
	printf(" 8 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 12  
	printf(" 9 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 13  
	printf("10 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |     |\n"); //y = 14  

	char inputFirstCoord[4] = "";
	char inputSecondCoord[4] = "";
	char input;
	int counter = 1;
	bool firstInput = true;

	_COORD c;
	c.X = 52;
	c.Y = 7;
	while (true)
	{
		c.X = 51;
		c.Y = 7 + counter;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		printf("Enter start coordinate for ship #%d(len-%d):    ", counter, (counter == 4 ? 3 : 6 - counter) == 1 ? 2 : counter == 4 ? 3 : 6 - counter);
		c.X = 93;
		inputFirstCoord[0] = 0;
		inputFirstCoord[1] = 0;
		inputFirstCoord[2] = 0;
		inputFirstCoord[3] = 0;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

		while (true)
		{
			if (_kbhit())
			{
				input = _getch();
				if (((input > 64 && input < 75) || (input > 47 && input < 58) || (input > 96 && input < 107)) && strlen(inputFirstCoord) < 3)
				{

					inputFirstCoord[strlen(inputFirstCoord)] = input;

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("%c", input);
					c.X++;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				}
				if (input == 8 && (strlen(inputFirstCoord) > 0))
				{
					inputFirstCoord[strlen(inputFirstCoord) - 1] = 0;
					c.X--;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf(" ");

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				}
				if (input == '\r')
					break;

			}

		}

		if (inputFirstCoord[0] == '0' && firstInput) {
			randomTableInitialization(table);
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("Boats randomly generated and placed            ");
			return;
		}
		if (firstInput) {
			firstInput = false;
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("                                              ");
		}
		

		if (strlen(inputFirstCoord) < 2 || strlen(inputFirstCoord) > 3)
		{
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("ERROR: Wrong number of characters  ");
			continue;
		}
		if (!checkFirstCoordinate(inputFirstCoord, strlen(inputFirstCoord)))
		{
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("ERROR: Wrong format of coordinates ");
			continue;
		}
		if (!checkIfFieldIsAvailable(helpTable[0], inputFirstCoord, strlen(inputFirstCoord)))
		{
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("ERROR: Too close to the placed boat");
			continue;
		}

		c.X = 51;
		c.Y = 7;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		printf("                                      ");
		while (true)
		{
			inputSecondCoord[0] = 0;
			c.X = 51;
			c.Y = 7 + counter + 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("Enter end coordinate for ship #%d(len-%d):      ", counter, (counter == 4 ? 3 : 6 - counter) == 1 ? 2 : counter == 4 ? 3 : 6 - counter);
			c.X = 92;
			inputSecondCoord[0] = 0;
			inputSecondCoord[1] = 0;
			inputSecondCoord[2] = 0;
			inputSecondCoord[3] = 0;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

			while (true)
			{
				if (_kbhit())
				{
					input = _getch();
					if (((input > 64 && input < 75) || (input > 47 && input < 58) || (input > 96 && input < 107)) && strlen(inputSecondCoord) < 3)
					{

						inputSecondCoord[strlen(inputSecondCoord)] = input;

						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
						printf("%c", input);
						c.X++;
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					}
					if (input == 8 && (strlen(inputSecondCoord) > 0))
					{
						inputSecondCoord[strlen(inputSecondCoord) - 1] = 0;
						c.X--;
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
						printf(" ");

						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					}
					if (input == '\r')
						break;

				}

			}

			if (strlen(inputSecondCoord) < 2 || strlen(inputSecondCoord) > 3)
			{
				c.X = 51;
				c.Y = 7;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				printf("ERROR: Wrong number of characters            ");
				continue;
			}
			int res = 0;
			if (counter == 4)
				res = checkSecondCoordinate(inputFirstCoord, strlen(inputFirstCoord), 3, inputSecondCoord, strlen(inputSecondCoord));
			else if (counter == 5)
				res = checkSecondCoordinate(inputFirstCoord, strlen(inputFirstCoord), 2, inputSecondCoord, strlen(inputSecondCoord));
			else
				res = checkSecondCoordinate(inputFirstCoord, strlen(inputFirstCoord), 6 - counter, inputSecondCoord, strlen(inputSecondCoord));
			if (res != 0)
			{
				if (res == -1)
				{
					c.X = 51;
					c.Y = 7;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("ERROR: Wrong format of coordinate         ");
					continue;
				}
				else
				{
					c.X = 51;
					c.Y = 7;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("ERROR: Wrong coordinate, boat doesnt match");
					continue;
				}
			}
			if (!checkIfFieldIsAvailable(helpTable[0], inputSecondCoord, strlen(inputSecondCoord)))
			{
				c.X = 51;
				c.Y = 7;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				printf("ERROR: Too close to the placed boat         ");
				continue;
			}
			c.X = 51;
			c.Y = 7;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			printf("                                          ");
			break;
		}

		// prikazi na tabli
		char number[3];
		strcpy_s(number, inputFirstCoord + 1);
		int firstCoordNumber = atoi(number);
		strcpy_s(number, inputSecondCoord + 1);
		int secondCoordNumber = atoi(number);

		if (inputFirstCoord[0] == inputSecondCoord[0])
		{
			c.X = 5 + (inputFirstCoord[0] % 65) * 4;
			c.Y = 5;
			if (secondCoordNumber > firstCoordNumber)
			{
				c.Y = 5 + secondCoordNumber - 1;
				for (i = secondCoordNumber; i >= firstCoordNumber; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][inputFirstCoord[0] % 65] = 1;
					if (i == secondCoordNumber) //prvi prolaz
					{
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else if (i == firstCoordNumber) //poslednji prolaz
					{
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else //srednji prolaz
					{
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					c.Y--;
				}
			}
			else
			{
				c.Y = 5 + secondCoordNumber - 1;
				for (i = firstCoordNumber; i >= secondCoordNumber; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][inputFirstCoord[0] % 65] = 1;
					if (i == secondCoordNumber) //prvi prolaz
					{
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else if (i == firstCoordNumber) //poslednji prolaz
					{
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65] = 3;
						}
						if ((c.Y - 5 - 1 >= 0) && ((inputFirstCoord[0] % 65) + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][inputFirstCoord[0] % 65 + 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					else //srednji prolaz
					{
						if ((inputFirstCoord[0] % 65) - 1 >= 0)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 - 1] = 3;
						}
						if ((inputFirstCoord[0] % 65) + 1 <= 9)
						{
							helpTable[c.Y - 5][inputFirstCoord[0] % 65 + 1] = 3;
						}
					}
					c.Y++;
				}
			}
		}
		else
		{
			c.Y = 5 + firstCoordNumber - 1;
			if (inputSecondCoord[0] > inputFirstCoord[0])
			{
				c.X = 5 + (inputSecondCoord[0] % 65) * 4;
				for (i = inputSecondCoord[0]; i >= inputFirstCoord[0]; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][i % 65] = 1;
					if (i == inputSecondCoord[0]) //prvi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][i % 65 + 1] = 3;
						}
						if (i % 65 + 1 <= 9)
						{
							helpTable[c.Y - 5][i % 65 + 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][i % 65 + 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}

					}
					else if (i == inputFirstCoord[0]) //drugi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][i % 65 - 1] = 3;
						}
						if (i % 65 - 1 >= 0)
						{
							helpTable[c.Y - 5][i % 65 - 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][i % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					else
					{
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					c.X -= 4;
				}
			}
			else
			{
				c.X = 5 + (inputFirstCoord[0] % 65) * 4;
				for (i = inputFirstCoord[0]; i >= inputSecondCoord[0]; i--)
				{
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
					printf("#");
					helpTable[c.Y - 5][i % 65] = 1;
					if (i == inputFirstCoord[0]) //prvi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 - 1][i % 65 + 1] = 3;
						}
						if (i % 65 + 1 <= 9)
						{
							helpTable[c.Y - 5][i % 65 + 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 + 1 <= 9))
						{
							helpTable[c.Y - 5 + 1][i % 65 + 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}

					}
					else if (i == inputSecondCoord[0]) //drugi kraj broda
					{
						if ((c.Y - 5 - 1 >= 0) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 - 1][i % 65 - 1] = 3;
						}
						if (i % 65 - 1 >= 0)
						{
							helpTable[c.Y - 5][i % 65 - 1] = 3;
						}
						if ((c.Y - 5 + 1 <= 9) && (i % 65 - 1 >= 0))
						{
							helpTable[c.Y - 5 + 1][i % 65 - 1] = 3;
						}
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					else
					{
						if (c.Y - 5 + 1 <= 9)
						{
							helpTable[c.Y - 5 + 1][i % 65] = 3;
						}
						if (c.Y - 5 - 1 >= 0)
						{
							helpTable[c.Y - 5 - 1][i % 65] = 3;
						}
					}
					c.X -= 4;
				}
			}
		}

		c.X = 51;
		c.Y = 7 + counter;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		if (firstCoordNumber < secondCoordNumber)
			printf("Position of boat #%d : %s - %s                ", counter, inputFirstCoord, inputSecondCoord);
		else
			printf("Position of boat #%d : %s - %s                ", counter, inputSecondCoord, inputFirstCoord);
		counter++;

		if (counter == 6)
			break;
	}

	for (int i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			*(table + i * 10 + j) = helpTable[i][j] == 3 ? 0 : helpTable[i][j] == 1 ? 3 : 0;
		}
	}

	hidecursor();
}

void waiting() {
	_COORD c;
	c.X = 51;
	c.Y = 13;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("                                             ");

	c.Y = 14;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("Waiting for opponent boat placements");
}

void updateTimerUI(int seconds) {
	_COORD c;
	c.X = 56;
	c.Y = 0;

	if (seconds == 9) {
		c.X = 57;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		printf(" ");
		c.X = 56;
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("%d", seconds);
}

void gameStartUI(char* table) {
	system("cls");
	hidecursor();

	printf("\n");
	printf("                   YOUR TABLE			  	           OPPONENT TABLE\n");
	printf("     A   B   C   D   E   F   G   H   I   J       |       A   B   C   D   E   F   G   H   I   J\n");
	printf("   _________________________________________     |     _________________________________________\n");
	//printf("\n");
	printf(" 1 | "); printElement(*(table + 0 * 10 + 0)); printf(" | "); printElement(*(table + 0 * 10 + 1)); printf(" | "); printElement(*(table + 0 * 10 + 2)); printf(" | "); printElement(*(table + 0 * 10 + 3)); printf(" | "); printElement(*(table + 0 * 10 + 4)); printf(" | "); printElement(*(table + 0 * 10 + 5)); printf(" | "); printElement(*(table + 0 * 10 + 6)); printf(" | "); printElement(*(table + 0 * 10 + 7)); printf(" | "); printElement(*(table + 0 * 10 + 8)); printf(" | "); printElement(*(table + 0 * 10 + 9)); printf(" |"); printf("     |   1 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 2 | "); printElement(*(table + 1 * 10 + 0)); printf(" | "); printElement(*(table + 1 * 10 + 1)); printf(" | "); printElement(*(table + 1 * 10 + 2)); printf(" | "); printElement(*(table + 1 * 10 + 3)); printf(" | "); printElement(*(table + 1 * 10 + 4)); printf(" | "); printElement(*(table + 1 * 10 + 5)); printf(" | "); printElement(*(table + 1 * 10 + 6)); printf(" | "); printElement(*(table + 1 * 10 + 7)); printf(" | "); printElement(*(table + 1 * 10 + 8)); printf(" | "); printElement(*(table + 1 * 10 + 9)); printf(" |");	printf("     |   2 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 3 | "); printElement(*(table + 2 * 10 + 0)); printf(" | "); printElement(*(table + 2 * 10 + 1)); printf(" | "); printElement(*(table + 2 * 10 + 2)); printf(" | "); printElement(*(table + 2 * 10 + 3)); printf(" | "); printElement(*(table + 2 * 10 + 4)); printf(" | "); printElement(*(table + 2 * 10 + 5)); printf(" | "); printElement(*(table + 2 * 10 + 6)); printf(" | "); printElement(*(table + 2 * 10 + 7)); printf(" | "); printElement(*(table + 2 * 10 + 8)); printf(" | "); printElement(*(table + 2 * 10 + 9)); printf(" |");	printf("     |   3 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 4 | "); printElement(*(table + 3 * 10 + 0)); printf(" | "); printElement(*(table + 3 * 10 + 1)); printf(" | "); printElement(*(table + 3 * 10 + 2)); printf(" | "); printElement(*(table + 3 * 10 + 3)); printf(" | "); printElement(*(table + 3 * 10 + 4)); printf(" | "); printElement(*(table + 3 * 10 + 5)); printf(" | "); printElement(*(table + 3 * 10 + 6)); printf(" | "); printElement(*(table + 3 * 10 + 7)); printf(" | "); printElement(*(table + 3 * 10 + 8)); printf(" | "); printElement(*(table + 3 * 10 + 9)); printf(" |");	printf("     |   4 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 5 | "); printElement(*(table + 4 * 10 + 0)); printf(" | "); printElement(*(table + 4 * 10 + 1)); printf(" | "); printElement(*(table + 4 * 10 + 2)); printf(" | "); printElement(*(table + 4 * 10 + 3)); printf(" | "); printElement(*(table + 4 * 10 + 4)); printf(" | "); printElement(*(table + 4 * 10 + 5)); printf(" | "); printElement(*(table + 4 * 10 + 6)); printf(" | "); printElement(*(table + 4 * 10 + 7)); printf(" | "); printElement(*(table + 4 * 10 + 8)); printf(" | "); printElement(*(table + 4 * 10 + 9)); printf(" |");	printf("     |   5 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 6 | "); printElement(*(table + 5 * 10 + 0)); printf(" | "); printElement(*(table + 5 * 10 + 1)); printf(" | "); printElement(*(table + 5 * 10 + 2)); printf(" | "); printElement(*(table + 5 * 10 + 3)); printf(" | "); printElement(*(table + 5 * 10 + 4)); printf(" | "); printElement(*(table + 5 * 10 + 5)); printf(" | "); printElement(*(table + 5 * 10 + 6)); printf(" | "); printElement(*(table + 5 * 10 + 7)); printf(" | "); printElement(*(table + 5 * 10 + 8)); printf(" | "); printElement(*(table + 5 * 10 + 9)); printf(" |");	printf("     |   6 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 7 | "); printElement(*(table + 6 * 10 + 0)); printf(" | "); printElement(*(table + 6 * 10 + 1)); printf(" | "); printElement(*(table + 6 * 10 + 2)); printf(" | "); printElement(*(table + 6 * 10 + 3)); printf(" | "); printElement(*(table + 6 * 10 + 4)); printf(" | "); printElement(*(table + 6 * 10 + 5)); printf(" | "); printElement(*(table + 6 * 10 + 6)); printf(" | "); printElement(*(table + 6 * 10 + 7)); printf(" | "); printElement(*(table + 6 * 10 + 8)); printf(" | "); printElement(*(table + 6 * 10 + 9)); printf(" |");	printf("     |   7 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 8 | "); printElement(*(table + 7 * 10 + 0)); printf(" | "); printElement(*(table + 7 * 10 + 1)); printf(" | "); printElement(*(table + 7 * 10 + 2)); printf(" | "); printElement(*(table + 7 * 10 + 3)); printf(" | "); printElement(*(table + 7 * 10 + 4)); printf(" | "); printElement(*(table + 7 * 10 + 5)); printf(" | "); printElement(*(table + 7 * 10 + 6)); printf(" | "); printElement(*(table + 7 * 10 + 7)); printf(" | "); printElement(*(table + 7 * 10 + 8)); printf(" | "); printElement(*(table + 7 * 10 + 9)); printf(" |");	printf("     |   8 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf(" 9 | "); printElement(*(table + 8 * 10 + 0)); printf(" | "); printElement(*(table + 8 * 10 + 1)); printf(" | "); printElement(*(table + 8 * 10 + 2)); printf(" | "); printElement(*(table + 8 * 10 + 3)); printf(" | "); printElement(*(table + 8 * 10 + 4)); printf(" | "); printElement(*(table + 8 * 10 + 5)); printf(" | "); printElement(*(table + 8 * 10 + 6)); printf(" | "); printElement(*(table + 8 * 10 + 7)); printf(" | "); printElement(*(table + 8 * 10 + 8)); printf(" | "); printElement(*(table + 8 * 10 + 9)); printf(" |");	printf("     |   9 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	printf("10 | "); printElement(*(table + 9 * 10 + 0)); printf(" | "); printElement(*(table + 9 * 10 + 1)); printf(" | "); printElement(*(table + 9 * 10 + 2)); printf(" | "); printElement(*(table + 9 * 10 + 3)); printf(" | "); printElement(*(table + 9 * 10 + 4)); printf(" | "); printElement(*(table + 9 * 10 + 5)); printf(" | "); printElement(*(table + 9 * 10 + 6)); printf(" | "); printElement(*(table + 9 * 10 + 7)); printf(" | "); printElement(*(table + 9 * 10 + 8)); printf(" | "); printElement(*(table + 9 * 10 + 9)); printf(" |");	printf("     |  10 | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ | ~ |\n");
	
	printf("   \n");

}

void userInputFunction(char* userInput, int* counter) {
	_COORD c;
	c.X = 51;
	c.Y = 15;
	char input;

	while (*counter > 0)
	{
		if (_kbhit())
		{
			input = _getch();
			if (((input > 64 && input < 75) || (input > 47 && input < 58)) && strlen(userInput) < 3)
			{
				c.X++;
				userInput[strlen(userInput)] = input;

				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				printf("%c", input);
			}
			if (input == 8 && (strlen(userInput) > 0))
			{
				userInput[strlen(userInput) - 1] = 0;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				printf(" ");
				c.X--;
			}
			if ((input == '\r') && (strlen(userInput) >= 2) && (userInput[1] != '0'))
				break;

		}
	}
}

void resetInput() {
	_COORD c;
	c.Y = 14;
	c.X = 0;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("ERROR: Wrong format, see example!");

	c.X = 51;
	c.Y = 15;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("    ");

	c.X = 51;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void pauseCounterThread(HANDLE hThread) {
	DWORD suspend_retval = SuspendThread(hThread);
	if (suspend_retval == static_cast<DWORD>(-1))
	{
		// handle suspend error
		printf("ERROR on suspend\n");
		exit(1);
	}
}

void resumeCounterThread(HANDLE hThread) {
	DWORD resume_retval = ResumeThread(hThread);
	if (resume_retval == static_cast<DWORD>(-1))
	{
		// handle resume error
		printf("ERROR on resume\n");
		exit(1);
	}
}

void opponentsTurn() {
	_COORD c;
	c.Y = 0;
	c.X = 36;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("WAITING FOR OPPONENT'S MOVE");

	c.Y = 14;
	c.X = 0;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("                                                  ");

	c.Y = 15;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("                                                               ");
}

void myTurn() {
	_COORD c;
	c.Y = 0;
	c.X = 36;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("     TICKING TIMER: 30      ");

	c.Y = 15;
	c.X = 0;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("Enter your next move in format [A-J,1-10] (ex. A5): ");
}

void tryAgain(char* userInput) {
	_COORD c;
	c.Y = 14;
	c.X = 0;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("ERROR: You already tried field %s, try another one", userInput);

	c.Y = 15;
	c.X = 51;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("    ");

	c.X = 51;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void changeTableField(int player, int i, int j, char* table, char element) {
	*(table + i * 10 + j) = element;

	_COORD c;
	c.Y = 4 + i;
	c.X = (player - 1) * 52 + 5 + j * 4;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printElement(element);
}

void victory() {
	system("cls");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");

	printf("			__      _______ _____ _______ ____ _______     __\n");
	printf("			\\ \\    / /_   _/ ____|__   __/  _ \\|  __\\ \\   / /\n");
	printf("			 \\ \\  / /  | || |       | |  | | | | |__)\\ \\_/ /\n");
	printf("			  \\ \\/ /   | || |       | |  | | | |  _  /\\   /\n");
	printf("			   \\  /   _| || |____   | |  | |_| | | \\ \\ | |\n");
	printf("			    \\/   |_____\\_____|  |_|  \\____/|_|  \\_\\|_|\n");

	printf("\n");
	printf("				    Press Enter to continue...");
	printf("\n");


}

void defeat() {
	system("cls");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");

	printf("			    _____  ______ ______ _____        _______		\n");
	printf("			   |  __ \\| ____ | ____ | ____|    /\\|__   __|	\n");
	printf("			   | |  | | |__  | |__  | |__     /  \\  | |		\n");
	printf("			   | |  | |  __| |  __| |  __|   / /\\ \\ | |		\n");
	printf("			   | |__| | |____| |    | |____ / ____ \\| |		\n");
	printf("			   |_____/|______|_|    |______/_/    \\_\\_|		\n");

	printf("\n");
	printf("				    Press Enter to continue...");
	printf("\n");
}