#pragma once
#include <cstdint>
#include <immer/flex_vector.hpp>
#include <span>
#include <vector>

class Join {
 public:
  Join(size_t slice_count) : _slice_count(slice_count) {}

  std::span<uint8_t> into_static(const std::span<uint8_t>& input);
  std::vector<uint8_t> into_copy(const std::vector<uint8_t>& input);
  std::vector<uint8_t> into_moved(std::vector<uint8_t>&& input);
  void into_ref(std::vector<uint8_t>& input);

  immer::flex_vector<uint8_t> into_flex(const immer::flex_vector<uint8_t>& input);

 private:
  size_t _slice_count;
  size_t _current = 0;
  std::vector<uint8_t> _buffer;
  immer::flex_vector<uint8_t> _flex_buffer;
};
