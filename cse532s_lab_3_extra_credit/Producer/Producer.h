#ifndef H_PRODUCER
#define H_PRODUCER

#include "DirectorHandler.h"

#include <future>
#include <string>
#include <regex>
#include <memory>

using namespace std;

class Producer :public ACE_Acceptor<DirectorHandler, ACE_SOCK_ACCEPTOR>{
public:
	~Producer();

	Producer();
	bool UI();
	void getFut();

	virtual int make_svc_handler(DirectorHandler *&sh);

private:
	MENU menu;
	shared_future<bool> shareFut;
	map<int, DirectorHandler*> directorHandlers;

	bool UIloop;
	int Counter;
};

#endif