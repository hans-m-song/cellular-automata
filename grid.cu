#include "grid.hpp"

#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "metric.hpp"
#include "util.hpp"

int width;
int height;
int size;
int* generation;
int* d_generation
int* next_generation;
int* d_next_generation
Metric metric;

void checkError(cudaError_t e) {
   if (e != cudaSuccess) {
      std::cerr << "CUDA error: " << int(e) << ": " 
                << cudaGetErrorString(e) << "\n";
      abort();
   }
}

void init(double initial_density) {
  metric.start(Measure::Total);
  metric.start(Measure::Init);
  
  generation = allocate_space();
  next_generation = allocate_space();
  
  int initial_cells = (int)(initial_density * size);
  
  Point point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell();
    generation[point.first * width + point.second] = 1;
  }
  
  // allocate memory on device
  checkError(cudaMalloc((void**)&d_generation, size));
  checkError(cudaMalloc((void**)&d_next_generation, size));

  // copy over empty data
  checkError(cudaMemcpy(d_generation, generation, size, cudaMemcpyHostToDevice));
  checkError(cudaMemcpy(d_next_generation, next_generation, size, cudaMemcpyHostToDevice));

  metric.stop(Measure::Init);
}

void deinit() {
  delete[] generation;
  delete[] next_generation;
  cudaFree(d_generation);
  cudaFree(d_next_generation);
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

__device__
int sum_neighbour(int x, int y) {
  int sum = 0;
  Point origin = Point(x, y);
  Point adjacent;
  // check each direction
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(origin, (Direction)i);
    if (d_generation[adjacent.first * width + adjacent.second]) {
      sum += 1;
    }
  }
  return sum;
}

void print(void) {
#ifndef DEBUG
  std::cout << "\e[1;1H\e[2J";
#endif

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (d_generation[x * width + y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

__global__
void run(int ticks) {
  for (int i = 0; i < ticks; i++) {
    __syncthreads(); // ensure generation is completely generated
    tick();
  }
}

__global__
void tick(void) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x_stride = blockDim.x * gridDim.x;
  int y_stride = blockDim.y * gridDim.y;

  for (int i = x; i < N; i += x_stride) {
    for (int j = y; j < N; j += y_stride) {
      int neighbours = sum_neighbour(i, j);
      if (neighbours == 3) {
        // if 3 live neighbours exactly, cell lives
        d_next_generation[i * width + j] = 1;
      } else if (neighbours == 2) {
        // if 2 live neighbours exactly, cell maintains status
        d_next_generation[i * width + j] = d_generation[i * width + j];
      } else {
        // otherwise else cell dies
        d_next_generation[i * width + j] = 0;
      }
    }
  }
  // swap currently active generation
  std::swap(generation, next_generation);
}

__device__
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

Metric run_grid(int h_width, int h_height, double initial_density, int ticks) {
  width = h_width;
  height = h_height;
  size = width * height;

  init(initial_density);

  metric.start(Measure::Run);
  run<<<1, 1>>>(ticks);
  metric.stop(Measure::Run);
  metric.stop(Measure::Total);
  
  deinit();

  return metric;
}
