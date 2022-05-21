#include "../args.hpp"
#include "../input.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);

  std::vector<uint8_t> last;
  for (size_t i = 0; i < args.iterations; i++) {
    last = input.into_moved(i, std::move(last));
  }
  return last.size() != args.record_size.back();
}
