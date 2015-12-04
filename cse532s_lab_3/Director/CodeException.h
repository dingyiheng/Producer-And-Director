#ifndef H_COEXCPT
#define H_COEXCPT

#include <exception>

using namespace std;

//Enumeration for return type of main function
enum {
	SUCCESS,
	FAIL_WRONG_ARGUMENTS,
	FAIL_FILE_OPEN,
	BAD_ALLOCATION,
	CONNECTION_FAIL,
	SENDING_MESSAGE_ERROR,
	UNKNOWN_ERROR,
	SCENE_COUNTER_ERROR
};

class CodeException : public exception {
public:
	CodeException() :err(UNKNOWN_ERROR){}
	CodeException(int err_, const char* msg) : err(err_), exception(msg) {}
	CodeException(int err_, exception& e) : err(err_), exception(e) {}
	int errCode() const { return err; }
private:
	// store the error code
	int err;
};

#endif
