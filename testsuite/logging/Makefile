TARGETS = Logging_test LogFile_test

all: Logging_test

Logging_test: Logging_test.cc ../Logging.cc ../LogFile.cc 
	g++ Logging_test.cc ../Logging.cc ../LogFile.cc ../../datetime/TimeStamp.cc -o Logging_test -pthread -Wall -g -O0 -std=c++11
	
LogFile_test: LogFile_test.cc ../Logging.cc ../LogFile.cc
	g++ LogFile_test.cc ../Logging.cc ../LogFile.cc ../../datetime/TimeStamp.cc -o LogFile_test -pthread -Wall -g -O0 -std=c++11
    
clean:
	rm -rf $(TARGETS)
	rm -rf *.log