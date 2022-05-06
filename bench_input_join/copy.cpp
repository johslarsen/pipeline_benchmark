#include "../args.hpp"
#include "../input.hpp"
#include "../join.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);
  Join join(args.slice_count);

  std::vector<uint8_t> last;
  size_t last_size = -1;
  for (size_t i = 0; i < args.iterations; i++) {
    last = join.into_copy(input.into_copy(i));
    if (last.size() > 0) last_size = last.size();
  }
  return last_size != args.slice_count * (args.record_size);
}
