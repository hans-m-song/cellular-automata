#include "metric.hpp"

#include <omp.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

Metric::Metric() {}

void Metric::start(std::string label) {
  auto now = omp_get_wtime();
  auto duration = time_range(now, now);
  auto existing = times.find(label);
  if (existing == times.end()) {
    times.insert(named_time_range(label, duration));
  } else {
    existing->second.first = now;
  }
}

void Metric::stop(std::string label) {
  auto now = omp_get_wtime();
  auto duration = time_range(now, now);
  auto existing = times.find(label);
  if (existing == times.end()) {
    times.insert(named_time_range(label, duration));
  } else {
    existing->second.second = now;
  }
}

time_range Metric::get_time_range(std::string label) {
  return times.find(label)->second;
}

double Metric::duration(std::string label) {
  auto data = get_time_range(label);
  return data.second - data.first;
}

void Metric::log() {
  for (auto i = times.begin(); i != times.end(); i++) {
    auto total = duration(i->first);
    std::cout << std::setw(8) << i->first << ":" << std::setw(12) << total
              << "\n";
  }
}

std::string Metric::csv(void) {
  std::string result = "";
  auto end = std::prev(times.end(), 1);
  for (auto i = times.begin(); i != times.end(); i++) {
    auto total = duration(i->first);
    result.append(std::to_string(total));
    if (i != end) {
      result.append(",");
    }
  }

  return result;
}
