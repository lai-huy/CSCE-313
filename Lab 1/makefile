all: buggy

buggy: buggy.cpp
	g++ -std=c++17 -pedantic -Wall -Wextra -fsanitize=address,undefined -o buggy.out buggy.cpp

memory_errors:
	g++ -std=c++17 -pedantic -Wall -Wextra -o buggy.out buggy.cpp
	valgrind --show-leak-kinds=all --leak-check=full -s ./buggy.out

clean:
	rm -rf buggy.out tests *.out *.debug *.txt

test: all
	chmod u+x tests.sh
	./tests.sh
