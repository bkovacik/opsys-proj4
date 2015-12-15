#include "disk.h"

Disk::Disk() {
    n_blocks = 128;
    blocksize = 4096;

    DiskCluster tmp(n_blocks, std::string(""), '.'); 
    diskSpace[0] = tmp;

    nextFileChar = 'A';
}

// Disk(int, int) - create a disk system with n blocks and blocksize = size
Disk::Disk(int n, int size) {
    n_blocks = n;
    blocksize = size;

    DiskCluster tmp (n_blocks, "", '.'); 
    diskSpace[0] = tmp;

    nextFileChar = 'A';
}

// printSpace - print the disk system as it is
void Disk::printSpace() {

    std::cout << "[process " << pthread_self() << "] Simulated Disk Space Allocation:" << std::endl;
    std::cout << std::string(n_blocks / 4, '=');

    std::map<int, DiskCluster>::const_iterator itr = diskSpace.begin();

    int j=0;
    while (itr != diskSpace.end()) {
        for (int i=0; i<itr->second.getSize(); i++) {
            if (j % (n_blocks / 4) == 0) {
                std::cout << std::endl;
            }

            std::cout << itr->second.getDisplayChar();
            j++;
        }
        itr++;
    }

    std::cout << std::endl << std::string(n_blocks / 4, '=') << std::endl;
}

// allocFile - returns the int for the first cluster allocated for a file
int Disk::allocFile(const std::string& name, int num_blocks) {
    // do nothing for now except print the disk system

    std::map<std::string, int>::iterator itr = fileMap.find(name);
    if (itr != fileMap.end()) {
        // file already exists, return with error
        return -1;
    }

    // figure out the character to print and allocate space for it
    
    printSpace();

    return 0;
}

// deallocFile - deallocates disk space for a file and deletes
void Disk::deallocFile(const std::string& name) {
    // do nothing for now except print disk system
    printSpace();
}

// we need a default constructor to use with std::map
DiskCluster::DiskCluster() {
    size = 0;
    name = "";
    displayChar = '?';
}

// DiskCluster(int, std::string, char) - create a disk cluster with size s, name and char
DiskCluster::DiskCluster(int s, const std::string& n, char d) {
    size = s;
    name = n;
    displayChar = d;
}
