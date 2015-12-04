#include "Producer.h"

Producer::~Producer(){
	cout << "Producer destructor" << endl;
}

Producer::Producer(){ 
	Counter = 0;
	UIloop = true;
	shareFut = async(launch::async, &Producer::UI, this);
}

int Producer::make_svc_handler(DirectorHandler *&sh){
	sh = new DirectorHandler();
	sh->reactor(this->reactor());
	Counter++;
	sh->setId(Counter);
	sh->setDirectorHandlers(&directorHandlers);
	sh->setMenu(&menu);

	return SUCCESS;
}

bool Producer::UI(){
	string command = "";

	while (UIloop){
		getline(cin, command);
		regex re("^([\\w]+)(\\s([\\d]+))*$");
		smatch sm;
		regex_match(command, sm, re);
		if (sm[1] == "quit"){
			char command[] = "quit";
			for (auto director : directorHandlers){
				if (director.second->peer().send_n(command, sizeof(command)) == -1){
					ACE_Reactor::instance()->end_reactor_event_loop();
					string error = "Sending command to director : " + to_string(director.first) + "  error";
					throw CodeException(SEND_MSG_ERROR, error.c_str());
				}
			}

			ACE_Reactor::instance()->end_reactor_event_loop();
			UIloop = false;
		}
		else if ((sm[1] == "start" || sm[1] == "stop" )&& sm[3].matched == true){
			//index from 1
			int i = stoi(sm[3]);
			string prefix = sm[1];
			int counting = 0;
			for (auto i1 : menu){
				for (auto i2 : i1.second){
					counting++;
					if (counting == i){
						if ((i2.second == "available" && prefix=="start")||
							(i2.second == "in progress" && prefix == "stop")){
							string name = i2.first;
							string command = prefix + " " + name;
							if (directorHandlers[i1.first]->peer().send_n(command.c_str(), command.length() + 1) == -1){
								ACE_Reactor::instance()->end_reactor_event_loop();
								string error = "Sending command to director : " + to_string(i1.first) + "  error";
								throw CodeException(SEND_MSG_ERROR, error.c_str());
							}
						}
						else{
							cout << "Invalid command" << endl;
						}
						
						break;
					}
				}
			}
		}
	}

	return true;
}

void Producer::getFut(){
	shareFut.get();
}

