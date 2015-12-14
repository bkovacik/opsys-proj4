#include <cstdint>
#include <string>

enum errs {NOERR, FILEEX, NOFILE};

class Server {
	errs storef(std::string name, uint32_t bytes, std::string contents);
	errs readf(std::string name, uint32_t byte_off, uint32_t length);
	errs deletef(std::string name);
	void dir();

  public:
	//returns true on success
	bool parseCommand(std::string command);
};
