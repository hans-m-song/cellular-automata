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
  int size = width * height;

  int* init_generation = new int[size];
  
  for (int i = 0; i < size; i++) {
    init_generation[i] = 0;
  }
  
  int initial_cells = (int)(initial_density * size);
  
  int point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell(init_generation, width, height);
    init_generation[point] = 1;
  }

  // allocate memory on device
  checkError(cudaMalloc((void**)&generation, size));
  checkError(cudaMalloc((void**)&next_generation, size));

  // copy over empty data
  checkError(cudaMemcpy(generation, init_generation, size, cudaMemcpyHostToDevice));
  // don't need to initialize next_generation since it'll be overwritten during tick
  delete[] init_generation;
}

void deinit() {
  cudaFree(generation);
  cudaFree(next_generation);
}

__device__
int apply_direction(int width, int height, int i, int j, Direction direction) {
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
int sum_neighbour(int* generation, int width, int height, int x, int y) {
  int sum = 0;
  int adjacent;
  // check each direction
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(width, height, x, y, (Direction)i);
    if (generation[adjacent]) {
      sum += 1;
    }
  }
  return sum;
}

__device__
void tick(int* generation, int* next_generation, int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x_stride = blockDim.x * gridDim.x;
  int y_stride = blockDim.y * gridDim.y;

  for (int i = x; i < width; i += x_stride) {
    for (int j = y; j < height; j += y_stride) {
      int neighbours = sum_neighbour(generation, width, height, i, j);
      if (neighbours == 3) {
        // if 3 live neighbours exactly, cell lives
        next_generation[i * width + j] = 1;
      } else if (neighbours == 2) {
        // if 2 live neighbours exactly, cell maintains status
        next_generation[i * width + j] = generation[i * width + j];
      } else {
        // otherwise else cell dies
        next_generation[i * width + j] = 0;
      }
    }
  }
  // swap currently active generation
  auto temp = generation;
  generation = next_generation;
  next_generation = temp;
}

__device__
void print(int* generation, int width, int height) {
#ifdef VISUAL
#ifndef DEBUG
  printf("\e[1;1H\e[2J");
#endif
#endif
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      printf(generation[x * width + y] ? "o " : ". ");
    }
    printf("\n");
  }
}

__global__
void run(int* generation, int* next_generation, int width, int height, int ticks) {
  for (int i = 0; i < ticks; i++) {
    __syncthreads(); // ensure generation is completely generated
    tick(generation, next_generation, width, height);
#ifdef VISUAL
    print(generation, width, height);
#endif
  }
}

Metric run_grid(int width, int height, double initial_density, int ticks) {
  metric.start(Measure::Total);
  
  metric.start(Measure::Init);
  init(width, height, initial_density);
  metric.stop(Measure::Init);

  metric.start(Measure::Run);
  run<<<1, 1>>>(generation, next_generation, width, height, ticks);
  metric.stop(Measure::Run);
  
  metric.stop(Measure::Total);

  deinit();

  return metric;
}
