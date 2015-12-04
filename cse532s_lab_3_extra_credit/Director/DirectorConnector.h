#ifndef H_DRT_CONNECT
#define H_DRT_CONNECT

#include "Director.h"

#include <ace/Connector.h>
#include <ace/Signal.h>

typedef ACE_Connector<Director, ACE_SOCK_CONNECTOR> aceConnector;

class DirectorConnector : public  aceConnector
{
public:
	~DirectorConnector();

	virtual int open();

	virtual int handle_signal(int signum, siginfo_t* = 0, ucontext_t* = 0);
};

#endif
