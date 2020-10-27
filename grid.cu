#include "grid.hpp"

#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "metric.hpp"
#include "util.hpp"

Metric metric;
int* generation;
int* next_generation;

int* d_generation;
int* d_next_generation;

int h_width;
int h_height;

__device__ int width;
__device__ int height;

void checkError(cudaError_t e) {
  if (e != cudaSuccess) {
    std::cerr << "CUDA error: " << int(e) << ": " 
              << cudaGetErrorString(e) << "\n";
    abort();
  }
}

int empty_cell(int* generation, int width, int height) {
  int x, y, point;
  for (int i = 0; i < width * height; i++) {
    x = random(0, width - 1);
    y = random(0, height - 1);
    point = x * width + y;
    if (!generation[point]) {
      return point;
    }
  }

  return 0;
}

void init(int width, int height, double initial_density) {
  checkError(cudaMemcpyToSymbol("width", &width, sizeof(int)));
  checkError(cudaMemcpyToSymbol("height", &height, sizeof(int)));

  int size = width * height;

  generation = new int[size];
  next_generation = new int[size];
  
  for (int i = 0; i < size; i++) {
    generation[i] = 0;
  }
  
  int initial_cells = (int)(initial_density * size);
  
  int point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell(generation, width, height);
    generation[point] = 1;
  }
  
  // allocate memory on device
  checkError(cudaMalloc((void**)&d_generation, size));
  checkError(cudaMalloc((void**)&d_next_generation, size));

  // copy over empty data
  checkError(cudaMemcpy(d_generation, generation, size, cudaMemcpyHostToDevice));
  // don't need to initialize next_generation since it'll be overwritten during tick
}

void deinit() {
  delete[] generation;
  delete[] next_generation;
  cudaFree(d_generation);
  cudaFree(d_next_generation);
}

__device__
int apply_direction(int i, int j, Direction direction) {
  int x = i;
  int y = j;
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

  return ((x + width) % width) * width + ((y + height) % height);
}

__device__
int sum_neighbour(int* d_generation, int x, int y) {
  int sum = 0;
  int adjacent;
  // check each direction
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(x, y, (Direction)i);
    if (d_generation[adjacent]) {
      sum += 1;
    }
  }
  return sum;
}

__device__
void tick(int* d_generation, int* d_next_generation) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x_stride = blockDim.x * gridDim.x;
  int y_stride = blockDim.y * gridDim.y;

  for (int i = x; i < N; i += x_stride) {
    for (int j = y; j < N; j += y_stride) {
      int neighbours = sum_neighbour(d_generation, i, j);
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
  auto temp = d_generation;
  d_generation = d_next_generation;
  d_next_generation = temp;
}

__global__
void run(int ticks, int* d_generation, int* d_next_generation) {
  for (int i = 0; i < ticks; i++) {
    __syncthreads(); // ensure generation is completely generated
    tick(d_generation, d_next_generation);
  }
}

void print(void) {
#ifndef DEBUG
  std::cout << "\e[1;1H\e[2J";
#endif

  for (int x = 0; x < h_width; x++) {
    for (int y = 0; y < h_height; y++) {
      std::cout << (d_generation[x * h_width + y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

Metric run_grid(int width, int height, double initial_density, int ticks) {
  h_width = width;
  h_height = height;
  metric.start(Measure::Total);

  metric.start(Measure::Init);
  init(width, height, initial_density);
  metric.stop(Measure::Init);

  metric.start(Measure::Run);
  run<<<1, 1>>>(ticks, d_generation, d_next_generation);
  metric.stop(Measure::Run);
  
  metric.stop(Measure::Total);

  deinit();

  return metric;
}
