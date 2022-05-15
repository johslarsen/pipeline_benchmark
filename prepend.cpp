#include "prepend.hpp"

#include <cstring>

std::span<uint8_t>
Prepend::into_static(const std::span<uint8_t>& input) {
  _buffer.clear();
  _buffer.insert(_buffer.end(), _bytes.begin(), _bytes.end());
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  return {_buffer.data(), _buffer.size()};
}

std::vector<uint8_t>
Prepend::into_copy(const std::vector<uint8_t>& input) {
  std::vector<uint8_t> copy;
  copy.reserve(_bytes.size() + input.size());

  copy.insert(copy.begin(), _bytes.begin(), _bytes.end());
  copy.insert(copy.begin(), input.begin(), input.end());
  return copy;
}

malloc_span
Prepend::into_malloc(const malloc_span& input) {
  auto output = make_span_malloc(_bytes.size() + input.second);
  std::memcpy(output.first.get(), _bytes.data(), _bytes.size());
  std::memcpy(output.first.get() + _bytes.size(), input.first.get(), input.second);
  return output;
}

std::vector<uint8_t>
Prepend::into_moved(std::vector<uint8_t>&& input) {
  input.insert(input.begin(), _bytes.begin(), _bytes.end());
  return input;
}

void Prepend::into_ref(std::vector<uint8_t>& input) {
  input.insert(input.begin(), _bytes.begin(), _bytes.end());
}
