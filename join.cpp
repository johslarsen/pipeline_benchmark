#include "join.hpp"

#include <cstring>

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
    auto output = std::move(_buffer);
    _buffer.reserve(_slice_count * input.size());
    return output;
  } else {
    return {};
  }
}

malloc_span
Join::into_malloc(const malloc_span& input) {
  if (_malloc.first == nullptr) {
    _malloc = make_span_malloc(_slice_count * input.second);
  }
  std::memcpy(_malloc.first.get() + _offset, input.first.get(), input.second);
  _offset += input.second;

  if (++_current == _slice_count) {
    _current = 0;
    _offset = 0;
    return std::move(_malloc);
  } else {
    return empty_malloc_span();
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
  input.clear();
  if (++_current == _slice_count) {
    _current = 0;
    std::swap(_buffer, input);
  }
}
