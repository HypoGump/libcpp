CC := g++
CXXFALGS := -g -Wall -I ../.. -std=c++11 -pthread

DIRS := $(shell ls -l ../../ | grep ^d | awk '{print $$9}')
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../$(dir)/*.cc))
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../$(dir)/*.h))

TARGETS := test1 test2 test3 test4 test5 test6

all: $(TARGETS)

test1: test1.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test2: test2.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test3: test3.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test4: test4.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)
  
test5: test5.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test6: test6.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm $(TARGETS)
