#pragma once
#include <cstdint>
#include <immer/flex_vector.hpp>
#include <span>
#include <vector>

class Input {
 public:
  Input(size_t buffer_size) : _buffer(buffer_size) {}

  std::span<uint8_t> raw_static();

  std::span<uint8_t> into_static(uint8_t filler);
  std::vector<uint8_t> into_copy(uint8_t filler);
  std::vector<uint8_t> into_moved(uint8_t filler, std::vector<uint8_t>&& buffer);
  void into_ref(uint8_t filler, std::vector<uint8_t>& buffer);

  immer::flex_vector<uint8_t> into_flex(uint8_t filler);

 private:
  std::vector<uint8_t> _buffer;
};
