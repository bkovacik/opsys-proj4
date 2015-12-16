/**
 * Authors: Ian Chamberlain and Brian Kovacik
 */
#include "server.h"

#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <netdb.h>

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

std::string Server::storef(std::string name, uint32_t bytes, std::string contents) {
	struct stat st;

    std::stringstream path;
    path << direct << "/" << name;

	if (!stat(path.str().c_str(), &st))
		return errout(FILEEX);

    if (simulatedStorage.allocFile(name, bytes) < 0) {
            return errout(FILEEX);  // file exists already in simulated storage
    }

    std::ofstream outfile (path.str().c_str());
    outfile << contents;
    outfile.close();
	return errout(NOERR);
}

std::string Server::readf(std::string name, uint32_t byte_off, uint32_t length) {
	struct stat st;

    std::stringstream path;
    path << direct << "/" << name;

	if (stat(path.str().c_str(), &st))
		return errout(NOFILE);

    if (length < 0 || byte_off + length > (uint32_t) st.st_size)
        return errout(BYTER);


    char res[length + 1];

    std::ifstream infile (path.str().c_str());
    infile.seekg(byte_off, infile.beg);
    infile.read(res, length);

    res[length] = '\0';

    std::stringstream result;
    result << "ACK " << length << "\n" << res;

	return result.str();
}

std::string Server::deletef(std::string name) {
	struct stat st;

	std::string path(direct);
	path = path + '/' + name.substr(0, name.length()-1);

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

std::string Server::errout(errs err) {
	switch (err) {
		case NOERR: return "ACK\n";
		case FILEEX: return "ERROR: FILE EXISTS\n";
		case NOFILE: return "ERROR: NO SUCH FILE\n";
		case BYTER: return "ERROR: INVALID BYTE RANGE\n";
		case MISC: break;
	}
	return "";
}

void* parseCommand(void* argv) {
	args* arga = (args*) argv;
	char buffer[BUFFERSIZE];
	int n;
	while ((n = recv(arga->socket, buffer, BUFFERSIZE, 0))) {
		buffer[n] = '\0';
		std::cout << "[thread " << (unsigned int) pthread_self() << "] Rcvd: " << buffer;

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

		    result = (arga->server->storef(filename, bytes, std::string(data)));
		}
		else if (command.substr(0, 4) == "READ") {
		    int start = command.find(' ');
		    int end = command.substr(5).find(' ') + 5;

		    std::string filename = command.substr(5, end - 5);

		    start = end + 1;
		    end = command.substr(start).find(' ') + start;

		    int offset = atoi(command.substr(start, end - start).c_str());

		    start = end + 1;
		    end = command.substr(start).find('\n') + start;

		    int bytes = atoi(command.substr(start, end - start).c_str());

		     result = arga->server->readf(filename, offset, bytes);
		}
		else if (command.substr(0, 6) == "DELETE")
			result = arga->server->deletef(command.substr(7));
		else if (command.substr(0, 3) == "DIR")
			result = arga->server->dir();

		std::cout << "[thread " << (unsigned int) pthread_self() << "] Sent: " << result.c_str() << std::endl;
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

	std::cout << "Block size is " << simulatedStorage.getBlocksize() << std::endl;
	std::cout << "Number of blocks is " << simulatedStorage.getN_blocks() << std::endl;
	std::cout << "Listening on port " << PORT << std::endl;

	while (1) {
		int newsock = accept(sock, (struct sockaddr*) &client, (socklen_t*) &fromlen);
		char hostname[BUFFERSIZE], sbuf[BUFFERSIZE];
		getnameinfo((struct sockaddr*) &client, (socklen_t) fromlen, hostname, sizeof(hostname), sbuf, sizeof(sbuf), 0);
		std::cout << "Received incoming connection from " << hostname << std::endl;
		pthread_t t;

		struct args argv;
		argv.socket = newsock;
		argv.server = this;

		pthread_create(&t, NULL, parseCommand, static_cast<void*>(&argv));
	}

}
