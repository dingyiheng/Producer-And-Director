#include "Play.h"

void Play::recite(LineIter & iter, int sceneCount)
{
	unique_lock<mutex> lock(m);
	bool largelineCounter = false; 

	cv.wait(lock, [&] {
		if (stopflag) {
			return true;
		}

		if (sceneCount == scene_fragment_counter && iter->first == line_counter) {
			return true;
		}
		else if (scene_fragment_counter < sceneCount || (sceneCount == scene_fragment_counter && iter->first > line_counter)) {
			return false;
		}
		else {
			//to be finished
			cerr << "[ERROR]:  scene already passed or line already passed" << endl;
			cerr << "[ERROR INFO]: " << scene_fragment_counter << " " << sceneCount << " " << iter->first << " " << line_counter << endl;
			cv.notify_all();
			largelineCounter = true;
			return true;
		}
	});

	if (!stopflag && !largelineCounter) {
		// emergency stop will notify this method to continue, and jump out out loop
		if (firstLine) {
			currentCharacter = iter->second.first;
			cout << iter->second.first << "." << endl;
			firstLine = false;
		}
		else {
			if (currentCharacter != iter->second.first) {
				cout << endl << iter->second.first << "." << endl;
				currentCharacter = iter->second.first;
			}
		}

		cout << iter->second.second << endl;

		line_counter++;
		//The notify_all can be done after unlock
		lock.unlock();
		cv.notify_all();
	}
}

void Play::enter(int sceneCount)
{
	if (sceneCount < scene_fragment_counter) {
		string error = "[ERROR]: scene counter less than current counter";
		throw CodeException(SCENE_COUNTER_ERROR, error.c_str());
	}
	else if (sceneCount == scene_fragment_counter) {
		on_stage++;
	}
	else {
		//different condition variable and different mutex
		unique_lock<mutex> lock(m_enter);
		// some one is not ont the stage but keep waiting, so not available
		waiting++;
		cv_enter.wait(lock, [&] {
			return (sceneCount == scene_fragment_counter || stopflag);
		});
		waiting--;
		on_stage++;
	}
}

void Play::exit()
{
	if (on_stage > 1) {
		on_stage--;
		cv_cue.notify_all();
	}
	else if (on_stage < 1) {
		throw exception("[ERROR]: lower bound of on stage exceed");
	}
	else {
		on_stage--;
		// if someone exit the stage, he can be cued, so notify
		cv_cue.notify_all();
		resetCCandFL();
		scene_fragment_counter++;
		cv_current_show_end.notify_all();
		line_counter = 1;
		if (sceneNameIter != sceneRef.end()) {
			if (!sceneNameIter->empty() && !stopflag) {
				cout << endl << *sceneNameIter << endl;
			}
			sceneNameIter++;
		}
		cv_enter.notify_all();
	}
}

void Play::checkAvailable(int max)
// check whether there is any available player
{
	unique_lock<mutex> lock(m_check);
	cv_cue.wait(lock, [&] {
		return ((on_stage + waiting) < max || stopflag);
	});
}

bool Play::checkPlayFinished()
// ACT cookie check, whether the scripts are all dispatched
{
	return finishFlag._Is_ready();
}

void Play::emergentStop()
{
	stopflag = true;
	// in case of some threads stuck with recite function, need to notify them all.
	cv.notify_all();
}

void Play::setFlag(bool flag){
	stopflag = flag;
}

void Play::resetCCandFL()
// reset the firstLine and currentCharacter
{
	firstLine = true;
	currentCharacter = string();
}


void Play::setSceneTitles(const sceneTitles& titles){
	sceneRef = titles;
	sceneNameIter = sceneRef.begin();
	if (!sceneRef.empty()){

		cout << (*sceneNameIter) << endl;
		sceneNameIter++;
	}
}

//please call before start a new play
void Play::resetWholePlay(){
	on_stage = 0;
	waiting = 0;
	line_counter = 1;
	scene_fragment_counter = 1;
	resetCCandFL();
	stopflag = false;
}

void Play::checkCurrentShowFinished(){
	unique_lock<mutex> lock(m_current_show);
	cv_current_show_end.wait(lock, [&]{
		return (scene_fragment_counter > (int)sceneRef.size() || stopflag);
	});
}

void Play::releaseWhenStop(){
	cv_current_show_end.notify_all();
	cv_cue.notify_all();
	cv_enter.notify_all();
}

bool Play::checkStopFlag(){
	return stopflag;
}



//# new added
void Play::setLineCounter(int t){
	lock_guard<mutex> lock(m);
	line_counter = t;
	cv.notify_all();
}