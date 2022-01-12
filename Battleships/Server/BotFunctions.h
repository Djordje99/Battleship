#pragma once
#ifndef BOTFUNC
#define BOTFUNC

struct COORDINATES {
	int X;
	int Y;
};

COORDINATES botAim(char* aimingTable);

void botTableInitialization(char* table);

#endif // !BOTFUNC

