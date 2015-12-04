#ifndef H_DRT_HEADER
#define H_DRT_HEADER

#define BUFSIZE 1024

#include <iostream>
#include <exception>

#include <vector>
#include <map>
#include <regex>
#include <mutex>

#include <ace/Acceptor.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>


#include "CodeException.h"

using namespace std;

typedef map<int, vector<pair<string, string>>> MENU;
typedef pair<string, string> entry;

class DirectorHandler : public ACE_Svc_Handler < ACE_SOCK_STREAM, ACE_NULL_SYNCH > {
public:
	~DirectorHandler();
	DirectorHandler();

	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
	virtual int open(void* p);

	void setId(int c);
	void setDirectorHandlers(map<int, DirectorHandler*> *sh);
	void setMenu(MENU* m);

private:
	int ID;
	map<int, DirectorHandler*> *directorHandlers;
	MENU* menu;

	static mutex m_handlers;
	static mutex m_menu;

	void freshScreen();
};

#endif