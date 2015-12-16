#include <iostream>

#include "disk.h"

int main(int argc, char* argv[]) {
    // set up a disk system
    Disk d;
    
    // add and remove files, and print the results to test simulation
    d.allocFile("abc.txt", 25842);
    d.allocFile("def.txt", 86014);
    d.allocFile("ghi.txt", 32765);
    d.allocFile("jkl.txt", 36860);
    d.allocFile("mno.txt", 204795);
    d.allocFile("pqr.txt", 69629);
    d.allocFile("stu.txt", 61337);

    d.deallocFile("jkl.txt");
    d.deallocFile("pqr.txt");

    d.allocFile("jkl.txt", 79112);
}
