#include "GameHelper.h"

bool GameOver(char board[10][10]) {
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (board[i][j] == 3) {
				return false;
			}
		}
	}

	return true;
}