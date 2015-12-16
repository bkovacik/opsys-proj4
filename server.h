#include <cstdint>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

#include "disk.h"

#define PORT 8765
#define CLIENTS 5
#define BUFFERSIZE 1024

enum errs {NOERR, FILEEX, NOFILE, BYTER, MISC};

class Server;

struct args {
	int socket;
	Server* server;
};

void* parseCommand(void* argv);

class Server {
	friend void* parseCommand(void* argv);

	std::string direct;
	Disk simulatedStorage;

	errs storef(std::string name, uint32_t bytes, std::string contents);
	errs readf(std::string name, uint32_t byte_off, uint32_t length);
	errs deletef(std::string name);
	std::string dir();

  public:
	Server(std::string direct) {
		this->direct = direct;
	}
	void run();
};
