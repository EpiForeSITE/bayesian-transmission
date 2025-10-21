
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall 
C++ = /usr/bin/g++ -O3 -pedantic -Wall 

SRCHOME = ..
BIN = ../bin

UTILHOME = $(SRCHOME)/util

INCLUDE = -I. -I$(UTILHOME)
LIBDIRS = -L. -L$(UTILHOME)
LINKS = -lutil

REALS = trackColonized reformat checkData countTransfers getTests trackOccupancy
TESTS =
TARGETS = $(TESTS) $(REALS)

HEADERS =  $(shell  sed s/"\t"/+/g infect.h | grep ++ | sed s/\"/=/ | sed s/.\*=// | sed s/\"//)

all: $(TARGETS) 
	
infect.h: $(HEADERS) $(UTILHOME)/util.h
	touch infect.h

$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)
	cp $@ $(BIN)

%.o: %.cc infect.h
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o $(REALS) $(TESTS)
