#include "pch.h"
#include "Message.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

message FormatMessage(MessageType type, ActionPlayer player, char* arg1, char* arg2) {
	message msg{
		msg.type = type,
		msg.player = player,
		strcpy(msg.argumet, arg1),
		strcpy(msg.aditionalArgumet, arg2),
	};

	return msg;
}

char* MessageToString(message msg) {
	char* stringMsg;
	char* msgType;
	char* msgPlayer;
	char* msgArg1;
	char* msgArg2;

	strcpy(msgType, "Message type is: ");
	strcpy(msgPlayer, "Player is: ");
	strcpy(msgArg1, "Argumet1 is: ");
	strcpy(msgArg2, "Argumet2 is: ");
	
	strcat(msgType, MessageTypeToString(msg.type));
	strcat(msgPlayer, PlayerToString(msg.player));
	strcat(msgArg1, msg.argumet);
	strcat(msgArg2, msg.aditionalArgumet);

	strcpy(stringMsg, msgType);
	strcat(stringMsg, "\n");
	strcat(stringMsg, msgPlayer);
	strcat(stringMsg, "\n");
	strcat(stringMsg, msgArg1);
	strcat(stringMsg, "\n");
	strcat(stringMsg, msgArg2);
	strcat(stringMsg, "\n");

	return stringMsg;
}

char* MessageTypeToString(MessageType type) {
	char* typeString;

	switch (type)
	{
	case CHOOSE_GAME:
		strcpy(typeString, "CHOSE_GAME");
		break;
	case PLACE_BOAT:
		strcpy(typeString, "PLACE_BOAT");
		break;
	case AIM_BOAT:
		strcpy(typeString, "AIM_BOAT");
		break;
	case READY:
		strcpy(typeString, "READY");
		break;
	case BUSY:
		strcpy(typeString, "BUSY");
		break;
	case HIT:
		strcpy(typeString, "HIT");
		break;
	case MISS:
		strcpy(typeString, "MISS");
		break;
	case DEFEAT:
		strcpy(typeString, "DEFEAT");
		break;
	case VICTORY:
		strcpy(typeString, "VICTORY");
		break;
	default:
		strcpy(typeString, "Wrong MessageType");
		break;
	}

	return typeString;
}

char* PlayerToString(ActionPlayer player) {
	char* playerString;

	switch (player)
	{
	case FIRST:
		strcpy(playerString, "FIRST");
		break;
	case SECOND:
		strcpy(playerString, "SECOND");
		break;
	default:
		strcpy(playerString, "Wrong ActionPlayer");
		break;
	}

	return playerString;
}