#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include "grid.hpp"
#include "metric.hpp"
#include "util.hpp"

Metric metric;
int worker_count, id, width, height, size, rows, ticks;
double initial_density;
int *init_generation, *generation, *next_generation;
int up_id, *up_row, down_id, *down_row;
int *sendcounts, *displs;

template <typename T>
void log(std::string label, T value) {
  std::cout << std::setw(10) << label << ":" << std::setw(10) << value << "\n";
}

void print(void) {
#ifdef VISUAL
#ifndef DEBUG
  std::cout << "\e[1;1H\e[2J";
#endif
#endif

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (init_generation[x * width + y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

std::string print_arr(int* a, int length) {
  std::string result = "[ ";
  for (int i = 0; i < length; i++) {
    if (a[i] < 10) {
      result.append(" ");
    }
    result.append(std::to_string(a[i]));
    if (i < length - 1) {
      result.append(", ");
    }
  }
  result.append(" ]");
  return result;
}

int* allocate_space(void) {
  int* space = new int[size];
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      space[x * width + y] = 0;
    }
  }
  return space;
}

Point empty_cell(void) {
  int x, y;
  for (int i = 0; i < size; i++) {
    x = random(0, width - 1);
    y = random(0, height - 1);
    if (!init_generation[x * width + y]) {
      return Point(x, y);
    }
  }

  return Point(-1, -1);
}

int sum_neighbour(int x, int y) {
  int sum = 0;
  for (int j = y - 1; j < y + 1; j++) {
    for (int i = x - 1; i < x + 1; i++) {
      int coord = (i + width) % width;
      if (j < 0) {  // above owned grid section
        if (up_row[coord]) {
          sum += 1;
        }
      } else if (j >= rows) {
        if (down_row[coord]) {  // below owned grid section
          sum += 1;
        }
      } else {  // within owned grid section
        if (generation[j * width + coord]) {
          sum += 1;
        }
      }
    }
  }

  return sum;
}

void tick(void) {
  // #pragma omp parallel for
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < width; x++) {
      int neighbours = sum_neighbour(x, y);
      if (neighbours == 3) {
        // if 3 live neighbours exactly, cell lives
        next_generation[y * width + x] = 1;
      } else if (neighbours == 2) {
        // if 2 live neighbours exactly, cell maintains status
        next_generation[y * width + x] = generation[y * width + x];
      } else {
        // otherwise else cell dies
        next_generation[y * width + x] = 0;
      }
    }
  }
  // swap currently active generation
  std::swap(generation, next_generation);
}

void run(void) {
  metric.start(Measure::Run);

  MPI_Status s;
  for (int i = 0; i < ticks; i++) {
    // send top row and receive bottom row
    MPI_Sendrecv(generation, width, MPI_INT, up_id, 1, down_row, width, MPI_INT,
                 down_id, 1, MPI_COMM_WORLD, &s);

    // send bottom row and receive top row
    MPI_Sendrecv(generation + sendcounts[id] - width, width, MPI_INT, down_id,
                 2, up_row, width, MPI_INT, up_id, 2, MPI_COMM_WORLD, &s);

#ifdef DEBUG
    std::cout << id << " - " << print_arr(up_row, width) << "\n"
              << "    " << print_arr(generation, sendcounts[id]) << "\n"
              << "    " << print_arr(down_row, width) << "\n";
#endif

    tick();

    MPI_Gatherv(generation, sendcounts[id], MPI_INT, init_generation,
                sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

#ifdef VISUAL
    if (id == 0) {
      print();
      usleep(100000);
    }
#endif
  }

  metric.stop(Measure::Run);
}

void init(void) {
  // begin measuring benchmarks
  metric.start(Measure::Init);

  // dynamically allocate space for generations
  init_generation = allocate_space();

  // number of initial live cells
  int initial_cells = (int)(initial_density * width * height);

  // populate first generation
  Point point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell();
    init_generation[point.first * width + point.second] = 1;
  }

  metric.stop(Measure::Init);
}
void scatter(void) {
  int sum = 0;
  int count = height / worker_count;
  int leftover = height % worker_count;
  sendcounts = new int[worker_count];
  displs = new int[worker_count];

  for (int i = 0; i < worker_count; i++) {
    sendcounts[i] = count * width;
    if (leftover > 0) {
      sendcounts[i] += width;
      leftover -= 1;
    }
    displs[i] = sum;
    sum += sendcounts[i];
  }

  generation = new int[sendcounts[id]];
  next_generation = new int[sendcounts[id]];
  up_row = new int[width];
  down_row = new int[width];
  rows = sendcounts[id] / width;

  MPI_Scatterv(init_generation, sendcounts, displs, MPI_INT, generation,
               sendcounts[id], MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &worker_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  up_id = (id - 1 + worker_count) % worker_count;
  down_id = (id + 1) % worker_count;

  if (argc != 5) {
    std::cout << "incorrect number of arguments\n"
              << "usage:   ./cellular_automata width height density ticks\n"
              << "example: ./cellular_automata 1000  1000   0.5     100\n";
    MPI_Finalize();
    return 1;
  }

  width = atoi(argv[1]);
  height = atoi(argv[2]);
  initial_density = atof(argv[3]);
  ticks = atoi(argv[4]);
  size = height * width;

  metric.start(Measure::Total);

  if (id == 0) {
    init();
    print();
  }

  scatter();

#ifdef DEBUG
  if (id == 0) {
    std::cout << "sendcounts " << print_arr(sendcounts, worker_count) << "\n"
              << "displs     " << print_arr(displs, worker_count) << "\n";
  }
#endif

  run();

  metric.stop(Measure::Total);

  if (id == 0) {
    auto avg = metric.duration(Measure::Run).count() / ticks;
#ifdef VISUAL
    log("Width", width);
    log("Height", height);
    log("Density", initial_density);
    log("Ticks", ticks);
    metric.log();
    log("Tick avg", avg);
#else
    std::cout << width << "," << height << "," << initial_density << ","
              << ticks << "," << metric.csv() << "," << avg << "\n";
#endif
  }

  if (id == 0) {
    delete[] init_generation;
  }
  delete[] generation;
  delete[] next_generation;
  delete[] up_row;
  delete[] down_row;
  delete[] sendcounts;
  delete[] displs;
  MPI_Finalize();
  return 0;
}
