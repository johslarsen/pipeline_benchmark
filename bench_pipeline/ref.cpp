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

  std::vector<uint8_t> last;
  size_t last_size = -1;
  for (size_t i = 0; i < args.iterations; i++) {
    input.into_ref(i, last);
    append.into_ref(last);
    prepend.into_ref(last);
    join.into_ref(last);
    if (last.size() > 0) last_size = last.size();
  }
  return last_size != args.joined_total() + args.slice_count * 2 * 3;
}
