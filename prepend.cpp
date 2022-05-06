#include "prepend.hpp"

std::span<uint8_t>
Prepend::into_static(const std::span<uint8_t>& input) {
  _buffer.clear();
  _buffer.insert(_buffer.end(), _bytes.begin(), _bytes.end());
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  return {_buffer.data(), _buffer.size()};
}

std::vector<uint8_t>
Prepend::into_copy(const std::vector<uint8_t>& input) {
  std::vector<uint8_t> copy(input);
  copy.insert(copy.begin(), _bytes.begin(), _bytes.end());
  return copy;
}

std::vector<uint8_t>
Prepend::into_moved(std::vector<uint8_t>&& input) {
  input.insert(input.begin(), _bytes.begin(), _bytes.end());
  return input;
}

void Prepend::into_ref(std::vector<uint8_t>& input) {
  input.insert(input.begin(), _bytes.begin(), _bytes.end());
}
