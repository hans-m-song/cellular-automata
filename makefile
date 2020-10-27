CXX				= g++
CXXFLAGS 	= -Wall -std=c++1y $(DEFINE)
OBJ 			= metric.o grid.o util.o main.o

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
	rm -f cellular_automata
