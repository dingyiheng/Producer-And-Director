#include "DirectorHandler.h"

DirectorHandler::~DirectorHandler(){
	cout << "--->Service Handler Destructor" << endl;
	unique_lock<mutex> lock(m_handlers);
	directorHandlers->erase(ID);
	cout << "--->Current Svc Handlers Number: " << directorHandlers->size() << endl;
	lock.unlock();


	unique_lock<mutex> lock2(m_menu);
	menu->erase(ID);
	cout << "--->Current Menu Size: " << menu->size() << endl;
	lock2.unlock();

	lock_guard<mutex> lock3(m_menu);
	freshScreen();
}

DirectorHandler::DirectorHandler(){
	ID = 0;
}

void DirectorHandler::freshScreen(){
	cout << string(50, '\n') << string(50, '\031') << endl;
	//fresh the screen
	int counting = 0;
	for (auto i1 : (*menu)){
		for (auto i2 : i1.second){
			counting++;
			cout << counting << ". " << i2.first << " : " << i2.second << endl;
		}
	}
	cout << string(50, '\030') << endl;
}


int DirectorHandler::handle_input(ACE_HANDLE h){
	char buf[BUFSIZ] = "";
	if (peer().recv(buf, BUFSIZ) == -1){
		cout << "Director :" << ID << " disconnected from producer" << endl;
		return -1;
	}

	if (strcmp(buf, "suicide") == 0){
		return -1;
	}

	if (strcmp(buf, "") != 0){
		string s = buf;

		std::regex words_regex("\\[([\\w\\d\\s_]*?)\\]\\s<([\\w\\s]*)>");
		auto words_begin =
			std::sregex_iterator(s.begin(), s.end(), words_regex);
		auto words_end = std::sregex_iterator();


		lock_guard<mutex> lock(m_menu);
		(*menu)[ID].clear();
		for (sregex_iterator i = words_begin; i != words_end; ++i) {
			smatch match = *i;
			string match_str = match[1].str();
			string match_str_2 = match[2].str();
			(*menu)[ID].push_back(entry(match_str, match_str_2));
		}

		freshScreen();
	}

	return SUCCESS;
}

int DirectorHandler::open(void* p){
	if (ACE_Svc_Handler < ACE_SOCK_STREAM, ACE_NULL_SYNCH >::open(p) == -1){
		cerr << "Ace svc handler open failed!" << endl;
		return -1;
	}

	int i = 0;
	cout << "Director Handler : " << ID << " Established" << endl;
	lock_guard<mutex> lock(m_handlers);
	(*directorHandlers)[ID] = this;
	return SUCCESS;
}

void DirectorHandler::setId(int c){
	ID = c;
}

void DirectorHandler::setDirectorHandlers(map<int, DirectorHandler*> *sh){
	directorHandlers = sh;
}

void DirectorHandler::setMenu(MENU* m){
	menu = m;
}