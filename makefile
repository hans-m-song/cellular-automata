CXX				= g++
CXXFLAGS 	= $(DEFINE) -Wall -std=c++1y -O2
OBJ 			= metric.o util.o main.o
NVCC			= nvcc
NVFLAGS		= $(DEFINE) -O2 --gpu-architecture=sm_35 -Wno-deprecated-gpu-targets

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: main

new: clean main

main: $(OBJ) grid.o
	$(CXX) $(CXXFLAGS) -o cellular_automata $(OBJ) grid.o

cuda: $(OBJ)
	$(NVCC) $(NVFLAGS) -o cuda_cellular_automata $(OBJ) grid.cu

visual: CXXFLAGS += -DVISUAL
visual: new

debug: CXXFLAGS += -DDEBUG
debug: visual

clean:
	rm -f *.o
	rm -f cellular_automata
