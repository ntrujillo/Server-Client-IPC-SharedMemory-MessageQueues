#include "common.h"
#include "RequestChannel.h"
using namespace std;

RequestChannel::RequestChannel (const string _name, const Side _side) : my_name(_name), my_side(_side) {}

RequestChannel::~RequestChannel() {}

int RequestChannel::cread (void* msgbuf, int bufcapacity) {return -1;}

int RequestChannel::cwrite (void* msgbuf, int bufcapacity) {return -1;}