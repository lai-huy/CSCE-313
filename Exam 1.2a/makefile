CXX=g++
CXXFLAGS=-std=c++17 -g -pedantic -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer


SRCS=p1.cpp p1-helper.cpp
BINS=$(patsubst %.cpp,%.exe,$(SRCS))
DEPS=


all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^


.PHONY: clean

clean:
	rm -f p1 p1-helper

test: all
	chmod u+x p1-tests.sh
	./p1-tests.sh