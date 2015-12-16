#include <cstdint>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

#include "disk.h"

enum errs {NOERR, FILEEX, NOFILE, BYTER, MISC};

class Server {
	std::string direct;
    Disk simulatedStorage;

  public:
	errs storef(std::string name, uint32_t bytes, std::string contents);
	errs readf(std::string name, uint32_t byte_off, uint32_t length);
	errs deletef(std::string name);
	std::string dir();

	Server(std::string direct) {
		this->direct = direct;
	}
	//returns true on success
	bool parseCommand(std::string command);
};
