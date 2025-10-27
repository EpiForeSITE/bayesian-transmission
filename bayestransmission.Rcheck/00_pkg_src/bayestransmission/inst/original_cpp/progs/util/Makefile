
C++ = /usr/bin/g++ -g -pedantic -ansi -Wall 

INCLUDE = -I.
LIBDIRS = -L.
LINKS = 

TARGETS = test #test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13

all: $(TARGETS) 
	/bin/rm -f *.o

$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

%.o: %.cc
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o  $(TARGETS)
