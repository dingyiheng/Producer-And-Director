#include "Director.h"

Director::~Director()
{
	cout << "--->Director Destructed!" << endl;
	finished.set_value(true);
	for (int i = 0; i < maximum; i++) {
		players[i]->join();
	}
}

string Director::generateCommand(){
	string forwardMsg = "";
	for (auto play : playDictionary){
		if (currentPlay == ""){
			forwardMsg += "[" + play.first + "] <available>";
		}
		else{
			if (play.first == currentPlay){
				forwardMsg += "[" + play.first + "] <in progress>";
			}
			else{
				forwardMsg += "[" + play.first + "] <unavailable>";
			}
		}
	}

	return forwardMsg;
}

int Director::open(void* p){
	if (svcHandler::open(p) == -1){
		cerr << "--->Director open failed" << endl;
		return -1;
	}
	cout << "Director Found!" << endl;

	//build up the forward message
	string forwardMsg = generateCommand();

	if (peer().send_n(forwardMsg.c_str(), forwardMsg.length() + 1) == -1){
		cerr << "Director sending message error" << endl;
		ACE_Reactor::instance()->end_reactor_event_loop();
		return -1;
	}
	
	return SUCCESS;
}

int Director::suicide(){
	string forwardMsg = "suicide";

	if (peer().send_n(forwardMsg.c_str(), forwardMsg.length() + 1) == -1){
		cerr << "Director sending message error" << endl;
	}

	return -1;
}

int Director::handle_input(ACE_HANDLE h){
	cout << "--->Entering handle input" << endl;
	char buf[BUFSIZ] = "";
	if (peer().recv(buf, BUFSIZ) == -1){
		cerr<< "--->Producer closed" << endl;
		ACE_Reactor::instance()->end_reactor_event_loop();
		emergencyStop();
		return -1;
	}

	//########################################### for test
	cout << "--->Command received: " << buf << endl;

	string command = buf;
	regex re("^([\\w]+)(\\s([\\w\\d\\s_]+))*$");
	smatch sm;
	regex_match(command, sm, re);


	if (sm[1] == "start" && sm[3].matched == true && currentPlay == ""){
		currentPlay = sm[3];
		string forwardMsg = generateCommand();

		if (peer().send_n(forwardMsg.c_str(), forwardMsg.length() + 1) == -1){
			cerr << "Director sending message error" << endl;
			ACE_Reactor::instance()->end_reactor_event_loop();
			return -1;
		}
		cue(currentPlay);
	}
	else if (sm[1] == "stop" && sm[3].matched == true && currentPlay != "" && !play->checkStopFlag()){
		play->setFlag(true);
		releaseAllplayer();
		play->emergentStop();
		while (play->checkStopFlag()){
			play->releaseWhenStop();
		}
	}
	else if(sm[1] == "quit"){
		emergencyStop();
		return suicide();
	}

	//############################################# test
	cout << "--->Leaving handle input" << endl;
	return SUCCESS;
}

int Director::handle_timeout(const ACE_Time_Value &current_time, const void *act){
	if (currentPlay != "" && !play->checkStopFlag()){
		int minimumCounter = MAXIMUM;
		int playerLineCounter = 0;

		for (int i = 0; i < maximum; i++) {
			if (!players[i]->checkRetrieving() && players[i]->isbusy()){
				if (players[i]->checkWaitingRecite()){
					playerLineCounter = players[i]->getCurrentLineCounter();
					minimumCounter = playerLineCounter < minimumCounter ? playerLineCounter : minimumCounter;
				}
				else{
					return SUCCESS;
				}
			}
		}

		play->setLineCounter(minimumCounter);
	}
	return SUCCESS;
}

