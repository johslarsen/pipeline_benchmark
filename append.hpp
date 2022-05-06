#pragma once
#include <cstdint>
#include <immer/flex_vector.hpp>
#include <span>
#include <vector>

class Append {
 public:
  Append(std::vector<uint8_t> bytes) : _bytes(std::move(bytes)),
                                       _flex_bytes(_bytes.begin(), _bytes.end()) {}

  std::span<uint8_t> into_static(const std::span<uint8_t>& input);
  std::vector<uint8_t> into_copy(const std::vector<uint8_t>& input);
  std::vector<uint8_t> into_moved(std::vector<uint8_t>&& input);
  void into_ref(std::vector<uint8_t>& input);

  immer::flex_vector<uint8_t> into_flex(const immer::flex_vector<uint8_t>& input);

 private:
  std::vector<uint8_t> _bytes, _buffer;
  immer::flex_vector<uint8_t> _flex_bytes;
};
