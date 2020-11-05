FLAGS     = -O2 -std=c++11 -fopenmp
CXX       = g++
CXXFLAGS  = -Wall
OBJ       = metric.o util.o randutil.o
NVCC      = nvcc
NVFLAGS   = --gpu-architecture=sm_35 -Wno-deprecated-gpu-targets
MPICXX    = mpic++

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -c -o $@ $<

all: main

main: $(OBJ) grid.o main.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o cellular_automata $(OBJ) grid.o main.o

cuda: $(OBJ) main.o
	$(NVCC) $(NVFLAGS) $(FLAGS) -o cuda_cellular_automata $(OBJ) main.o grid.cu

parallel: $(OBJ)
	$(MPICXX) $(CXXFLAGS) $(FLAGS) -o parallel_cellular_automata $(OBJ) main-parallel.cpp

debug: FLAGS += -DDEBUG
debug: visual

visual: FLAGS += -DVISUAL
visual: clean main

debugcuda: FLAGS += -DDEBUG
debugcuda: visualcuda

visualcuda: FLAGS += -DVISUAL
visualcuda: clean cuda

debugparallel: FLAGS += -DDEBUG
debugparallel: visualparallel

visualparallel: FLAGS += -DVISUAL
visualparallel: clean parallel

clean:
	rm -f *.o
	rm -f cellular_automata
	rm -f cuda_cellular_automata
	rm -f parallel_cellular_automata
