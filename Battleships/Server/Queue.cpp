#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>
#include "Queue.h"

CRITICAL_SECTION cs;

void Enqueue(message* message, element **root) {
	EnterCriticalSection(&cs);

	element* temp = *root;

	element* newEl = (element*)malloc(sizeof(element));
	newEl->next = temp;
	newEl->msg = message;

	*root = newEl;

	LeaveCriticalSection(&cs);
}

message* Dequeue(element **root) {
	EnterCriticalSection(&cs);

	if (*root == NULL) {
		LeaveCriticalSection(&cs);
		return NULL;
	}


	int count = QueueCount(*root);

	element* temp = *root;

	for (int i = 0; i < count - 2; i++)
	{
		temp = temp->next;
	}

	message* ret;

	if (count != 1) {
		element* del = temp->next;
		ret = del->msg;
		free(del);
		del = NULL;
		temp->next = NULL;
	}
	else {
		ret = temp->msg;
		free(temp);
		temp = NULL;
		*root = NULL;
	}

	LeaveCriticalSection(&cs);

	return ret;
}

void InitQueue(element** root) {
	InitializeCriticalSection(&cs);
	*root = NULL;
}

int QueueCount(element* root) {
	int count = 0;
	element* temp = root;

	while (temp != NULL) {
		count++;
		temp = temp->next;
	}

	return count;
}