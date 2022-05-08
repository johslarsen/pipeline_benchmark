#include "append.hpp"

std::span<uint8_t>
Append::into_static(const std::span<uint8_t>& input) {
  _buffer.clear();
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  _buffer.insert(_buffer.end(), _bytes.begin(), _bytes.end());
  return {_buffer.data(), _buffer.size()};
}

std::vector<uint8_t>
Append::into_copy(const std::vector<uint8_t>& input) {
  std::vector<uint8_t> output;
  output.reserve(input.size() + _bytes.size());
  output.insert(output.end(), input.begin(), input.end());
  output.insert(output.end(), _bytes.begin(), _bytes.end());
  return output;
}

std::vector<uint8_t>
Append::into_moved(std::vector<uint8_t>&& input) {
  input.insert(input.end(), _bytes.begin(), _bytes.end());
  return input;
}

void Append::into_ref(std::vector<uint8_t>& input) {
  input.insert(input.end(), _bytes.begin(), _bytes.end());
}
