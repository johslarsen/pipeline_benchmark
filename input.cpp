#include "input.hpp"

#include <cstring>

std::span<uint8_t>
Input::raw_static() {
  return {_buffer.data(), _buffer.size()};
}

std::span<uint8_t>
Input::into_static(uint8_t filler) {
  std::memset(_buffer.data(), filler, _buffer.size());
  return {_buffer.data(), _buffer.size()};
}

std::vector<uint8_t>
Input::into_copy(uint8_t filler) {
  return std::vector<uint8_t>(_buffer.size(), filler);
}

std::vector<uint8_t>
Input::into_moved(uint8_t filler, std::vector<uint8_t>&& buffer) {
  buffer.resize(_buffer.size());
  std::memset(buffer.data(), filler, buffer.size());
  return buffer;
}

void Input::into_ref(uint8_t filler, std::vector<uint8_t>& buffer) {
  buffer.resize(_buffer.size());
  std::memset(_buffer.data(), filler, _buffer.size());
}

immer::flex_vector<uint8_t>
Input::into_flex(uint8_t filler) {
  return immer::flex_vector<uint8_t>(_buffer.size(), filler);
}
