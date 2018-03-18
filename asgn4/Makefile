all: libmyfs.a test1 test2 test3 test4

libmyfs.a: asgn4.h asgn4.cpp
	g++ -Wall asgn4.cpp -c
	ar rc libmyfs.a asgn4.o
	rm asgn4.o

test1: test1.cpp libmyfs.a
	g++ -Wall test1.cpp libmyfs.a -o test1

test2: test2.cpp libmyfs.a
	g++ -Wall test2.cpp libmyfs.a -o test2

test3: test3.cpp libmyfs.a
	g++ -Wall test3.cpp libmyfs.a -o test3

test4: test4.cpp libmyfs.a
	g++ -Wall test4.cpp libmyfs.a -o test4

clean:
	rm libmrfs.a test1 test2 test3 test4
