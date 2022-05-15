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

  malloc_span last = empty_malloc_span();
  size_t last_size = -1;
  for (size_t i = 0; i < args.iterations; i++) {
    last = join.into_malloc(prepend.into_malloc(append.into_malloc(input.into_malloc(i))));
    if (last.first != nullptr) last_size = last.second;
  }
  return last_size != args.slice_count * (3 + args.record_size + 3);
}
