#ifndef PLAYER_H
#define PLAYER_H

#include "Play.h"

typedef pair<string, string> SingleLine;

class Player
{
public:
	Player(Play& p) : play(p), busy(false), isActive(false), sceneCount(0), retrieving(true) {}
	void enter(int sceneCounter, string name, string script);

	void exit();
	void activate();
	void deactive();
	void join();
	void reset();
	void clearLines();

	bool isbusy();
	
private:
	void read(string name, string script);
	void act();

	bool busy;
	bool isActive;
	bool retrieving;

	int sceneCount;

	Line lines;
	LineIter iter;
	Play& play;
	thread t;
};

#endif

