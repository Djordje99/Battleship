#pragma once
#ifndef BOTFUNC
#define BOTFUNC

struct MATRIX_FIELD {
	int I;
	int J;
};

MATRIX_FIELD botAim(char* aimingTable);

void botTableInitialization(char* table);

#endif // !BOTFUNC

