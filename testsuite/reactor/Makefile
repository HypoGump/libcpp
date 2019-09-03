CC := g++
CXXFALGS := -g -Wall -I ../.. -std=c++11 -pthread

DIRS := $(shell ls -l ../../ | grep ^d | awk '{print $$9}')
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../$(dir)/*.cc))
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../$(dir)/*.h))

TARGETS := test1 test2 test3

all: $(TARGETS)

test1: test1.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test2: test2.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test3: test3.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm $(TARGETS)
