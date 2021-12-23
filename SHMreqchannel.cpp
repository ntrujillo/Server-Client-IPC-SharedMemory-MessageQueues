#include "SHMreqchannel.h"

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _len) : RequestChannel(_name, _side), len(_len){
	s1 = "/shm_" + my_name + "_1";
	s2 = "/shm_" + my_name + "_2";

    if (_side == SERVER_SIDE){
		wfd = SHMQueue(s1, len);
		rfd = SHMQueue(s2, len);
	}
	else{
		wfd = SHMQueue(s2, len);
		rfd = SHMQueue(s1, len);
	}
}

SHMRequestChannel::~SHMRequestChannel() {
	close(wfd.fd);
	close(rfd.fd);

    shm_unlink(s1.c_str());
	shm_unlink(s2.c_str());

	sem_close(wfd.readerdone);
	sem_close(wfd.writerdone);
	sem_close(rfd.readerdone);
	sem_close(rfd.writerdone);

	sem_unlink((char*)(s1 + "_recv").c_str());
	sem_unlink((char*)(s1 + "_send").c_str());
	sem_unlink((char*)(s2 + "_recv").c_str());
	sem_unlink((char*)(s2 + "_send").c_str());

	munmap(wfd.shmbuffer, len);
	munmap(rfd.shmbuffer, len);
}

int SHMRequestChannel::cread(void* msgbuf, int bufcapacity) {
	/* 1. Wait for the writer side until data is available */
	sem_wait (rfd.writerdone);

	/* 2. copy from the shmbuffer to msg to recieve the data */
	memcpy(msgbuf, rfd.shmbuffer, bufcapacity);

	/* 3. Notify the writer that reading is done */
	sem_post(rfd.readerdone);

	return bufcapacity;

}

int SHMRequestChannel::cwrite(void* msgbuf, int bufcapacity) {
	/* 1. wait for the reader side to make sure that it is done
	first. This syn will happen using Kernel Semaphore */
	sem_wait(wfd.readerdone);

	/* 2. Now write the data */
	memcpy(wfd.shmbuffer, msgbuf, bufcapacity);

	/* 3. Notify the reader that the new data is available */
	sem_post(wfd.writerdone);

	return bufcapacity;
}