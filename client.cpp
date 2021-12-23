// fix the declarations of each of the channels to be specific not just random!!!!!!

/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
	Original author of the starter code
	
	Please include your name and UIN below
	Name: Nathaniel Trujillo
	UIN: 228005816
 */
#include "common.h"
#include "RequestChannel.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include "SHMreqchannel.h"
#include <fstream> // used for writing to .csv file
#include <cmath> // used in ceil function
#include <iostream> // used for cout statements

// libraries needed for fork
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> // for wait function

// used for file transfer speeds
 #include <sys/time.h>


using namespace std;

// requesting data point(s) function
void DataPointReq(RequestChannel* chan, int p, float t, int e) {
	if (e == 3) { // 1000 data points requested
	timeval tim;
	gettimeofday(&tim, NULL);
	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

	// create datamsg
	datamsg x (p,t,e);

	// initiate a file
	ofstream myFile("x1.csv");
	
	// set e to 1
	x.ecgno = 1; 

	// set time to 0
	t = 0;
	x.seconds = 0; 

	// loop 1000 times
	for(int i = 0; i < 1000; i++) {

		// update datamsg
		x.ecgno = 1;
		x.seconds = t;

		// get ecg1
		chan->cwrite(&x, sizeof(datamsg)); // question
		double reply = 0.0;
		int nbytes = chan->cread(&reply, sizeof(double)); // answer

		// store time, ecg1 & update datamsg
		myFile << t << ",";
		myFile << reply << ",";
		x.ecgno = 2; // update to get ecg2

		// get ecg2
		chan->cwrite(&x, sizeof(datamsg)); // question
		reply = 0.0;
		nbytes = chan->cread(&reply, sizeof(double)); // answer

		// store ecg2
		myFile << reply << endl;

		// increment time parameter
		t += 0.004;

		// repeat
	}

	// close file
	myFile.close();

	cout << "File x1.csv successfully created with 1000 datapoints." << endl;
	// set time2
	gettimeofday(&tim, NULL);
	double t2=tim.tv_sec+(tim.tv_usec/1000000.0);

	// time elaps
	double timeElap = t2-t1;

	cout << "File Transfer Time (seconds): " << timeElap << endl;
	} else { // single data point requested
		datamsg x (p, t, e);
		chan->cwrite (&x, sizeof (datamsg)); // question
		double reply;
		int nbytes = chan->cread (&reply, sizeof(double)); //answer
		cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
	}
}

// file transfer function
void fileTransfer(RequestChannel* chan, string filename, int bufsize, int chanNum, string ipcMethod) {

	// set time1
	timeval tim;
	gettimeofday(&tim, NULL);
	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

	// set filename
	string fname = filename;

	// create an empty filemsg
	filemsg fm (0,0);
	
	int len = sizeof (filemsg) + fname.size()+1;
	char buf2 [len];
	memcpy (buf2, &fm, sizeof (filemsg));
	strcpy (buf2 + sizeof (filemsg), fname.c_str());
	chan->cwrite (buf2, len);  // I want the file length;

	// get total file size
	__int64_t fileSize;
	int nbytes = chan->cread(&fileSize, sizeof(__int64_t));

	// initialize fm
	fm.offset = 0;
	fm.length = MAX_MESSAGE;

	// create a file in the recieved directory
	ofstream myFile("recieved/" + filename);

	// determine channel operation size
	int chanOpSize = (fileSize/chanNum);

	for (int i = 0; i < chanNum; i ++) {
		// transfer
		// number of times to loop
		int finalLoop = (chanOpSize/MAX_MESSAGE) + 1;

		int size = MAX_MESSAGE;

		// create message
		MESSAGE_TYPE channel = NEWCHANNEL_MSG;

		// send message on control channel to create a new channel
		chan->cwrite(&channel, sizeof(MESSAGE_TYPE)); // question

		// recieve and store message
		char channelName[30];
		int nbytes = chan->cread(&channelName, 30); // answer

		RequestChannel* newChan = nullptr;

		// join the new channel
		if (ipcMethod == "f") {
			newChan = new FIFORequestChannel(channelName, RequestChannel::CLIENT_SIDE);
		} else if (ipcMethod == "q") {
			newChan = new MQRequestChannel(channelName, RequestChannel::CLIENT_SIDE, bufsize);
		} else if (ipcMethod == "s") {
			newChan = new SHMRequestChannel(channelName, RequestChannel::CLIENT_SIDE, bufsize);
		}
		
		// loop for numRequests of correct size
		for (int i = 0; i < finalLoop; i++) {

			// update buffer to send to conatain new offset and length information
			int fileMsgLen = sizeof (filemsg) + fname.size()+1;
			char bufferSend [fileMsgLen];
			memcpy (bufferSend, &fm, sizeof(filemsg));
			strcpy (bufferSend + sizeof (filemsg), fname.c_str());
			newChan->cwrite (bufferSend, fileMsgLen); // question

			// read & store chunk
			char bufferRecieve[size];
			nbytes = newChan->cread(&bufferRecieve, size); // answer

			// write chunk to file
			myFile.write(bufferRecieve,size); // add to file

			// update fm offset
			fm.offset += size;

			// keep track of remaining bytes
			chanOpSize = chanOpSize - fm.length;
			chanOpSize = abs(chanOpSize);

			// if on the last loop
			if (i == (finalLoop - 2)) {
				// do a unique read and write
				// dynamically update the size
				size = chanOpSize;
				fm.length = size;
			}
		}

		// close newChan
		// MESSAGE_TYPE m = QUIT_MSG;
    	// newChan->cwrite (&m, sizeof (MESSAGE_TYPE));

	}

	// close file
	myFile.close();

	// set time2
	gettimeofday(&tim, NULL);
	double t2=tim.tv_sec+(tim.tv_usec/1000000.0);

	// time elaps
	double timeElap = t2-t1;

	cout << "File Transfer Time (seconds): " << timeElap << endl;
}


