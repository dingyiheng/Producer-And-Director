#ifndef PLAY_H
#define PLAY_H

#include <mutex>
#include <condition_variable>
#include <exception>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <regex>
#include <memory>
#include <future>

#include "CodeException.h"

using namespace std;

//ACT
typedef future<bool> endFuture;

//sceneTitle data type
typedef vector<string> sceneTitles;
typedef sceneTitles::iterator sceneCountIter;

//lines datastructure 
typedef map<int, pair<string, string>> Line;
typedef Line::iterator LineIter;

class Play
{
public:
	Play(endFuture&& fut) : on_stage(0), waiting(0), line_counter(1), scene_fragment_counter(1), firstLine(true), stopflag(false) {
		finishFlag = move(fut);
		sceneNameIter = sceneRef.begin();
	}
	void recite(LineIter& iter, int sceneCount);
	void enter(int sceneCount);
	void exit();
	void checkAvailable(int max);
	bool checkPlayFinished();
	void emergentStop();
	void releaseWhenStop();
	void setSceneTitles(const sceneTitles& titles);
	void resetWholePlay();
	void setFlag(bool flag);
	void checkCurrentShowFinished();

	bool checkStopFlag();




	//# new added
	void setLineCounter(int t);
private:
	int line_counter;
	int scene_fragment_counter;
	int on_stage;
	int waiting;
	//The boolean value used to determine whether it's firstLine right now
	bool firstLine;
	bool stopflag;
	string currentCharacter;

	mutex m;
	mutex m_enter;
	mutex m_check;
	mutex m_current_show;
	condition_variable cv;
	condition_variable cv_enter;
	condition_variable cv_cue;
	condition_variable cv_current_show_end;
	sceneTitles sceneRef;
	sceneCountIter sceneNameIter;
	endFuture finishFlag;

	//reset current character and first line variables
	void resetCCandFL();
};

#endif PLAY_H

