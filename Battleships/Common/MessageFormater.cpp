#include "pch.h"
#include "Message.h"
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

message FormatMessageStruct(MessageType type, ActionPlayer player, char arg1, char arg2) {
	message msg;

	msg.type = type;
	msg.player = player;
	/*if(arg1 != NULL)
		strcpy(msg.argumet, arg1);
	if(arg2 != NULL)
		strcpy(msg.aditionalArgumet, arg2);*/
	msg.argument[0] = arg1;
	msg.argument[1] = arg2;
	msg.argument[2] = '/0';

	return msg;
}

char* MessageTypeToString(MessageType type) {
	char* typeString = 0;

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
	char* playerString = 0;

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

char* MessageToString(message msg) {
	char* stringMsg = 0;
	char* msgType = 0;
	char* msgPlayer = 0;
	char* msgArg1 = 0;
	char* msgArg2 = 0;

	strcpy(msgType, "Message type is: ");
	strcpy(msgPlayer, "Player is: ");
	strcpy(msgArg1, "Argumet1 is: ");
	strcpy(msgArg2, "Argumet2 is: ");
	
	strcat(msgType, MessageTypeToString(msg.type));
	strcat(msgPlayer, PlayerToString(msg.player));
	//strcat(msgArg1, msg.argumet);
	//strcat(msgArg2, msg.aditionalArgumet);

	strcpy(stringMsg, msgType);
	strcpy(stringMsg, "\n");
	strcpy(stringMsg, msgPlayer);
	strcpy(stringMsg, "\n");
	strcpy(stringMsg, msgArg1);
	strcpy(stringMsg, "\n");
	strcpy(stringMsg, msgArg2);
	strcpy(stringMsg, "\n");

	return stringMsg;
}