// init
void Director::init(vector<string>& files, int minCount){
	maximum = 0;
	currentPlay = "";

	ACE_Time_Value delay(0);
	ACE_Time_Value interval(1);
	ACE_Reactor::instance()->schedule_timer(this, 0, delay, interval);

	for (auto file : files){
		sceneTitles titles;
		scriptsContainer scripts;
		ifstream infile(file);
		if (!infile.is_open()) {
			string error = "[ERROR]:  Open file fail: " + file;
			throw CodeException(FAIL_FILE_OPEN, error.c_str());
		}
		string currentLine;
		bool followingPL;
		int scriptCounter = 1;
		int lastPartDefCount = 0;

		regex re("^\\[scene\\][\\s]([\\w\\s]*)$");
		smatch sm;
		regex re_config("^[\\s]*(.*.txt)[\\s]*$");
		smatch sm_config;
		regex re_part("^[\\s]*([\\w]+)[\\s]+(.*.txt)[\\s]*$");
		smatch sm_part;

		while (getline(infile, currentLine)) {
			// scan for scene titles
			if (regex_match(currentLine, sm, re)) {
				titles.push_back(sm[1]);
				followingPL = false;
			}
			else {
				// scan for scene fragment config files
				if (regex_match(currentLine, sm_config, re_config)) {
					scripts[scriptCounter] = script();

					if (!followingPL) {
						followingPL = true;
					}
					else {
						titles.push_back(string());
					}
					ifstream configFile(sm_config[1]);
					int partDefinitionLineCount = 0;

					if (!configFile.is_open()) {
						string temp = sm_config[1];
						string error = "[ERROR]:  Open file fail: " + temp;
						throw CodeException(FAIL_FILE_OPEN, error.c_str());
					}

					string partDefinitionLine;
					while (getline(configFile, partDefinitionLine)) {
						// scan for part definition files
						if (regex_match(partDefinitionLine, sm_part, re_part)) {
							partDefinitionLineCount++;
							scripts[scriptCounter][sm_part[1]] = sm_part[2];
						}
					}

					scriptCounter++;
					
					int consecutiveSum = partDefinitionLineCount + lastPartDefCount;
					lastPartDefCount = partDefinitionLineCount;
					maximum = maximum > consecutiveSum ? maximum : consecutiveSum;
					

					configFile.close();
				}
			}
		}

		size_t pos = file.find(".txt");
		string dicKey = file.substr(0, pos);

		playDictionary[dicKey] = pair<sceneTitles, scriptsContainer>(titles, scripts);

		infile.close();
	}

	//finish reading script
	try {
		play = playPtr(new Play(finished.get_future()));
	}
	catch (exception& e) {
		throw (BAD_ALLOCATION, e);
	}

	// check the override option
	maximum = maximum > (int)minCount ? maximum : (int)minCount;


	for (int i = 0; i < maximum; i++) {
		try {
			players.push_back(playerPtr(new Player(*play)));
		}
		catch (exception& e) {
			throw (BAD_ALLOCATION, e);
		}

		players[i]->activate();
	}
}


void Director::cue(string playName)
{
	scripts = playDictionary[playName].second;

	releaseAllplayer();
	play->emergentStop();
	play->resetWholePlay();
	play->setSceneTitles(playDictionary[playName].first);
	clearAllLines();

	//reset play and set titles
	sharedFut = async(launch::async, [&]{
		[&]{
			for (auto script : scripts){
				for (auto part : script.second){
					play->checkAvailable(maximum);
					if (play->checkStopFlag()){
						return;
					}
					for (int i = 0; i < maximum; i++) {
						if (!players[i]->isbusy()) {
							try {
								players[i]->enter(script.first, part.first, part.second);
								if (play->checkStopFlag()){
									return;
								}
							}
							catch (CodeException& e) {
								// if there is an exception, need to stop all the currently running threads
								if (e.errCode() == SCENE_COUNTER_ERROR){
									play->setFlag(true);
									return;
								}
								else{
									emergencyStop();
									this->close();
								}
							}
							break;
						}
					}
				}
			}
		}();

		cout << "%%%%%%%%%% exit loop" << endl;
		if (!play->checkStopFlag()){
			cout << "--------------anchor 1" << endl;
			play->checkCurrentShowFinished();
			cout << "--------------anchor 1 finish" << endl;
		}
		
		if (play->checkStopFlag()){
			/*for (auto player : players){
				unique_lock<mutex> lock(m_playerExit);
				cv_playerExit.wait(lock, [&]{return player->isbusy(); });
			}*/
			cout << "--------------anchor 1" << endl;
			play->setFlag(false);
		}

		currentPlay = "";
		string forwardMsg = generateCommand();

		if (peer().send_n(forwardMsg.c_str(), forwardMsg.length() + 1) == -1){
			cerr << "Director sending message error" << endl;
			ACE_Reactor::instance()->end_reactor_event_loop();
		}

		cout << "--------------anchor 3" << endl;

		return true;

	}).share();
}

void Director::emergencyStop() {
	for (int i = 0; i < maximum; i++) {
		players[i]->deactive();
	}
	play->emergentStop();
	ACE_Reactor::instance()->end_reactor_event_loop();
}

void Director::releaseAllplayer(){
	for (int i = 0; i < maximum; i++) {
		players[i]->reset();
	}
}

void Director::clearAllLines(){
	for (int i = 0; i < maximum; i++) {
		players[i]->clearLines();
	}
}