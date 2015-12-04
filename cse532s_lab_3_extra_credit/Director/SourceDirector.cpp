#include "DirectorConnector.h"

#include <ace/ace_wchar.h>
#include <ace/OS.h>

#include <string>
#include <sstream>

using namespace std;


int ACE_TMAIN(int argc, ACE_TCHAR* argv[]){
	if (argc < 5){
		cout << "usage: " << argv[0] << "<port> <ip_address> <min_threads> <script_file>+" << endl;
		return FAIL_WRONG_ARGUMENTS;
	}
	
	unsigned int minimum = 0;
	if (argc >= 3) {
		istringstream in(argv[3]);
		// check for whether the third argument is integer or not
		if (!(in >> minimum && in.eof())) {
			cerr << "[ERROR]:  <optional: minimum_player> :'" << argv[3] << "' must be a integer!" << endl;
			return FAIL_WRONG_ARGUMENTS;
		}
	}

	vector<string> playScriptFiles;
	for (int i = 4; i < argc; i++){
		string temp = argv[i];
		playScriptFiles.push_back(temp);
	}	

	string port = argv[1];
	string ip = argv[2];
	string addr = ip + ":" + port;
	cout << addr << endl;

	ACE_INET_Addr address(addr.c_str());
	DirectorConnector connector;
	Director director;
	try {
		director.init(playScriptFiles, 0);

		Director* ptr = &director;
		if (connector.connect(ptr, address) == -1){
			cerr << "--->Connection Failed" << endl;
			director.emergencyStop();
			return CONNECTION_FAIL;
		}

		ACE_Reactor::instance()->run_reactor_event_loop();
		ACE_Reactor::instance()->close();
	}
	catch (CodeException& error) {
		cerr << error.what() << endl;
		return error.errCode();
	}

	return SUCCESS;
}