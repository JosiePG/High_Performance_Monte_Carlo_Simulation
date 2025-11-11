
# Specify the C++ compiler
CXX     = g++

# optimisation level, outputs debugging info for gdb, and C++ version to use.
CXXFLAGS = -std=c++20

All: all
all: main

# These are the two executables to be produced
main: src/main.cpp monte_carlo_simulation.o
	$(CXX) $(CXXFLAGS) src/main.cpp monte_carlo_simulation.o -o main

# These are the "intermediate" object files
# The -c command produces them
FileSystem.o: src/monte_carlo_simulation.cpp src/monte_carlo_simulation.h
	$(CXX) $(CXXFLAGS) -c src/monte_carlo_simulation.cpp -o monte_carlo_simulation.o

# Some cleanup functions, invoked by typing "make clean" or "make deepclean"
deepclean:
	rm -f *~ *.o main main.exe *.stackdump

clean:
	rm -f *~ *.o *.stackdump
