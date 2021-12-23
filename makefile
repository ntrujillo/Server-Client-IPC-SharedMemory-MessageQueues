# makefile

all: server client

common.o: common.h common.cpp
	g++ -g -w -std=c++11 -c common.cpp -lrt -pthread

SHMQueue.o: SHMQueue.h SHMQueue.cpp
	g++ -g -w -std=c++11 -c SHMQueue.cpp -lrt -pthread

SHMreqchannel.o: SHMreqchannel.h SHMreqchannel.cpp SHMQueue.o
	g++ -g -w -std=c++11 -c SHMreqchannel.cpp -lrt -pthread

MQreqchannel.o: MQreqchannel.h MQreqchannel.cpp
	g++ -g -w -std=c++11 -c MQreqchannel.cpp -lrt -pthread

FIFOreqchannel.o: FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -std=c++11 -c FIFOreqchannel.cpp -lrt -pthread

RequestChannel.o: RequestChannel.h RequestChannel.cpp
	g++ -g -w -std=c++11 -c RequestChannel.cpp -lrt -pthread

client: client.cpp RequestChannel.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMQueue.o common.o
	g++ -g -w -std=c++11 -o client client.cpp RequestChannel.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMQueue.o common.o -lpthread -lrt -pthread

server: server.cpp  RequestChannel.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMQueue.o common.o
	g++ -g -w -std=c++11 -o server server.cpp RequestChannel.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMQueue.o common.o -lpthread -lrt -pthread

clean:
	rm -rf *.o *.csv fifo* server client data*_*
