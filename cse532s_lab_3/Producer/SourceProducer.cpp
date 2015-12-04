#include "Producer.h"

#include <iostream>
#include <ace/ace_wchar.h>
#include <ace/OS.h>

using namespace std;

mutex DirectorHandler::m_handlers;
mutex DirectorHandler::m_menu;

int ACE_TMAIN(int argc, ACE_TCHAR* argv[]){
	if (argc > 2){
		cout << "usage: " << argv[0] << "[port]" << endl;
		return TOO_MUCH_PARAMETERS;
	}

	ACE_INET_Addr address(1234,ACE_LOCALHOST);
	if (argc == 2){
		int port = stoi(argv[1]);
		address = ACE_INET_Addr(port);
	}

	Producer p;
	if (p.open(address) == -1){
		cerr << "Acceptor open failed!" << endl;
		p.close();
		return ACCEPTOR_OPEN_FAILED;
	}
	ACE_Sig_Set sigSet;
	sigSet.sig_add(SIGINT);
	ACE_Reactor::instance()->register_handler(sigSet, &p);


	ACE_Reactor::instance()->run_reactor_event_loop();
	ACE_Reactor::instance()->close();

	try{
		p.getFut();
	}
	catch (CodeException& e){
		cerr << e.what() << endl;
		return e.errCode();
	}

	return SUCCESS;
}