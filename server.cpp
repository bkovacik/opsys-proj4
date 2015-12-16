#include "server.h"

#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <algorithm>

errs Server::storef(std::string name, uint32_t bytes, std::string contents) {
	struct stat st;

	if (!stat(direct.c_str(), &st))
		return FILEEX;
	else {
        if (simulatedStorage.allocFile(name, bytes) < 0) {
            return FILEEX;  // file exists already in simulated storage
        }
	}
	return NOERR;
}

errs Server::readf(std::string name, uint32_t byte_off, uint32_t length) {
	struct stat st;

	if (stat(direct.c_str(), &st))
		return NOFILE;
	else {
		if (length < 0 || byte_off + length > (uint32_t) st.st_size)
			return BYTER;
	}
	return NOERR;
}

errs Server::deletef(std::string name) {
	struct stat st;

	if (stat(direct.c_str(), &st))
		return NOFILE;
	else
		remove(name.c_str());

    if (simulatedStorage.deallocFile(name) < 0) {
        return NOFILE;
    }

	return NOERR;
}

std::string Server::dir() {
	DIR* dir;
	dir = opendir(direct.c_str());

	dirent* ent;
	std::vector<std::string> names;
	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.')
			names.push_back(ent->d_name);
	}

	sort(names.begin(), names.end());

	std::string name(std::to_string(names.size()));
	for (unsigned int i = 0; i < names.size(); i++) {
		name += "\n";
		name += names[i];
	}
	name += "\n";

	return name;
}

void* parseCommand(void* argv) {
	args* arga = (args*) argv;
	char buffer[BUFFERSIZE];
	int n;
	while ((n = recv(arga->socket, buffer, BUFFERSIZE, 0))) {
		buffer[n] = '\0';
		printf("%s", buffer);
		fflush(0);

		std::string result;
		std::string command(buffer);

		if (command.substr(0, 5) == "STORE") {
            int start = command.find(' ');
            int end = command.substr(6).find(' ');

            std::string filename = command.substr(6, end);

            start = end + 1;
            end = command.substr(end).find('\n');

            int bytes = atoi(command.substr(start, end).c_str());

            std::string data = command.substr(end);

            if (arga->server->storef(filename, bytes, data)) {
                return NULL;
            }
		}
		else if (command.substr(0, 4) == "READ") {

		}
		else if (command.substr(0, 6) == "DELETE") {
			if (arga->server->deletef(command.substr(6)))
				return NULL;
		}
		else if (command.substr(0, 3) == "DIR")
			result = arga->server->dir();
		
	}

	return NULL;
}

void Server::run() {
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	server.sin_port = htons(PORT);
	int len = sizeof(server);

	bind(sock, (struct sockaddr*) &server, len);
	listen(sock, CLIENTS);

	struct sockaddr_in client;
	int fromlen = sizeof(client);
	while (1) {
		int newsock = accept(sock, (struct sockaddr*) &client, (socklen_t*) &fromlen);
		pthread_t t;

		struct args argv;
		argv.socket = newsock;
		argv.server = this;

		pthread_create(&t, NULL, parseCommand, static_cast<void*>(&argv));
	}

}
