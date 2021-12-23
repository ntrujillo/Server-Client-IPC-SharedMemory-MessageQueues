#ifndef _FIFOreqchannel_H_
#define _FIFOreqchannel_H_

#include "RequestChannel.h"
#include "common.h"

class FIFORequestChannel : public RequestChannel
{
	
private:
	/*  The current implementation uses named pipes. */
	
	int wfd;
	int rfd;
	
	string pipe1, pipe2;
	int open_pipe(string _pipe_name, int mode);
	
public:
	FIFORequestChannel(const string _name, const Side _side);
    
	~FIFORequestChannel();
	/* Destructor of the local copy of the bus. By default, the Server Side deletes any IPC 
	 mechanisms associated with the channel. */


	int cread (void* msgbuf, int bufcapacity);
	/* Blocking read of data from the channel. You must provide the address to properly allocated
	memory buffer and its capacity as arguments. The 2nd argument is needed because the recepient 
	side may not have as much capacity as the sender wants to send.
	
	In reply, the function puts the read data in the buffer and  
	returns an integer that tells how much data is read. If the read fails, it returns -1. */
	
	int cwrite (void* msgbuf, int bufcapacity);
	/* Writes msglen bytes from the msgbuf to the channel. The function returns the actual number of 
	bytes written and that can be less than msglen (even 0) probably due to buffer limitation (e.g., the recepient
	cannot accept msglen bytes due to its own buffer capacity. */
	 
	string name(); 
};

#endif