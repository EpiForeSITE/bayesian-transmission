
# Compile with -g for valgrind tracing with program line numbers.
#C++ = /usr/bin/g++ -g -pg -O -pedantic -ansi -Wall 
#C++ = /usr/bin/g++ -O3 -pedantic -Wall 
C++ = /usr/bin/g++ -O3

SRCHOME = ..
BIN = ../bin

UTILHOME = $(SRCHOME)/util
INFECTHOME = $(SRCHOME)/infect
MODELHOME = $(SRCHOME)/modeling
LOGNORMALHOME = $(SRCHOME)/lognormal

INCLUDE = -I. -I$(UTILHOME) -I$(INFECTHOME) -I$(MODELHOME) -I$(LOGNORMALHOME)
LIBDIRS = -L. -L$(UTILHOME)
LINKS = -lutil

REALS = LTCFMCMC
TESTS = listtest
#TESTS = lnoMassAct 

HEADERS :=  $(shell  sed s/"\t"/+/g ltcf.h | grep ++ | sed s/\"/=/ | sed s/.\*=// | sed s/\"//)

TARGETS = $(REALS) $(TESTS) 

all: $(TARGETS)
	
$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)
	cp $@ $(BIN)


ltcf.h : $(HEADERS) $(UTILHOME)/util.h $(INFECTHOME)/infect.h $(MODELHOME)/modeling.h $(LOGNORMALHOME)/lognormal.h
	touch ltcf.h

% : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

%.o: %.cc ltcf.h
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o $(REALS) $(TESTS)
