CXX=g++
CXXFLAGS=-std=c++17 -g -pedantic -Wall -Wextra -Werror -pthread
LDLIBS=


SRCS=Teller.cpp
DEPS=BankAccount.cpp
BINS=$(SRCS:%.cpp=%.exe)
OBJS=$(DEPS:%.cpp=%.o)


all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^ $(LDLIBS)


.PHONY: clean test

clean:
	rm -f Teller out.trace ./test-files/cmd*.txt ./test-files/add*.txt

test: all
	chmod u+x le6-tests.sh
	./le6-tests.sh