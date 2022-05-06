#pragma once
#include <cstddef>
#include <iostream>

struct Args {
  size_t iterations;
  size_t record_size;
  size_t slice_count;

  static Args from_env() {
    return {
        std::stoul(env_or_die("ITERATIONS")),
        std::stoul(env_or_die("RECORD_SIZE")),
        std::stoul(env_or_die("SLICE_COUNT")),
    };
  }

 private:
  static std::string env_or_die(const char* name) {
    if (const char* value = std::getenv(name); value != nullptr) {
      return value;
    }
    std::cerr << "Missing environment " << name << std::endl;
    std::exit(1);
  }
};
