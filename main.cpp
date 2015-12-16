#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <iostream>

#include "server.h"

#define STORAGE ".storage"

using namespace std;

int main (int argc, char* argv[]) {
	struct stat st;
	if (!stat(STORAGE, &st)) {
		cout << "Is it ok to clean " << STORAGE << "? ";
		string input;
		getline(cin, input);
		if (input[0] == 'y' || input[0] == 'Y') {
			DIR* dir;
			dir = opendir(STORAGE);

			dirent* ent;
			while ((ent = readdir(dir)) != NULL) {
				string file(STORAGE);
				file = file + '/' + ent->d_name;
				if (ent->d_name[0] != '.')
					remove(file.c_str());
#ifdef DEBUG
if (ent->d_name[0] != '.')
	perror(file.c_str());
#endif
			}

			cout << "Dir cleaned.\n";
		}
		else {
			cout << "Ok, exiting...\n";
		Server server(STORAGE);
		server.run();
			return 0;
		}
	
	}

	return 0;
}
