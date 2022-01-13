#pragma once
#ifndef BOTFUNC
#define BOTFUNC

/*
	Struktura za povratnu vrednost botAim metode
*/
struct MATRIX_FIELD {
	int I;
	int J;
};

/*
	botAim
	-------------------------------------------------------------------
	Random bira polje koje gadja BOT, a koje pre toga nije gadjao. 
	-------------------------------------------------------------------
	aimingTable: matrica u koju se upisuju pogadjanja
	-------------------------------------------------------------------
	Povratna vrednost:
	MATRIX_FIELD.I: red u matrici koji se gadja
	MATRIX_FIELD.J: kolona u matrici koji se gadja
*/
MATRIX_FIELD botAim(char* aimingTable);
/*
	botTableInitialization
	-------------------------------------------------------------------
	Random postavlja brodove na tablu koju ce gadjati klijent.
	-------------------------------------------------------------------
	table: matrica u koju se postavljaju brodovi
*/
void botTableInitialization(char* table);

#endif // !BOTFUNC

