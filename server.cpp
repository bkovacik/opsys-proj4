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
#include <fstream>
#include <sstream>

std::string Server::storef(std::string name, uint32_t bytes, std::string contents) {
	struct stat st;

	if (stat(direct.c_str(), &st))
		return errout(FILEEX);
	else {
        if (simulatedStorage.allocFile(name, bytes) < 0) {
            	return(FILEEX);  // file exists already in simulated storage
        }

        std::stringstream path;
        path << direct << "/" << name;

        std::ofstream outfile (path.str().c_str());
        outfile << contents;
        outfile.close();
	}
	return errout(NOERR);
}

std::string Server::readf(std::string name, uint32_t byte_off, uint32_t length) {
	struct stat st;

	if (stat(direct.c_str(), &st))
		return errout(NOFILE);
	else {
		if (length < 0 || byte_off + length > (uint32_t) st.st_size)
			return errout(BYTER);
	}
	return errout(NOERR);
}

std::string Server::deletef(std::string name) {
	struct stat st;

	std::string path(direct);
	path = path + '/' + name.substr(0, name.length()-1);

	send(4, "HEY", 3, 0);

	if (stat(path.c_str(), &st))
		return errout(NOFILE);
	else
		remove(path.c_str());

	if (simulatedStorage.deallocFile(name.substr(0, name.length()-1)) < 0) {
#ifdef DEBUG_MODE
        std::cout << "file not found in sim" << std::endl;
#endif
		return errout(NOFILE);
    }
	
	return errout(NOERR);
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

std::string errout(errs err) {
	switch (err) {
		case NOERR: return "ACK\n";
		case FILEEX: return "ERROR: FILE EXISTS\n";
		case NOFILE: return "ERROR: NO SUCH FILE\n";
		case BYTER: return "ERROR: INVALID BYTE RANGE\n";
		case MISC: break;
	}
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
            int end = command.substr(6).find(' ') + 6;

            std::string filename = command.substr(6, end - 6);

            start = end + 1;
            end = command.substr(start).find('\n') + start;
#ifdef DEBUG_MODE
            std::cout << "byte string is '" << command.substr(start, end - start) << "'" << std::endl;
#endif

            int bytes = atoi(command.substr(start, end - start).c_str());

            char data[bytes + 1];
            recv(arga->socket, data, bytes, 0);
            data[bytes] = '\0';

            // print out the data we received
            printf("%s\n", data);

#ifdef DEBUG_MODE
            std::cout << "storing file '" << filename << "' of size " << bytes;
            std::cout << " with data '" << data << "'" << std::endl;
#endif

            if (arga->server->storef(filename, bytes, std::string(data))) {
                return NULL;
            }
		}
		else if (command.substr(0, 4) == "READ") {
		}
		else if (command.substr(0, 6) == "DELETE")
			result = arga->server->deletef(command.substr(7));
		else if (command.substr(0, 3) == "DIR")
			result = arga->server->dir();
	
		send(arga->socket, result.c_str(), result.length(), 0);
		fflush(0);
	
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
