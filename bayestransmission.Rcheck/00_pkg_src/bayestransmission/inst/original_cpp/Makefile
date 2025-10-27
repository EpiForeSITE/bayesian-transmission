

all: compile simulated.data linearabx2.Model
	src/lognormal/runMCMC linearabx2.Model 1 0 1000 1 0 1 1 1 < simulated.data > output
	diff output output.compare

compile:
	cd src; make

simulated.data: compile test.data simulated.Model
	src/bin/runForward simulated.Model 0 1 1  < test.data > simulated.data

clean:
	/bin/rm -r -f 
