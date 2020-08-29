#include "grid.hpp"
#include <chrono>
#include <iostream>
#include <unistd.h>

/**
 * Runs the simulation
 * @param {Grid*} grid : a grid containing the simulation state
 * @param {int} ticks  : duration to run simulation for
 * @return {long long} : time taken to run simulation in microseconds
 */
long long run(Grid* grid, int ticks) {
  using namespace std::chrono;
  auto start = high_resolution_clock::now();

  for (int i = 0; i < ticks; i++) {
    grid->tick();
#ifdef VISUAL
    grid->print();
    usleep(100000);
#endif
  }

  auto end = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(end - start);
  return duration.count();
}

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "incorrect number of arguments\n"
              << "usage:   ./cellular_automata width height density ticks\n"
              << "example: ./cellular_automata 1000  1000   0.5     100\n";
    return 1;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  double initial_density = atof(argv[3]);
  int ticks = atoi(argv[4]);

  Grid* grid = new Grid(width, height, initial_density);

  long long duration = run(grid, ticks);

  int initial_cells = (int)(initial_density * width * height);
  std::cout << width << "," << height << "," << initial_density << "," << ticks
            << "," << initial_cells << "," << grid->sum() << "," << duration
            << "\n";

  delete grid;
  return 0;
}