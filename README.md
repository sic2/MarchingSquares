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

	[] Camera dynamic path
	[] Create an extended set of predefined locations of the camera
	[] Create more color schemes
	[] Calculate contour's color once rather than every time
	[] If number of contours cannot be halved, than set to max
	[] Create menus
	[] Info panel
	[] Be able to pick a contour, color it and display info about it

# Known bugs

	[] Perspective projection works on Linux Lab Machines only

# Requirements

	gcc compiler, compliant to c++03

## Tested targets
	i686-apple-darwin10-gcc-4.2.1 on Mac OS X 10.6.8
	gcc-4.4.7 (Red Hat 4.4.7-3) on Scientific Linux - Kernel Linux 2.6.32

## Profiling

	gperftools-2.1 (http://code.google.com/p/gperftools/)
	perl 5
	gv (installable via MacPorts)


# Third party resources

The makefile is a modified version of the one provided by Michael Safyan:
	https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile
