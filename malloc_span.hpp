#pragma once
#include <memory>

using unique_malloc = std::unique_ptr<uint8_t, decltype(std::free)*>;
using malloc_span = std::pair<unique_malloc, size_t>;

inline malloc_span empty_malloc_span() {
  return {unique_malloc(nullptr, std::free), 0};
}

inline malloc_span make_span_malloc(size_t size) {
  return {unique_malloc(reinterpret_cast<uint8_t*>(std::malloc(size)), std::free), size};
}
