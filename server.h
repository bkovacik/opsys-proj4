#include <cstdint>
#include <string>

#define PORT 8765
#define CLIENTS 5
#define BUFFERSIZE 1024

enum errs {NOERR, FILEEX, NOFILE, BYTER, MISC};

class Server {
	std::string direct;

	errs storef(std::string name, uint32_t bytes, std::string contents);
	errs readf(std::string name, uint32_t byte_off, uint32_t length);
	errs deletef(std::string name);
	std::string dir();

  public:
	Server(std::string direct) {
		this->direct = direct;
	}
	//returns true on success
	bool parseCommand(std::string command);
	void run();
};
