#include "DirectorConnector.h"


DirectorConnector::~DirectorConnector()
{
	cout << "--->Director connector destructed!" << endl;
}

int DirectorConnector::open(){
	if (aceConnector::open() == -1){
		cerr << "--->ACE connector open failed!" << endl;
		return -1;
	}

	ACE_Sig_Set sigSet;
	sigSet.sig_add(SIGINT);
	ACE_Reactor::instance()->register_handler(sigSet, this);

	return SUCCESS;
}

int DirectorConnector::handle_signal(int signum, siginfo_t*, ucontext_t*){
	
	if (signum == SIGINT){
		cout << "--->Signal detected! Program terminated!" << endl;
		ACE_Reactor::instance()->end_reactor_event_loop();
		return -1;
	}

	return SUCCESS;
}
