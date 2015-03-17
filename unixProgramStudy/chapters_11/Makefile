test: threadcondtest.o
	gcc -o test threadcondtest.o -lpthread

threadcondtest.o: threadcondtest.c threadcond.h
	gcc -c threadcondtest.c

clean:
	rm -rf *.o test
