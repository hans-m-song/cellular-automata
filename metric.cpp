#include "metric.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

Metric::Metric() {}

void Metric::start(std::string label) {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = time_range(now, now);
  auto existing = times.find(label);
  if (existing == times.end()) {
    times.insert(named_time_range(label, duration));
  } else {
    existing->second.first = now;
  }
}

void Metric::stop(std::string label) {
  auto now = std::chrono::high_resolution_clock::now();
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

microseconds Metric::duration(std::string label) {
  auto data = get_time_range(label);
  return std::chrono::duration_cast<microseconds>(data.second - data.first);
}

void Metric::log() {
  for (auto i = times.begin(); i != times.end(); i++) {
    auto total = duration(i->first);
    std::cout << std::setw(10) << i->first << ":" << std::setw(10)
              << total.count() << "\n";
  }
}

std::string Metric::csv(void) {
  std::string result = "";
  auto end = std::prev(times.end(), 1);
  for (auto i = times.begin(); i != times.end(); i++) {
    auto total = duration(i->first);
    result.append(std::to_string(total.count()));
    if (i != end) {
      result.append(",");
    }
  }

  return result;
}
