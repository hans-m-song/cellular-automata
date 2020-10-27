
#include <chrono>
#include <iomanip>
#include <iostream>

#include "grid.hpp"
#include "metric.hpp"

template <typename T>
void log(std::string label, T value) {
  std::cout << std::setw(10) << label << ":" << std::setw(10) << value << "\n";
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

  auto metric = run_grid(width, height, initial_density, ticks);
  auto avg = metric.duration(Measure::Run).count() / ticks;

#ifdef VISUAL
  log("Width", width);
  log("Height", height);
  log("Density", initial_density);
  log("Ticks", ticks);
  metric.log();
  log("Tick avg", avg);
#else
  std::cout << width << "," << height << "," << initial_density << "," << ticks
            << "," << metric.csv() << "," << avg << "\n";
#endif

  return 0;
}
