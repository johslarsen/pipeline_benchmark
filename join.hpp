#pragma once
#include <cstdint>
#include <span>
#include <vector>

#include "malloc_span.hpp"

class Join {
 public:
  Join(size_t slice_count) : _slice_count(slice_count), _malloc(empty_malloc_span()) {}

  std::span<uint8_t> into_static(const std::span<uint8_t>& input);
  std::vector<uint8_t> into_copy(const std::vector<uint8_t>& input);
  malloc_span into_malloc(const malloc_span& input);
  std::vector<uint8_t> into_moved(std::vector<uint8_t>&& input);
  void into_ref(std::vector<uint8_t>& input);

 private:
  size_t _slice_count;
  size_t _current = 0;
  std::vector<uint8_t> _buffer;
  malloc_span _malloc;
  size_t _offset = 0;
};
