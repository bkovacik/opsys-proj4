/**
 * Authors: Ian Chamberlain and Brian Kovacik
 */
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

    std::cout << "[thread " << pthread_self() << "] Simulated Disk Space Allocation:" << std::endl;
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
int Disk::allocFile(const std::string& name, int num_bytes) {

    std::map<std::string, int>::iterator fileItr = fileMap.find(name);
    if (fileItr != fileMap.end()) {
        // file already exists, return with error
        return -1;
    }

    // allocate space as necessary
    int num_blocks = num_bytes / blocksize;
    if (num_bytes % blocksize != 0) {
        num_blocks++;
    }

    std::map<int, DiskCluster>::iterator itr = diskSpace.begin();
    int first_cluster = -1;
    int last_cluster = -1;
    int num_clusters = 0;

    while (itr != diskSpace.end()) {
        if(itr->second.getDisplayChar() == '.' && num_blocks > 0) {
            // if the remainder of the file fits, split the cluster
            if (itr->second.getSize() >= num_blocks) {
#ifdef DEBUG_MODE
                std::cout << "found open cluster of size " << itr->second.getSize() << std::endl;
#endif
                int next_size = itr->second.getSize() - num_blocks;

                DiskCluster newAlloc (num_blocks, name, nextFileChar);
                diskSpace[itr->first] = newAlloc;

                DiskCluster nextAlloc (next_size, "", '.');
                diskSpace[itr->first + num_blocks] = nextAlloc;

                num_blocks = 0;
            }
            // otherwise, use this cluster and keep looking for more space
            else {
                DiskCluster newAlloc (itr->second.getSize(), name, nextFileChar);
                diskSpace[itr->first] = newAlloc;

                num_blocks -= itr->second.getSize();
            }

            num_clusters++;

            if (first_cluster < 0) {
                first_cluster = itr->first;
                fileMap[name] = itr->first;
            }
            if (last_cluster > 0) {
                diskSpace[last_cluster].setNextCluster(itr->first);
            }
            last_cluster = itr->first;
        }
        itr++;
    }

    // advance the character if we successfully allocated
    if (first_cluster >= 0) {
        int total_blocks = (num_bytes / blocksize);
        if (num_bytes % blocksize != 0)
            total_blocks++;

        std::cout << "[thread " << pthread_self() << "] Stored file '";
        std::cout << nextFileChar;

        if (num_bytes == 1)
            std::cout << "' (" << num_bytes << " byte; ";
        else
            std::cout << "' (" << num_bytes << " bytes; ";

        if (total_blocks == 1)
            std::cout << total_blocks << " block; ";
        else
            std::cout << total_blocks << " blocks; ";
        
        if (num_clusters == 1)
            std::cout << num_clusters << " cluster)" << std::endl;
        else
            std::cout << num_clusters << " clusters)" << std::endl;

        nextFileChar++;
        printSpace();
    }
    else {
        // TODO: print an error message or something?
    }

    return first_cluster;
}

// deallocFile - deallocates disk space for a file and deletes
int Disk::deallocFile(const std::string& name) {

    if (fileMap.find(name) == fileMap.end()) {
        return -1;  // error noFile
    }

    int index = fileMap.find(name)->second;

    std::map<int, DiskCluster>::iterator itr = diskSpace.find(index);

    if (itr == diskSpace.end()) {
        // this probably shouldn't ever occur
        return -1;  // error noFile
    }

    char display = itr->second.getDisplayChar();
    int blocks = 0;

    // deallocate all of the blocks
    while (itr != diskSpace.end()) {
        index = itr->second.getNextCluster();

        int thisClusterSize = itr->second.getSize();
        blocks += itr->second.getSize();

        // check prev and next chunks as well
        std::map<int, DiskCluster>::iterator tmpItr = itr;
        tmpItr++;
        if (tmpItr->second.getDisplayChar() == '.') {
            blocks += tmpItr->second.getSize();
            thisClusterSize += tmpItr->second.getSize();
            diskSpace.erase(tmpItr);
        }
        tmpItr = itr;
        tmpItr--;

        if (tmpItr->second.getDisplayChar() == '.') {
            blocks += tmpItr->second.getSize();
            thisClusterSize += tmpItr->second.getSize();
            DiskCluster tmp (thisClusterSize, "", '.');
            diskSpace[tmpItr->first] = tmp;
        }
        else {
            DiskCluster tmp (thisClusterSize, "", '.');
            diskSpace[itr->first] = tmp;
        }
        itr = diskSpace.find(index);
    }

    // remove the file mapping as well
    fileMap.erase(name);

    std::cout << "[thread " << pthread_self() << "] Deleted " << name << " file ";
    std::cout << "'" << display << "' (deallocated ";
    if (blocks == 1)
        std::cout << blocks << " block)" << std::endl;
    else
        std::cout << blocks << " blocks)" << std::endl;

    printSpace();

    return 0;
}

// we need a default constructor to use with std::map
DiskCluster::DiskCluster() {
    size = 0;
    name = "";
    displayChar = '?';
    nextCluster = -1;
}

// DiskCluster(int, std::string, char) - create a disk cluster with size s, name and char
DiskCluster::DiskCluster(int s, const std::string& n, char d) {
    size = s;
    name = n;
    displayChar = d;
    nextCluster = -1;
}
