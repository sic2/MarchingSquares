# CS4102 CG Practical 3 [-] SquareShift

# https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile
program_NAME := squareShift
program_C_SRCS := $(wildcard src/*.c)
program_CXX_SRCS := $(wildcard src/*.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))
CXXFLAGS = -Wall -g 

UNAME := $(shell uname)

# GL Flags
ifeq ($(UNAME), Darwin)
	GLFLAGS = -framework OpenGL -framework GLUT	
endif
ifeq ($(UNAME), Linux)
	GLFLAGS = -lGL -lGLU -lglut
endif

.PHONY: all clean distclean

all: CXXFLAGS += -DPROFILER=0
all: $(program_NAME) 

profile: CXXFLAGS += -lprofiler -DPROFILER=1
profile: $(program_NAME)

release: CXXFLAGS += -O2 -DPROFILER=0
release: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) $(GLFLAGS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	@- $(RM) Profiling/*.prof # removes any profiling files

distclean: clean
