CXX=g++
CXXFLAGS=-std=c++17 -pedantic -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer
LDLIBS=-lrt

SRCS=burgers_and_fries.cpp
BINS=$(SRCS:%.cpp=%.exe)
OBJS=$(DEPS:%.cpp=%.o)

all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^ $(LDLIBS)

.PHONY: clean 

clean:
	rm -f burgers_and_fries

	