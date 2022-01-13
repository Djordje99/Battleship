#pragma once
#ifndef GAMEHELPER_H
#define GAMEHELPER_H
/*
	GameOver
	-------------------------------------------------------------------
	GameOver - omogucava da se proveri da li su svi brodovi pogodjeni i tada odredjuje da li je kraj igre.
	char board[10][10] - predstavlja tabelu koja se proverava
	-------------------------------------------------------------------
	Povratna vrednost:
	bool:
		true - ako je kraj igre
		false ako se igra i dalje igra
*/
bool GameOver(char board[10][10]);
#endif // !GAMEHELPER_H
