#include "Defines.h"
#include <time.h>
#include <stdlib.h>

int getRandomNumber() {
	struct timespec now;

	timespec_get(&now, TIME_UTC);
	srand(time(NULL) + now.tv_sec + now.tv_nsec);

	return randnum(0, 9);
}