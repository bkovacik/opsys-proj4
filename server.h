#include <cstdint>

enum errs = {NOERR, FILEEX, NOFILE};

class Server {
	errs storef(std::string name, uint_32 bytes, std::string contents);
	errs readf(std::string name, uint_32 byte_off, uint_32 length);
	errs deletef(std::string name);
	errs dir();

  public:
	//returns true on success
	bool parseCommand(std::string command);
}
