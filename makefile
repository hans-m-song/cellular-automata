FLAGS    = -O2 -std=c++11
CXX      = g++
CXXFLAGS = -Wall
OBJ      = metric.o util.o main.o
NVCC     = nvcc
NVFLAGS  = --gpu-architecture=sm_35 -Wno-deprecated-gpu-targets

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -c -o $@ $<

all: main

main: $(OBJ) grid.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o cellular_automata $(OBJ) grid.o

cuda: $(OBJ)
	$(NVCC) $(NVFLAGS) $(FLAGS) -o cuda_cellular_automata $(OBJ) grid.cu

visual: FLAGS += -DVISUAL
visual: clean main

visualcuda: FLAGS += -DVISUAL
visualcuda: clean cuda

debug: FLAGS += -DDEBUG
debug: visual

debugcuda: FLAGS += -DDEBUG
debugcuda: visualcuda

clean:
	rm -f *.o
	rm -f cellular_automata
	rm -f cuda_cellular_automata
