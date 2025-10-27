
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall 
C++ = /usr/bin/g++ -O3 -pedantic -Wall 

SRCHOME = ..
BIN = ../bin

UTILHOME = $(SRCHOME)/util
INFECTHOME = $(SRCHOME)/infect

INCLUDE = -I. -I$(UTILHOME) -I$(INFECTHOME) 
LIBDIRS = -L. -L$(UTILHOME)
LINKS = -lutil

REALS = makeRealistic shortForm sampleModel simulateSystem cheatSample checkEvents checkByPatient 
TESTS = simMassAct estMassAct

HEADERS :=  $(shell  sed s/"\t"/+/g modeling.h | grep ++ | sed s/\"/=/ | sed s/.\*=// | sed s/\"//)

TARGETS = $(TESTS) $(REALS)

all: $(TARGETS) 
	
$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)
	cp $@ $(BIN)

modeling.h : $(HEADERS) $(UTILHOME)/util.h $(INFECTHOME)/infect.h
	touch modeling.h

% : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

%.o: %.cc modeling.h
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o $(REALS) $(TESTS)
