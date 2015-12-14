#include "server.h"

errs Server::storef(std::string name, uint32_t bytes, std::string contents) {
    return NOERR;
}

errs Server::readf(std::string name, uint32_t byte_off, uint32_t length) {
    return NOERR;
}

errs Server::deletef(std::string name) {
    return NOERR;
}

void Server::dir() {

}

//returns true on success
bool parseCommand(std::string command) {
    return true;
}
