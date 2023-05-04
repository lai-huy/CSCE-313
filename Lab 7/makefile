CXX=g++
CXXFLAGS=-std=c++17 -g -pedantic -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer
LDLIBS=

SRCS=data_entry.cpp
DEPS=BoundedBuffer.cpp
BINS=$(SRCS:%.cpp=%.exe)
OBJS=$(DEPS:%.cpp=%.o)

all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^ $(LDLIBS)


.PHONY: clean print-var test

clean:
	rm -f data_entry a.out *.o *.csv *.trace ./test-files/cmd*

test: all
	chmod u+x le7-tests.sh
	./le7-tests.sh