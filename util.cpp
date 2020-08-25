#include "util.hpp"
#include <cstdlib>

int random(int lower, int upper) {
  return lower + (std::rand() % (upper - lower + 1));
}