#ifndef _SHMQueue_H_
#define _SHMQueue_H_

#include "common.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/mman.h>

class SHMQueue {
    public:
        char* shmbuffer;
        sem_t* readerdone;
        sem_t* writerdone;
        int fd;

        // default const
        SHMQueue();

        // constructor
        SHMQueue (string name, int len);
};

#endif