
C++ = /usr/bin/g++ -g -pedantic -ansi -Wall 

UTILHOME = $(HOME)/sdbc/infect/src/util

INCLUDE = -I. -I$(UTILHOME)
LIBDIRS = -L.
LINKS = 

TARGETS = simcols summary onewrong onemix onetest coltimes oneunit getnpatients getnepisodes #alltest #changetest

#TARGETS = alltest staricu onetest changetest test2 testindep infection multiinf hierinf test cleanup sortraws countfneg coopertest tocooper hiertest hiertest2 coltest imptest fntest nocoltest noimptest nofntest 

all: $(TARGETS) 
	/bin/rm -f *.o
	
$(TARGETS): % : %.o
	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)

#% : %.o
#	$(C++) $(LIBDIRS) -o $@ $< $(LINKS)


%.o: %.cc
	$(C++) $(INCLUDE) -c -o $@ $<
	
clean:
	/bin/rm -f *.o $(TARGETS)
