#pragma once
#include <cstdint>
#include <span>
#include <vector>

#include "malloc_span.hpp"

class Append {
 public:
  Append(std::vector<uint8_t> bytes) : _bytes(std::move(bytes)) {}

  std::span<uint8_t> into_static(const std::span<uint8_t>& input);
  std::vector<uint8_t> into_copy(const std::vector<uint8_t>& input);
  malloc_span into_malloc(const malloc_span& input);
  std::vector<uint8_t> into_moved(std::vector<uint8_t>&& input);
  void into_ref(std::vector<uint8_t>& input);

 private:
  std::vector<uint8_t> _bytes, _buffer;
};
