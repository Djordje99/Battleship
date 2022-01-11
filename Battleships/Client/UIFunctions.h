#pragma once
#ifndef UIFUNCTIONS_H
#define UIFUNCTIONS_H
/*
	Enum opisuje stanje strelice na chooseGameType screenu
*/
enum State { UP = 1, DOWN };
/*
	setWidowSize
	-------------------------------------------------------------------
	Postavlja velicinu klijentskog ekrana na definisanu vrednost
	-------------------------------------------------------------------
*/
void setWindowSize();
/*
	hidecursor
	-------------------------------------------------------------------
	Sakriva treptuci beli kursor
	-------------------------------------------------------------------
*/
void hidecursor();
/*
	showcursor
	-------------------------------------------------------------------
	Prikazuje treptuci beli kursor
	-------------------------------------------------------------------
*/
void showcursor();
/*
	startUpScreen
	-------------------------------------------------------------------
	UI prikaz logoa na pocetnom ekranu
	-------------------------------------------------------------------
*/
void startUpScreen();
/*
	startingMenu
	-------------------------------------------------------------------
	UI prikaz menija za odabir igre
	-------------------------------------------------------------------
*/
void startingMenu();
/*
	chooseGameType
	-------------------------------------------------------------------
	Menja UI prikaz prilikom selektovanja igre i prati input sa 
	tastature ocekivajuci strelice up/down
	-------------------------------------------------------------------
	Povratna vrednost:
	State UP = 1 - BOT game
	State DOWN = 2 - 2 player game
*/
int chooseGameType();
/*
	gameInProgress
	-------------------------------------------------------------------
	UI prikaz poruke da je igra u toku
	-------------------------------------------------------------------
*/
void gameInProgress();
/*
	pressEnterToContinue
	-------------------------------------------------------------------
	Cekanje na Enter da bi se funkcija zavrsila
	Koristi se pri zatvaranju prozora
	-------------------------------------------------------------------
*/
void pressEnterToContinue();
/*
	printElement
	-------------------------------------------------------------------
	UI prikaz elementa na tabli
	-------------------------------------------------------------------
	printEl: char vrednost koja se nalazi na odredjenom polju table
*/
void printElement(char printEl);
/*
	tableInitialization
	-------------------------------------------------------------------
	Funkcija za inicijalizaciju table za igru
	Startovanje funkcije postavlja UI prikaz za inicijalizaciju i prima
	input za postavljanje brodova. Brodove prikazuje na tabli
	-------------------------------------------------------------------
	table: pokazivac na matricu u kojoj se postavljaju brodovi
*/
void tableInitialization(char* table);
/*
	waiting
	-------------------------------------------------------------------
	UI prikaz poruke za cekanje na drugog igraca
	-------------------------------------------------------------------
*/
void waiting();
/*
	updateTimerUI
	-------------------------------------------------------------------
	UI prikaz tajmera za igru, prikazuje koliko je sekundi preostalo za
	odigravanje koraka
	-------------------------------------------------------------------
	seconds: broj sekundi koje je potrebno prikazati
*/
void updateTimerUI(int seconds);
/*
	gameStartUI
	-------------------------------------------------------------------
	UI prikaz kada igra pocne
	-------------------------------------------------------------------
	table: pokazivac na matricu u kojoj su postavljeni brodovi igraca
*/
void gameStartUI(char *table);
/*
	userInputFunction
	-------------------------------------------------------------------
	Funkcija kontrolise unos igraca u toku igre i  prikazuje je ga na 
	ekranu. U slucaju da je isteklo vreme za potez funkcija prestaje sa 
	radom
	-------------------------------------------------------------------
	userInput: pokazivac na bufer za input
	counter: pokazivac na counter tajmera
*/
void userInputFunction(char* userInput, int* counter);
/*
	resetInput
	-------------------------------------------------------------------
	Funkcija za UI ispis u slucaju greske pri unosu u toku igre
	-------------------------------------------------------------------
*/
void resetInput();
/*
	opponentsTurn
	-------------------------------------------------------------------
	UI prikaz tokom cekanja na potez drugog igraca
	-------------------------------------------------------------------
*/
void opponentsTurn();
/*
	myTurn
	-------------------------------------------------------------------
	UI prikaz tokom poteza igraca
	-------------------------------------------------------------------
*/
void myTurn();
/*
	tryAgain
	-------------------------------------------------------------------
	UI prikaz prilikom greske pri unosu, u slucaju da igrac pokusa da 
	pogadja polje koje je ranije pogadjao
	-------------------------------------------------------------------
*/
void tryAgain(char* userInput);
/*
	changeTableField
	-------------------------------------------------------------------
	UI prikaz izmene na tablama tokom igre, nakon poteza
	igraca ili protivnika i ubacivanje elementa u odgovarajucu
	matricu tabele
	-------------------------------------------------------------------
	player: FIRST/SECOND u zavisnoti od toga koji je igrac imao potez
	i: red u matrici tabele
	j: kolona u matrici tabele
	table: pokazivac na matricu tabele
	element: element koji se ubacuje u tabelu i koji se prikazuje na UI
*/
void changeTableField(int player, int i, int j, char* table, char element);
/*
	victory
	-------------------------------------------------------------------
	UI prikaz u slucaju da je igrac pobedio
	-------------------------------------------------------------------
*/
void victory();
/*
	defeat
	-------------------------------------------------------------------
	UI prikaz u slucaju da je igrac izgubio
	-------------------------------------------------------------------
*/
void defeat();

#endif // !UIFUNCTIONS_H
