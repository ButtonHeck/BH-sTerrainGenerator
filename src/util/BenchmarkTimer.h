#ifndef BENCHMARKTIMER_H
#define BENCHMARKTIMER_H
#include <chrono>
#include <fstream>
#include <map>
#include <string.h>
#include <algorithm>
#include <iomanip>
#include "util/Settings.h"
#include "util/Logger.h"

class BenchmarkTimer
{
public:
  BenchmarkTimer(const std::string& text, bool perFrameBenchmark);
  virtual ~BenchmarkTimer();
  static void finish(unsigned int updateCount);
  static void printBenchmarksPerApp(unsigned int updateCount);
  static void printBenchmarksPerFrame(unsigned int updateCount, unsigned int ups);
  static void clearBenchmarksPerFrameValues();
private:
  static std::ofstream perFrameLog, perAppLog;
  static bool outputCreated;
  static std::map<std::string, unsigned long> benchmarksTimers;
  static std::map<std::string, int> benchmarksInvocations;
  static std::map<std::string, float> appBenchmarks;
  static constexpr int FORMAT_VALUE_ASCII = 45, BENCH_NAME_MAX_LENGTH = 40;
  std::string benchmark;
  bool perFrame;
  decltype(std::chrono::high_resolution_clock::now()) startTime = std::chrono::high_resolution_clock::now();
  decltype(startTime) endTime = startTime;
};

#endif // BENCHMARKTIMER_H
