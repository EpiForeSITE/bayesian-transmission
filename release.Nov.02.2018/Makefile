
all:
	cd util; make
	cd infect; make
	cd modeling; make
	cd lognormal; make

progs.tar:
	tar -cvf progs.tar Makefile util infect modeling lognormal bin

clean:
	cd util; make clean
	cd infect; make clean
	cd modeling; make clean
	cd lognormal; make clean
	/bin/rm -r -f bin/*

