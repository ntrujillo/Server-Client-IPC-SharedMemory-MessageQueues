#ifndef _SHMreqchannel_H_
#define _SHMreqchannel_H_

#include "common.h"
#include "RequestChannel.h"
#include "SHMQueue.h"

class SHMRequestChannel : public RequestChannel {
    public:
        SHMRequestChannel(const string _name, const Side _side, int _len);
        ~SHMRequestChannel();
        int cread (void* msgbuf, int bufcapacity);
        int cwrite (void* msgbuf, int bufcapacity);
        int len;

        SHMQueue wfd;
	    SHMQueue rfd;
};

#endif