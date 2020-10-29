FLAGS     = -O2 -std=c++11
CXX       = g++
CXXFLAGS  = -Wall
OBJ       = metric.o util.o
SERIALOBJ = $(OBJ) grid.o main.o
CUDAOBJ   = $(OBJ) main.o
MPIOBJ    = $(OBJ) grid-parallel.o main-parallel.o
NVCC      = nvcc
NVFLAGS   = --gpu-architecture=sm_35 -Wno-deprecated-gpu-targets
MPICXX    = mpic++

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -c -o $@ $<

all: main

main: $(SERIALOBJ)
	$(CXX) $(CXXFLAGS) $(FLAGS) -o cellular_automata $(SERIALOBJ)

cuda: $(CUDAOBJ)
	$(NVCC) $(NVFLAGS) $(FLAGS) -o cuda_cellular_automata $(CUDAOBJ) grid.cu

mpi: $(MPIOBJ)
	$(MPICXX) $(CXXFLAGS) -fopenmp $(FLAGS) -o mpi_cellular_automata $(MPIOBJ)

debug: FLAGS += -DDEBUG
debug: visual

visual: FLAGS += -DVISUAL
visual: clean main

debugcuda: FLAGS += -DDEBUG
debugcuda: visualcuda

visualcuda: FLAGS += -DVISUAL
visualcuda: clean cuda

debugmpi: FLAGS += -DDEBUG
debugmpi: visualmpi

visualmpi: FLAGS += -DVISUAL
visualmpi: clean mpi

clean:
	rm -f *.o
	rm -f cellular_automata
	rm -f cuda_cellular_automata
	rm -f mpi_cellular_automata
