#include "../append.hpp"
#include "../args.hpp"
#include "../input.hpp"
#include "../join.hpp"
#include "../prepend.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);
  Append append({'b', 'a', 'r'});
  Prepend prepend({'f', 'o', 'o'});
  Join join(args.slice_count);

  immer::flex_vector<uint8_t> last;
  size_t last_size = -1;
  for (size_t i = 0; i < args.iterations; i++) {
    last = join.into_flex(prepend.into_flex(append.into_flex(input.into_flex(i))));
    if (last.size() > 0) last_size = last.size();
  }
  return last_size != args.slice_count * (3 + args.record_size + 3);
}
