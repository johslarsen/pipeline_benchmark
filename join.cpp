#include "join.hpp"

std::span<uint8_t>
Join::into_static(const std::span<uint8_t>& input) {
  if (_current == _slice_count) {
    _buffer.clear();
    _current = 0;
  }
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  if (++_current == _slice_count) {
    return {_buffer.data(), _buffer.size()};
  } else {
    return {};
  }
}

std::vector<uint8_t>
Join::into_copy(const std::vector<uint8_t>& input) {
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  if (++_current == _slice_count) {
    _current = 0;
    return std::move(_buffer);
  } else {
    return {};
  }
}

std::vector<uint8_t>
Join::into_moved(std::vector<uint8_t>&& input) {
  if (_current == 0) {
    std::swap(_buffer, input);
  } else {
    _buffer.insert(_buffer.end(), input.begin(), input.end());
  }
  input.clear();
  if (++_current == _slice_count) {
    _current = 0;
    std::swap(input, _buffer);
  }
  return input;
}

void Join::into_ref(std::vector<uint8_t>& input) {
  _buffer.insert(_buffer.end(), input.begin(), input.end());
  if (++_current == _slice_count) {
    _current = 0;
    std::swap(_buffer, input);
    _buffer.clear();
  }
}
