# Square Shift

Square Shift is a 2.5D contours modelling viewer. 

======================================

# How to compile and run the program:

Compile:

	$ make

To turn on optimisation (-O2)

	$ make release

Run:

	$ ./squareShift

Clean all compiled files + any profiling data file:

	$ make clean


# Profiling

	$ make profile
	$ ./squareShift

Generate textual listing

	$ pprof --text ./squareShift Profiling/squareShift.prof

Generate annotated callgraph 

	$ pprof --gv ./squareShift Profiling/squareShift.prof

Generate gif callgraph
	
	$ pprof --gif ./squareShift Profiling/squareShift.prof > OUTPUT.gif


# TODO

	[] Perspective projection
	[] Predefined camera locations
	[] Camera dynamic path
	[] Predefined set of colors


# Requirements

	gcc compiler, compliant to c++03

## Tested targets
	i686-apple-darwin10-gcc-4.2.1

## Profiling

	gperftools-2.1 (http://code.google.com/p/gperftools/)
	perl 5
	gv (installable via MacPorts)


# Third party resources

The makefile is a modified version of the one provided by Michael Safyan:
	https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile