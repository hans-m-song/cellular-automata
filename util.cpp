#include "util.hpp"
#include <cstdlib>
#include <ctime>
#include <random>

int random(int lower, int upper) {
  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> uniform_dist(lower, upper);
  return uniform_dist(e1);
}