#pragma once
#ifndef QUEUE_H
#define QUEUE_H

typedef struct ELEMENT
{
	ELEMENT* next;
	char value[1024];

}element;

void Enqueue(char* message, element** root); //will be changed to matrix or cordinates
char* Dequeue(element **root);
void InitQueue(element** root);
int QueueCount(element* root);

#endif // !QUEUE_H
