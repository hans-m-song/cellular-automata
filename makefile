CXX=g++
CXXFLAGS = -Wall -std=c++14
OBJ = grid.o main.o util.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: main

new: clean main

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o cellular_automata $(OBJ)

visual: CXXFLAGS += -DDEBUG
visual: new

clean:
	rm -f *.o
	rm -f epidemic
