#include "Player.h"

void Player::read(string name, string script)
{
	ifstream infile(script);
	if (!infile.is_open()) {
		string error = "[ERROR]:  Open file fail: " + script;
		throw CodeException(FAIL_FILE_OPEN, error.c_str());
	}
	string currentLine;
	//Using regular expression to check the data for each line
	regex re("^\\s*(\\d+)\\s*([^\\d\\s].*?)\\s*$");
	while (getline(infile, currentLine)) {
		smatch sm;
		regex_match(currentLine, sm, re);
		if (sm.size() > 0) {
			//Once the line match the RE, come in this if condition and insert data
			lines[stoi(sm[1])] = SingleLine(name, sm[2]);
		}
	}

	infile.close();

}

void Player::act()
{
	while (isActive) {
		if (!retrieving) {
			// acting
			if (!lines.empty()) {
				if (iter != lines.end()) {
					waitingRecite = true;
					play.recite(iter, sceneCount);
					waitingRecite = false;
					iter++;
				}
				else {
					exit();
					retrieving = true;
				}
			}
			
			// no job anymore
			if (lines.empty() && play.checkPlayFinished()) {
				deactive();
			}

			if (play.checkStopFlag()){
				busy = false;
			}
		}
		// else is waiting
	}
}

void Player::enter(int sceneCounter, string name, string script)
{
	busy = true;
	retrieving = true;
	sceneCount = sceneCounter;
	try {
		read(name, script);
	}
	catch (CodeException& e) {
		throw e;
	}
	iter = lines.begin();
	play.enter(sceneCounter);
	if (play.checkStopFlag()){
		exit();
	}
	else{
		retrieving = false;
	}
}

void Player::exit()
{
	busy = false;
	lines.clear();
	play.exit();
}

bool Player::isbusy()
{
	return busy;
}

void Player::activate()
{
	if (!isActive) {
		isActive = true;
		this->t = thread(&Player::act, this);
	}
}

void Player::deactive()
{
	if (isActive) {
		isActive = false;
	}
}

void Player::join()
{
	if (t.joinable()) {
		t.join();
	}
}

void Player::reset(){
	busy = false;
	retrieving = true;

	//## new added
	waitingRecite = true;
}

void Player::clearLines(){
	lines.clear();
}


//##new added
bool Player::checkWaitingRecite(){
	return waitingRecite;
}
bool Player::checkRetrieving(){
	return retrieving;
}
int Player::getCurrentLineCounter(){
	return iter->first;
}