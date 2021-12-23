#ifndef _RequestChannel_H_
#define _RequestChannel_H_

#include "common.h"
#include <string>
using namespace std;


class RequestChannel {
    public:
        enum Side {SERVER_SIDE, CLIENT_SIDE};
        enum Mode {READ_MODE, WRITE_MODE};
        string my_name;
        Side my_side;
        string s1, s2;

    public:
        /* CONSTRUCTOR/DESTRUCTOR */
        RequestChannel (const string _name, const Side _side);

        virtual ~RequestChannel() = 0;
        /* destruct operation should be derived class-specific */

        virtual int cread (void* msgbuf, int bufcapacity) = 0;
        /* Blocking read; returns the number of bytes read.
        If the read fails, it returns -1. */

        virtual int cwrite (void* msgbuf, int bufcapacity) = 0;
        /* Write the data to the channel. The function returns
        the number of characters written, or -1 when it fails */
};

#endif
