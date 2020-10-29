#ifndef METRIC_H
#define METRIC_H

#include <chrono>
#include <map>
#include <string>

typedef std::pair<double, double> time_range;
typedef std::pair<std::string, time_range> named_time_range;
typedef std::map<std::string, time_range> time_record;

typedef std::pair<std::string, int> named_count;
typedef std::map<std::string, int> count_record;

namespace Measure {
  const std::string Init = "Init";
  const std::string Total = "Total";
  const std::string Run = "Run";
};  // namespace Measure

class Metric {
  time_record times;
  count_record counts;

 public:
  Metric();
  void start(std::string label);
  void stop(std::string label);
  time_range get_time_range(std::string label);
  double duration(std::string label);
  void log(void);
  std::string csv(void);
};

#endif
