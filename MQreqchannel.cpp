#include "common.h"
#include "MQreqchannel.h"
#include <mqueue.h>
#include <fcntl.h>

MQRequestChannel::MQRequestChannel(const string _name, const Side _side, int _len) : RequestChannel(_name, _side), len(_len){

    s1 = "/mq_" + my_name + "_1";
	s2 = "/mq_" + my_name + "_2";

    struct mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_msgsize = len;
    attr.mq_maxmsg = 1;
    attr.mq_flags = 0;

    if (_side == SERVER_SIDE){
		wfd = (int) mq_open(s1.c_str(), O_RDWR | O_CREAT, 0600, &attr);
		rfd = (int) mq_open(s2.c_str(), O_RDWR | O_CREAT, 0600, &attr); 
	}
	else{
		wfd = (int) mq_open(s2.c_str(), O_RDWR | O_CREAT, 0600, &attr);
		rfd = (int) mq_open(s1.c_str(), O_RDWR | O_CREAT, 0600, &attr); 
	}
}

MQRequestChannel::~MQRequestChannel() {
    mq_close(wfd);
    mq_close(rfd);

    mq_unlink(s1.c_str());
    mq_unlink(s2.c_str());
}

int MQRequestChannel::cread(void* msgbuf, int bufcapacity) {
    return mq_receive(rfd, (char*) msgbuf, 8192, NULL);
}

int MQRequestChannel::cwrite(void* msgbuf, int bufcapacity) {
    return mq_send(wfd, (char*) msgbuf, bufcapacity, 0);
}
