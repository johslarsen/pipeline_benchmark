#include "append.hpp"

#include <cstring>

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

malloc_span
Append::into_malloc(const malloc_span& input) {
  auto output = make_span_malloc(input.second + _bytes.size());
  std::memcpy(output.first.get(), input.first.get(), input.second);
  std::memcpy(output.first.get() + input.second, _bytes.data(), _bytes.size());
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
