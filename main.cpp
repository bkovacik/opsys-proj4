/**
 * Authors: Ian Chamberlain and Brian Kovacik
 */
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
	if (stat(STORAGE, &st)) {
        if (mkdir(STORAGE, 0666) < 0) {
            perror(NULL);
        }
    }
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

    Server server(STORAGE);
    server.run();
    return 0;
}
