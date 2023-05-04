all: clean echoall printvars

echoall: echoall.c
	gcc -o echoall echoall.c

printvars: printvars.c
	gcc -o printvars printvars.c

.PHONY: clean

clean:
	rm -f echoall printvars