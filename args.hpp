#pragma once
#include <cstddef>
#include <iostream>
#include <numeric>
#include <vector>

struct Args {
  size_t iterations;
  std::vector<size_t> record_size;
  size_t slice_count;

  static Args from_env() {
    Args args{
        std::stoul(env_or_die("ITERATIONS")),
        split(env_or_die("RECORD_SIZE")),
        std::stoul(env_or_die("SLICE_COUNT")),
    };
    auto nsize = args.record_size.size();
    if (nsize == 0 || (nsize&(nsize-1)) != 0) {
      std::cerr << "RECORD_SIZE must be a power-of-2 sized CSV list" << std::endl;
      std::exit(1);
    }
    if (args.slice_count % nsize != 0) {
      std::cerr << "Number of RECORD_SIZEs must divide SLICE_COUNT" << std::endl;
      std::exit(1);
    }
    return args;
  }

  size_t record_size_total() {
    return std::reduce(record_size.begin(), record_size.end(), 0);
  }
  size_t joined_total() {
    return slice_count * record_size_total() / record_size.size();
  }

 private:
  static std::string env_or_die(const char* name) {
    if (const char* value = std::getenv(name); value != nullptr) {
      return value;
    }
    std::cerr << "Missing environment " << name << std::endl;
    std::exit(1);
  }

  static std::vector<size_t> split(std::string_view csv) {
    std::vector<size_t> numbers;
    for (auto end = std::string::npos;
         (end = csv.find(",")) != std::string::npos;
         csv.remove_prefix(end + 1)) {
      numbers.emplace_back(std::stoul(std::string(csv.substr(0, end))));
    }
    numbers.emplace_back(std::stoul(std::string(csv)));
    return numbers;
  }
};