int main(int argc, char *argv[]){

	int buffercap = MAX_MESSAGE;
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	bool f = false;
	int c = 1;
	string filename = "";
	string ipcMethod = "f";

	while ((opt = getopt(argc, argv, "p:t:e:f:c:i:m:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't': 
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				f = true;
				break;
			case 'c':
				// get the number of channels
				c = atoi (optarg);
				break;
			case 'i':
				// get the ipc method 
				ipcMethod = optarg;
				break;
			case 'm':
				// get the ipc method 
				buffercap = atoi(optarg);
				break;
		}
	}

	pid_t process = fork();

	// if fail
	if (process < 0) {
		cout << "Fork Error!" << endl;
		return 0;
	}

	// run server as a child
	if (process == 0) {
		char* args [] = {"./server", "-m", (char *) to_string(buffercap).c_str(), "-i", (char *) ipcMethod.c_str(), NULL};
		execvp (args [0], args);
	}

	// what type of channel is being opened
	if (ipcMethod == "f") { // FIFO
		// open FIFOchannel on client side
		RequestChannel* chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
		if (f) { // file transfer request
			fileTransfer(chan, filename, buffercap, c, ipcMethod);	
		} else { // data point request
			DataPointReq(chan, p, t, e);
		}

		// close control channel  
		MESSAGE_TYPE m = QUIT_MSG;
		chan->cwrite (&m, sizeof (MESSAGE_TYPE));
		delete chan;

	} else if  (ipcMethod == "q") { // MessageQueue
		RequestChannel* chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE, buffercap);

		if (f) { // file transfer request
			fileTransfer(chan, filename, buffercap, c, ipcMethod);
			
		} else { // data point request
			DataPointReq(chan, p, t, e);
		}

		// close control channel  
		MESSAGE_TYPE m = QUIT_MSG;
		chan->cwrite (&m, sizeof (MESSAGE_TYPE));
		delete chan;

	} else if (ipcMethod == "s") { // SharedMemory
		RequestChannel* chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE, buffercap);

		if (f) { // file transfer request
			fileTransfer(chan, filename, buffercap, c, ipcMethod);
			
		} else { // data point request
			DataPointReq(chan, p, t, e);
		}

		// close control channel  
		MESSAGE_TYPE m = QUIT_MSG;
		chan->cwrite (&m, sizeof (MESSAGE_TYPE));
		delete chan;

	} else {
		cout << "Error, this IPC method does not exist." << endl;
		return 0;
	}

	// new channel request 
	// if (c) {
		// // create message
		// MESSAGE_TYPE channel = NEWCHANNEL_MSG;

		// // send message
		// chan.cwrite(&channel, sizeof(MESSAGE_TYPE)); // question

		// // recieve and store message
		// char channelName[30];
		// int nbytes = chan.cread(&channelName, 30); // answer

		// // join the new channel
		// FIFORequestChannel newChan (channelName, RequestChannel::CLIENT_SIDE);

		// // request some data points through the new channel
		// p = 2;
		// t = 1;
		// e = 2;
		// datamsg x (p, t, e);
		// newChan.cwrite (&x, sizeof (datamsg)); // question --> newChan
		// double reply;
		// nbytes = newChan.cread (&reply, sizeof(double)); //answer --> newChan
		// cout << "By using the newChan: ";
		// cout << "p(" << x.person <<"), t(" << x.seconds << "), e("<< x.seconds <<") = " << reply << endl;

		// // request another data point through newChan
		// x.person = 2;
		// x.seconds = 2;
		// x.ecgno = 2;
		// newChan.cwrite (&x, sizeof (datamsg)); // question --> newChan
		// reply;
		// nbytes = newChan.cread (&reply, sizeof(double)); //answer --> newChan
		// cout << "By using the newChan: ";
		// cout << "p(" << x.person <<"), t(" << x.seconds << "), e("<< x.ecgno <<") = " << reply << endl;

		// // close the new channel
    	// MESSAGE_TYPE m = QUIT_MSG;
    	// newChan.cwrite (&m, sizeof (MESSAGE_TYPE));
	// }

	// wait function for fork/exec
	int status;
	wait(&status);
	cout << "Wait status returned: " << status << endl;
}