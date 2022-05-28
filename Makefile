DESIGNAR = ./DeSiGNAR
CXX = clang++ -std=c++14
WARN = -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wno-invalid-source-encoding
FLAGS = -DDEBUG -D_GLIBCXX__PTHREADS -g -O0 $(WARN) 
INCLUDE = -I. -I $(DESIGNAR)/include 
LIBS = Definitions.o -L $(DESIGNAR)/lib -lDesignar

all: GeomTest PathfinderTest

GeomTest: Geom.h Definitions.o Geom.h GeomTest.cpp
	$(CXX) $(FLAGS) $(INCLUDE) $@.cpp -o $@ $(LIBS)

PathfinderTest: Geom.h Definitions.o Geom.h Pathfinder.h PathfinderTest.cpp
	$(CXX) $(FLAGS) $(INCLUDE) $@.cpp -o $@ $(LIBS)

Definitions.o: Definitions.h Definitions.cpp
	$(CXX) -c $(FLAGS) $(INCLUDE) Definitions.cpp

.PHONY: clean
clean:
	$(RM) *~ GeomTest PathfinderTest *.o
