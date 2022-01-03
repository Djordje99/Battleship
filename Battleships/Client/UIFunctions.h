#pragma once
#ifndef UIFUNCTIONS_H
#define UIFUNCTIONS_H
#include <windows.h>

enum State { UP = 1, DOWN };

void setWindowSize();

void hidecursor();

void startUpScreen();

void startingMenu();

int chooseGameType();

void gameInProgress();

void pressEnterToContinue();

void printElement(char printEl);

void tableInitialization(char* table);

void waiting();

void updateTimerUI(int seconds);

void gameStartUI(char *table);

void userInputFunction(char* userInput, int* counter);

void pauseCounterThread(HANDLE hThread);

void resumeCounterThread(HANDLE hThread);

void opponentsTurn();

void myTurn();

void tryAgain(char* userInput);

void changeTableField(int player, int i, int j, char* table, char element);

void victory();

void defeat();

#endif // !UIFUNCTIONS_H
