
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall -Wno-long-long
C++ = /usr/bin/g++ -O3 -pedantic -Wall -Wno-long-long

#SRCHOME = /home/alun/work/Infect/src
SRCHOME = ../../../
EIGENHOME = /usr/include/eigen3

INCLUDE = -I. -I$(EIGENHOME)
LIBDIRS = -L. 
LINKS = -lutil

REALS = 
TESTS = test 
TARGETS = $(TESTS) $(REALS)

HEADERS := $(shell  sed s/"\t"/+/g util.h | grep include | grep ++ | sed s/\"/=/ | sed s/.\*=// | sed s/\"//)

all: libutil.a $(TARGETS) 
	
$(TARGETS): % : %.o libutil.a 
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

#util.h : $(HEADERS)
#	touch util.h

libutil.a : util.o 
	ar r libutil.a util.o
	touch util.h

%.o: %.cc $(HEADERS)
	$(C++) $(INCLUDE) -c -o $@ $< 
	
clean:
	/bin/rm -f *.o $(REALS) $(TESTS) libutil.a
