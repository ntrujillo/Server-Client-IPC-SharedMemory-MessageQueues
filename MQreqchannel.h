#ifndef _MQreqchannel_H_
#define _MQreqchannel_H_

#include "RequestChannel.h"
#include "common.h"
#include <string>
#include <iostream>
#include <mqueue.h>
#include <fcntl.h>
using namespace std;

class MQRequestChannel : public RequestChannel {
    public:
        MQRequestChannel(const string _name, const Side _side, int _len);
        ~MQRequestChannel();
        int cread (void* msgbuf, int bufcapacity);
        int cwrite (void* msgbuf, int bufcapacity);
        int len;
        int wfd;
	    int rfd;
};

#endif