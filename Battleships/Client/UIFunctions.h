#pragma once
#ifndef UIFUNCTIONS_H
#define UIFUNCTIONS_H
enum State { UP = 1, DOWN };

void setWindowSize();

void hidecursor();

void startUpScreen();

void startingMenu();

int chooseGameType();

void gameInProgress();

void pressEnterToContinue();

#endif // !UIFUNCTIONS_H
