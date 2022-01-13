#pragma once
#include "../Common/Message.h"
#ifndef QUEUE_H
#define QUEUE_H

typedef struct ELEMENT
{
	ELEMENT* next;
	message* msg;

}element;

void Enqueue(message* message, element** root);
/*
	Enqueue
	-------------------------------------------------------------------
	Enqueue - postavlja poruku na queue
	message* message - pokazivac na formiranu poruku tipa MESSAGE_TCP
	element** root - dupli pokazivac na koren queue-a (dupli jer se menja sadrzaj)
	-------------------------------------------------------------------
	Povratna vrednost: void
*/
void Enqueue(message* message, element** root);

/*
	Dequeue
	-------------------------------------------------------------------
	Dequeue - uzima vrednost sa queue-a
	element** root - dupli pokazivac na koren queue-a (dupli jer se menja sadrzaj)
	-------------------------------------------------------------------
	Povratna vrednost:
	message* - pokazivac na poruku koja se uzela sa queue-a. (MESSAGE_TCP)
*/
message* Dequeue(element **root);

/*
	InitQueue
	-------------------------------------------------------------------
	InitQueue - inicijalizacija queue-a
	element** root - dupli pokazivac na koren queue-a (dupli jer se menja sadrzaj)
	-------------------------------------------------------------------
	Povratna vrednost: void
*/
void InitQueue(element** root);

/*
	QueueCount
	-------------------------------------------------------------------
	QueueCount - proverava koliko ima poruka u queue
	element* root - pokazivac na koren queue-a
	-------------------------------------------------------------------
	Povratna vrednost: 
	int - broj poruka u redu.
*/
int QueueCount(element* root);

#endif // !QUEUE_H
