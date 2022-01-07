#include "BotFunctions.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <wincrypt.h>
#include <stdlib.h>

char* botAim(char* aimingTable) {
	HCRYPTPROV   hCryptProv;
	BYTE         pbData[2];

	int i, j;

	CryptAcquireContext(
		&hCryptProv,
		NULL,
		(LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT);

	bool validAim = false;

	while (!validAim)
	{
		if (CryptGenRandom(hCryptProv, 2, pbData))
		{

			i = pbData[0] % 10;
			j = pbData[1] % 10;

			if (*(aimingTable + i * 10 + j) == 0)
			{
				*(aimingTable + i * 10 + j) = 1;
			}
			else
				continue;
		}

		validAim = true;
	}

	char* ret = (char*)malloc(3);
	*(ret + 0) = (char)i;
	*(ret + 1) = (char)j;
	*(ret + 2) = '\0';

	return ret;
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

void botTableInitialization(char* table) {
	_COORD c;
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
					c.X += 4;
				}
			}
		}

		counter++;

		if (counter == 6)
			break;
	}

	for (int i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			*(table + i * 10 + j) = helpTable[i][j] == 3 ? 0 : helpTable[i][j] == 1 ? 3 : 0;
		}
	}

}