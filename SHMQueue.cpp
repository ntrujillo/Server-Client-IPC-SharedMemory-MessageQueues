#include "SHMQueue.h"

// defualt constructor
SHMQueue::SHMQueue() {}

SHMQueue::SHMQueue (string name, int len) {
    /* 1. Create a SHM segment using the name and connect to it */
    fd = (int)shm_open((char*)name.c_str(), O_RDWR | O_CREAT, 0600); // create
    ftruncate(fd, len); // truncate
    shmbuffer = (char*) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // connect

    /* 2. Create 2 semaphores: readerdone and writerdone */
    readerdone = sem_open((name + "_recv").c_str(), O_CREAT, 0600, 1);
    writerdone = sem_open((name + "_send").c_str(), O_CREAT, 0600, 0);
}