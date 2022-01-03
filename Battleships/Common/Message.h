#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType {
	CHOOSE_GAME,
	PLACE_BOAT,
	PLACE_BOAT_CLIENT,
	PLACE_BOAT_CLIENT_OPONENT,
	AIM_BOAT,
	READY,
	BUSY,
	HIT,
	MISS,
	DEFEAT,
	VICTORY,
	RECONNECT,
	PLAY
};

enum ActionPlayer {
	NONE = 0,
	FIRST = 1,
	SECOND = 2
};

typedef struct MESSAGE_TCP {
	MessageType type;
	ActionPlayer player;
	char argument[4];
	char matrixArgumetn[10][10];
	//int argument;
	//char aditionalArgumet[4];
}message;

#endif