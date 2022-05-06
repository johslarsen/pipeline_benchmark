#include "../append.hpp"
#include "../args.hpp"
#include "../input.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);
  Append append({'b', 'a', 'r'});

  std::span<uint8_t> last;
  for (size_t i = 0; i < args.iterations; i++) {
    last = append.into_static(input.into_static(i));
  }
  return last.size() != args.record_size + 3;
}
