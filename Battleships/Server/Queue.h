#pragma once
#include "../Common/Message.h"
#ifndef QUEUE_H
#define QUEUE_H

typedef struct ELEMENT
{
	ELEMENT* next;
	message* msg;

}element;

void Enqueue(message* message, element** root); //will be changed to matrix or cordinates
message* Dequeue(element **root);
void InitQueue(element** root);
int QueueCount(element* root);

#endif // !QUEUE_H
