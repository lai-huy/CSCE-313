CXX=g++
CXXFLAGS=-std=c++17 -g -pedantic -Wall -Wextra -Werror -fno-omit-frame-pointer


SRCS=p2.cpp
BINS=$(patsubst %.cpp,%.exe,$(SRCS))
DEPS=


all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^


.PHONY: clean

clean:
	rm -f p2 ./test-files/tmp.txt

test: all
	chmod u+x p2-tests.sh
	./p2-tests.sh