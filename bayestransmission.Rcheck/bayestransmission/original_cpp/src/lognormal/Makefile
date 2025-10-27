
# Compile with -g for valgrind tracing with program line numbers.
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall 
C++ = /usr/bin/g++ -O3 -pedantic -Wall 

SRCHOME = ..
BIN = ../bin

UTILHOME = $(SRCHOME)/util
INFECTHOME = $(SRCHOME)/infect
MODELHOME = $(SRCHOME)/modeling

INCLUDE = -I. -I$(UTILHOME) -I$(INFECTHOME) -I$(MODELHOME)
LIBDIRS = -L. -L$(UTILHOME)
LINKS = -lutil

REALS = runMCMC cheatMCMC runForward
TESTS = lnoMassAct 

HEADERS :=  $(shell  sed s/"\t"/+/g lognormal.h | grep ++ | sed s/\"/=/ | sed s/.\*=// | sed s/\"//)

TARGETS = $(REALS) $(TESTS) 

all: $(TARGETS)
	
$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)
	cp $@ $(BIN)


lognormal.h : $(HEADERS) $(UTILHOME)/util.h $(INFECTHOME)/infect.h $(MODELHOME)/modeling.h
	touch lognormal.h

% : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

%.o: %.cc lognormal.h
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o $(REALS) $(TESTS)
