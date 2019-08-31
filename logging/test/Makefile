TARGETS = LogStream_test Logging_test LogFile_test

all: $(TARGETS)

LogStream_test: LogStream_test.cc ../LogStream.cc ../../datetime/TimeStamp.cc
	g++ LogStream_test.cc ../LogStream.cc ../../datetime/TimeStamp.cc -o LogStream_test -Wall -g -O0 -std=c++11
	
Logging_test: Logging_test.cc ../Logging.cc ../../datetime/TimeStamp.cc ../LogFile.cc ../LogStream.cc ../../thread/Thread.cc
	g++ Logging_test.cc ../Logging.cc ../LogStream.cc ../LogFile.cc ../../datetime/TimeStamp.cc ../../thread/Thread.cc -o Logging_test -pthread -Wall -g -O0 -std=c++11
	
LogFile_test: LogFile_test.cc ../Logging.cc ../../datetime/TimeStamp.cc ../LogFile.cc ../LogStream.cc ../../thread/Thread.cc
	g++ LogFile_test.cc ../Logging.cc ../LogStream.cc ../LogFile.cc ../../datetime/TimeStamp.cc ../../thread/Thread.cc -o LogFile_test -pthread -Wall -g -O0 -std=c++11
    
clean:
	rm -rf $(TARGETS)
	rm -rf *.log