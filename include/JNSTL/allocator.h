#ifndef JNSTL_ALLOCATOR_H_
#define JNSTL_ALLOCATOR_H_

#include <cstdlib>

#include "JNSTL/bits/config.h"

namespace jnstl {

class allocator {
 public:
  void* allocate(size_t n);
  void* allocate(size_t n, size_t alignment, size_t offset);

  void deallocate(void* p, size_t n);
};

bool operator==(const allocator& lhs, const allocator& rhs);
bool operator!=(const allocator& lhs, const allocator& rhs);

inline void* allocator::allocate(size_t n) {
  if (n > 0)
    return ::malloc(n);
  else
    return nullptr;
}

inline void allocator::deallocate(void* p, size_t) {
  free(p);
}

inline bool operator==(const allocator&, const allocator&) {
  return true;
}

inline bool operator!=(const allocator&, const allocator&) {
  return false;
}

}  // namespace jnstl

#endif /* JNSTL_ALLOCATOR_H_ */
