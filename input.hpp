#pragma once
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include "malloc_span.hpp"

class Input {
 public:
  Input(size_t buffer_size) : _buffer(buffer_size) {}

  std::span<uint8_t> raw_static();

  std::span<uint8_t> into_static(uint8_t filler);
  std::vector<uint8_t> into_copy(uint8_t filler);
  malloc_span into_malloc(uint8_t filler);
  std::vector<uint8_t> into_moved(uint8_t filler, std::vector<uint8_t>&& buffer);
  void into_ref(uint8_t filler, std::vector<uint8_t>& buffer);

 private:
  std::vector<uint8_t> _buffer;
};
