#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType {
	CHOOSE_GAME,
	PLACE_BOAT,
	AIM_BOAT,
	READY,
	BUSY,
	HIT,
	MISS,
	DEFEAT,
	VICTORY
};

enum ActionPlayer {
	FIRST = 1,
	SECOND = 2
};

typedef struct MESSAGE_TCP {
	MessageType type;
	ActionPlayer player;
	char* argumet;
	char* aditionalArgumet;
}message;

#endif