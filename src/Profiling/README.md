# How to use profiling data

Generate textual listing
	$ pprof --text ./../squareShift squareShift.prof

Generate annotated callgraph 
	$ pprof --gif ./../squareShift squareShift.prof > OUTPUT.gif
