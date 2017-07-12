#ifndef JNSTL_CONSTRUCT_H_
#define JNSTL_CONSTRUCT_H_


#include "./config.h"
#include "JNSTL/iterator.h"

namespace jnstl {

template<typename T1, typename T2>
inline void
Construct(T1* p, const T2&& value) {
  ::new(static_cast<void*>(p)) T1(LIB::move(value));
}

template<typename T1, typename T2>
inline void
Construct(T1* p, const T2& value) {
  ::new(static_cast<void*>(p)) T1(value);
}

template <typename T>
inline void
Destruct(T* p) {
  p->~T();
}

template <typename ForwardIterator>
inline void
destruct_impl(ForwardIterator /*first*/, ForwardIterator /*last*/,
                          std::true_type) {
  // Do nothing. Trivial destructor.
}

template <typename ForwardIterator>
inline void
destruct_impl(ForwardIterator first, ForwardIterator last,
                          std::false_type) {
  for (; first != last; ++first)
    jnstl::Destruct(LIB::addressof(*first));
}

template <typename ForwardIterator>
inline void
Destruct(ForwardIterator first, ForwardIterator last) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;

  destruct_impl(first, last, std::is_trivially_destructible<value_type>());
}
}  // namespace jnstl

#endif /* JNSTL_CONSTRUCT_H_ */
