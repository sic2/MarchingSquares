# Square Shift

Square Shift is a 2.5D contours modelling viewer. 

![alt text](https://github.com/sic2/MarchingSquares/blob/master/Other/example.png?raw=true "Honolulu")

======================================

# How to compile and run the program:

Compile:

	$ make

To turn on optimisation (-O2)

	$ make release

Run:

	$ ./squareShift Data/honolulu_raw.txt (or other data sets)

Clean all compiled files + any profiling data file:

	$ make clean

Clean, Compile and Run:

	$ make clean && make release && ./squareShift Data/honolulu_raw.txt (or other data sets)

# Profiling

	$ make profile
	$ ./squareShift Data/honolulu_raw.txt (or other data sets)

Generate textual listing

	$ pprof --text ./squareShift Profiling/squareShift.prof

Generate annotated callgraph using gv

	$ pprof --gv ./squareShift Profiling/squareShift.prof

Generate gif callgraph
	
	$ pprof --gif ./squareShift Profiling/squareShift.prof > OUTPUT.gif

# Demo

	http://youtu.be/FYMh4WhZvuE 

Note:
	Performance of the video demo is affected by the recording tool QuickTime player.

# Known bugs

	[?] Perspective projection works on Linux Lab Machines only

# Requirements

	gcc compiler, compliant to c++03

## Tested targets
	i686-apple-darwin10-gcc-4.2.1 on Mac OS X 10.6.8
	i686-apple-darwin11-llvm-gcc-4.2 on Mac OS X 10.8.5
	gcc-4.4.7 (Red Hat 4.4.7-3) on Scientific Linux - Kernel Linux 2.6.32

## Profiling

	gperftools-2.1 (http://code.google.com/p/gperftools/)
	perl 5
	gv (installable via MacPorts)


# Third party resources

The makefile is a modified version of the one provided by Michael Safyan:
	https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile
