#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>

// bump allocator with alignment for normal types
// buffer base address does not have guaranteed alignment
class BumpAllocator {
public:
  BumpAllocator(std::size_t n)
      : capacity_{n}, buffer_{std::make_unique<std::byte[]>(n)}, loc_{0} {}
  template <typename T> T *allocate(std::size_t count = 1) {
    std::size_t bytes = count * sizeof(T);
    std::size_t next_loc = loc_ + bytes;
    if (next_loc > capacity_) {
      return nullptr;
    }

    std::byte *block = &buffer_[loc_];
    std::size_t a = alignof(T);
    std::size_t alligned_loc = (loc_ + a - 1) & ~(a - 1);
    loc_ = next_loc;
    return static_cast<T *>(static_cast<void *>(block));
  }

private:
  std::size_t capacity_{};
  std::unique_ptr<std::byte[]> buffer_{};
  std::size_t loc_{};
};

void test_good_allo() {
  BumpAllocator bmp{200};
  std::array<std::byte *, 4> tmp{};
  int insert_count{4};

  for (int i{}; i < insert_count; ++i) {
    tmp[i] = bmp.allocate<std::byte>(50);
  }

  for (auto const &ele : tmp) {
    std::cout << ele << '\n';
  }

  // check every ptr is non-null
  for (auto const &ele : tmp) {
    assert(ele != nullptr);
  }

  // correct_stride
  assert(tmp[1] - tmp[0] == 50);
  assert(tmp[2] - tmp[1] == 50);
  assert(tmp[3] - tmp[2] == 50);

  // set chunk values
  std::memset(tmp[0], 0xAA, 50);
  std::memset(tmp[1], 0xBB, 50);
  std::memset(tmp[2], 0xCC, 50);
  std::memset(tmp[3], 0xDD, 50);

  std::array<std::byte, 4> fill_values{std::byte{0xAA}, std::byte{0xBB},
                                       std::byte{0xCC}, std::byte{0xDD}};

  for (int k = 0; k < insert_count; ++k) {
    for (int j = 0; j < 50; ++j) {
      assert(tmp[k][j] == fill_values[k]);
    }
  }
}

void test_overflow_allo() {
  BumpAllocator bmp{50};
  std::byte *first = bmp.allocate<std::byte>(50);
  assert(first != nullptr);
  std::byte *overflow = bmp.allocate<std::byte>(1);
  assert(overflow == nullptr);
}

void test_overflow_allo_int() {
  BumpAllocator bmp{200};
  int *first = bmp.allocate<int>(50);
  assert(first != nullptr);
  int *overflow = bmp.allocate<int>(1);
  assert(overflow == nullptr);
}

void test_overflow_allo_short() {
  BumpAllocator bmp{100};
  short *first = bmp.allocate<short>(50);
  assert(first != nullptr);
  short *overflow = bmp.allocate<short>(1);
  assert(overflow == nullptr);
}

int main() {
  test_good_allo();
  test_overflow_allo();
  test_overflow_allo_int();
  test_overflow_allo_short();
}
