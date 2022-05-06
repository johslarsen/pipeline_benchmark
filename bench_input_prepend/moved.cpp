#include "../args.hpp"
#include "../input.hpp"
#include "../prepend.hpp"
int main() {
  auto args = Args::from_env();
  Input input(args.record_size);
  Prepend prepend({'f', 'o', 'o'});

  std::vector<uint8_t> last;
  for (size_t i = 0; i < args.iterations; i++) {
    last = prepend.into_moved(input.into_moved(i, std::move(last)));
  }
  return last.size() != 3 + args.record_size;
}
