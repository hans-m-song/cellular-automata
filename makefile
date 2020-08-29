CXX=g++
CXXFLAGS = -Wall -std=c++14 $(DEFINE)
OBJ = grid.o main.o util.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: main

new: clean main

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o cellular_automata $(OBJ)

visual: CXXFLAGS += -DVISUAL
visual: new

debug: CXXFLAGS += -DDEBUG
debug: visual

clean:
	rm -f *.o
	rm -f epidemic
