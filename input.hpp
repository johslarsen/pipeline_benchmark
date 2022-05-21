#pragma once
#include <algorithm>
#include <cstdint>
#include <span>
#include <vector>

class Input {
 public:
  Input(std::vector<size_t> buffer_sizes) : _sizes(std::move(buffer_sizes)),
                                            _size_mask(_sizes.size() - 1) {
    _buffer.reserve(*std::max_element(_sizes.begin(), _sizes.end()));
  }

  std::span<uint8_t> raw_static(uint8_t filler);

  std::span<uint8_t> into_static(uint8_t filler);
  std::vector<uint8_t> into_copy(uint8_t filler);
  std::vector<uint8_t> into_moved(uint8_t filler, std::vector<uint8_t>&& buffer);
  void into_ref(uint8_t filler, std::vector<uint8_t>& buffer);

 private:
  std::vector<size_t> _sizes;
  size_t _size_mask;
  std::vector<uint8_t> _buffer;
};
