CXX=g++
CXXFLAGS = -Wall -pedantic -g -std=c++14
OBJ = grid.o main.o util.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: main

new: clean main

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o cellular_automata $(OBJ)

clean:
	rm -f *.o
	rm -f epidemic
