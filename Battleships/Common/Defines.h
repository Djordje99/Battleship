#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#define SAFE_DELETE_HANDLE(a) if(a){if(CloseHandle(a))printf("closed\n");} 

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

#endif
