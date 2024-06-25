
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall 
C++ = /usr/bin/g++ -O -pedantic -Wall -Wno-long-long

SRCHOME = ..
EIGENHOME = $(SRCHOME)/eigen-eigen-5a0156e40feb

INCLUDE = -I. -I$(EIGENHOME)
LIBDIRS = -L. 
LINKS = -lutil

REALS = 
TESTS =  test
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
