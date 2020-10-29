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
int upId, *up, downId, *down;
int *sendcounts, *displs;

template <typename T>
void log(std::string label, T value) {
  std::cout << std::setw(10) << label << ":" << std::setw(10) << value << "\n";
}

void print(void) {
#ifndef DEBUG
  std::cout << "\e[1;1H\e[2J";
#endif

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (init_generation[x * width + y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

void print_arr(int* a) {
  int length = sizeof(a) / sizeof(a[0]);
  std::cout << "[ ";
  for (int i = 0; i < length; i++) {
    std::cout << a[i];
    if (i < length - 1) {
      std::cout << ", ";
    }
  }
  std::cout << " ]\n";
}

/**
 * Sends/receives a message
 * 1 to signal job or 0 to signal completion
 */
int msg(int payload = 1) {
  int data = payload;
  MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
  return data;
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
    if (!generation[x * width + y]) {
      return Point(x, y);
    }
  }

  return Point(-1, -1);
}

int sum(void) {
  int sum = 0;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      sum += generation[x * width + y];
    }
  }
  return sum;
}

Point apply_direction(Point origin, Direction direction) {
  int x = origin.first;
  int y = origin.second;
  switch (direction) {
    case Direction::N:
      x += 1;
      break;
    case Direction::NE:
      x += 1;
      y += 1;
      break;
    case Direction::E:
      y += 1;
      break;
    case Direction::SE:
      x -= 1;
      y += 1;
      break;
    case Direction::S:
      x -= 1;
      break;
    case Direction::SW:
      x -= 1;
      y -= 1;
      break;
    case Direction::W:
      y -= 1;
      break;
    case Direction::NW:
      x += 1;
      y -= 1;
      break;
  }

  return Point((x + width) % width, (y + height) % height);
}

int sum_neighbour(int x, int y) {
  int sum = 0;
  Point origin = Point(x, y);
  Point adjacent;
  // check each direction
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(origin, (Direction)i);
    if (generation[adjacent.first * width + adjacent.second]) {
      sum += 1;
    }
  }
  return sum;
}

void tick(void) {
#pragma omp parallel for
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int neighbours = sum_neighbour(x, y);
      if (neighbours == 3) {
        // if 3 live neighbours exactly, cell lives
        next_generation[x * width + y] = 1;
      } else if (neighbours == 2) {
        // if 2 live neighbours exactly, cell maintains status
        next_generation[x * width + y] = generation[x * width + y];
      } else {
        // otherwise else cell dies
        next_generation[x * width + y] = 0;
      }
    }
  }
  // swap currently active generation
  std::swap(generation, next_generation);
}

void sync(void) {
  MPI_Barrier(MPI_COMM_WORLD);
  for (int i = 0; i <) }

void run(void) {
  metric.start(Measure::Run);

  for (int i = 0; i < ticks; i++) {
    msg();

    MPI_Barrier(MPI_COMM_WORLD);
    tick();
    sync();
#ifdef VISUAL
    print();
    usleep(100000);
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

  MPI_Scatterv(init_generation, sendcounts, displs, MPI_INT, generation,
               sendcounts[id], MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &worker_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  upId = (id - 1 + worker_count) % worker_count;
  downId = (id + 1) % worker_count;

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
  size = height * width;

  metric.start(Measure::Total);

  if (id == 0) {
    init();
    print();
  }

  scatter();

  if (id == 0) {
    print_arr(sendcounts);
    print_arr(displs);
  }

  if (true) {
    return;
  }

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

  delete[] init_generation;
  delete[] generation;
  delete[] next_generation;
  delete[] up;
  delete[] down;
  delete[] sendcounts;
  delete[] displs;
  MPI_Finalize();
  return 0;
}
