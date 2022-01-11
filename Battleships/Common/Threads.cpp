#include "pch.h"
#include "Defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void pauseThread(HANDLE hThread) {
	if (hThread)
	{
		DWORD suspend_retval = SuspendThread(hThread);
		if (suspend_retval == static_cast<DWORD>(-1))
		{
			// handle suspend error
			printf("ERROR on suspend\n");
			exit(1);
		}
	}
}

void resumeThread(HANDLE hThread) {
	if (hThread)
	{
		DWORD resume_retval = ResumeThread(hThread);
		if (resume_retval == static_cast<DWORD>(-1))
		{
			// handle resume error
			printf("ERROR on resume\n");
			exit(1);
		}
	}
}