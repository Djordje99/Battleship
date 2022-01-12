#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#define SAFE_DELETE_HANDLE(a) if(a){if(CloseHandle(a))printf("closed\n");} 
#define randnum(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

/*
	pauseThread
	-------------------------------------------------------------------
	Funkcija za pauziranje threada
	-------------------------------------------------------------------
	hThread: handle threada koji zelimo da pauziramo
*/
void pauseThread(HANDLE hThread);
/*
	resumeThread
	-------------------------------------------------------------------
	Funkcija za nastavljanje rada threada
	-------------------------------------------------------------------
	hThread: handle threada koji zelimo da nastavi sa radom
*/
void resumeThread(HANDLE hThread);
/*
	getRandomNumber
	-------------------------------------------------------------------
	Funkcija vraca random broj u intervalu [0,9]
	-------------------------------------------------------------------
*/
int getRandomNumber();

#endif
