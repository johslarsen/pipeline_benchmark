#include "../args.hpp"
#include "../input.hpp"
#include "../join.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);
  Join join(args.slice_count);

  std::span<uint8_t> last;
  size_t last_size = -1;
  for (size_t i = 0; i < args.iterations; i++) {
    last = join.into_static(input.into_static(i));
    if (last.size() > 0) last_size = last.size();
  }
  return last_size != args.joined_total();
}
