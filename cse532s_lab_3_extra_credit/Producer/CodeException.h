#ifndef H_COEXCPT
#define H_COEXCPT

#include <exception>

using namespace std;
enum {
	SUCCESS,
	TOO_MUCH_PARAMETERS,
	SVC_HANDLER_OPEN_FAILED,
	UNKNOWN_ERROR,
	SEND_MSG_ERROR
};

class CodeException : public exception {
public:
	CodeException() : err(UNKNOWN_ERROR) {}
	CodeException(int err_, const char* msg) : err(err_), exception(msg) {}
	CodeException(int err_, exception& e) : err(err_), exception(e) {}
	int errCode() const { return err; }
private:
	// store the error code
	int err;
};

#endif
