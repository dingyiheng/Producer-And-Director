#ifndef H_DRT_HEADER
#define H_DRT_HEADER

#include "Play.h"
#include "Player.h"

#include <iostream>
#include <regex>

#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Svc_Handler.h>
#include <ace/Time_Value.h>

using namespace std;

#define MAXIMUM 9999

//ACT
typedef promise<bool> endPromise;

//play and player datatype
typedef shared_ptr<Play> playPtr;
typedef shared_ptr<Player> playerPtr;
typedef vector<shared_ptr<Player>> playerContainer;

//data structure to store the script information
typedef map<string, string> script;
typedef map<int, script> scriptsContainer;

typedef map<string, pair<sceneTitles, scriptsContainer>> playDict;

//exception
typedef shared_future<bool> result;

typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> svcHandler;

class Director : public svcHandler
{
public:
	~Director();

	virtual int open(void* p);
	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act = 0);

	void init(vector<string>& files, int minCount);
	void cue(string playname);
	//function for emergency deactivation for players
	void emergencyStop();

private:
	int maximum;

	//flag to deactivate the player threads
	endPromise finished;
	
	//the major datastructure for info
	playDict playDictionary;
	//the current script for playing
	scriptsContainer scripts;
	//the current play's name
	string currentPlay;


	//the pointer to store the play and container for player
	playPtr play;
	playerContainer players;

	//the future to throw exception through threads pool thread
	result sharedFut;

	string generateCommand();
	int suicide();

	void releaseAllplayer();
	void clearAllLines();

	mutex m_playerExit;
	condition_variable cv_playerExit;
};


#endif
