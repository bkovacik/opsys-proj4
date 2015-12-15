#ifndef DISK_H
#define DISK_H

#include <string>
#include <map>
#include <iostream>

/**
 * DiskCluster - a structure to hold a single cluster of a file
 */
class DiskCluster {
    int size;
    std::string name;
    char displayChar;
    int nextCluster;

public:
    DiskCluster();
    DiskCluster(int s, const std::string& name, char d);

    int getSize() const { return size; }
    char getDisplayChar() const { return displayChar; }
    const std::string& getName() const { return name; }

    void setNextCluster(int n) { nextCluster = n; }
    int getNextCluster() const { return nextCluster; }
};

/**
 *  Disk - a data structure to simulate management of disk space and files.
 *  This (maybe?) also stores files on the server without manually managing disk space.
 */
class Disk {
    int n_blocks;
    int blocksize;

    std::map<int, DiskCluster> diskSpace;
    std::map<std::string, int> fileMap;
    char nextFileChar;

    // print the allocated space
    void printSpace();

public:

    Disk();
    Disk(int n, int size);

    // allocFile - returns the int for the first cluster allocated for a file
    int allocFile(const std::string& name, int num_blocks);

    // deallocFile - deallocates disk space for a file and deletes
    void deallocFile(const std::string& name);
};

#endif