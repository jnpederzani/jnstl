#ifndef JNSTL_TEMPBUF_H_
#define JNSTL_TEMPBUF_H_


#include "./config.h"
#include "./construct.h"
#include "JNSTL/utility.h"

namespace jnstl {

template <typename T>
jnstl::pair<T*, ptrdiff_t>
get_temporary_buffer(ptrdiff_t len) {
  const ptrdiff_t max = std::numeric_limits<ptrdiff_t>::max() / sizeof(T);

  if (len > max)
    len = max;

  while (len > 0) {
    T* tmp = static_cast<T*>(malloc(len * sizeof(T)));

    if (tmp != nullptr)
      return jnstl::pair<T*, ptrdiff_t>(tmp, len);
    else
      len /= 2;
  }
  return jnstl::pair<T*, ptrdiff_t>(nullptr, 0);
}

template <typename T>
inline void
return_temporary_buffer(T* p) {
  free(p);
}

template<bool>
struct uninitialized_construct_buf_impl {
  template<typename Pointer, typename ForwardIterator>
  static void
  ucr(Pointer first,Pointer last, ForwardIterator it) {
    if(first == last)
      return;

    Pointer cur = first;
#if JNSTL_EXCEPTIONS_ENABLED
    try {
#endif
      jnstl::Construct(LIB::addressof(*first), LIB::move(*it));
      Pointer prev = cur;
      ++cur;
      for(; cur != last; ++cur, ++prev)
        jnstl::Construct(LIB::addressof(*cur), LIB::move(*prev));
      *it = LIB::move(*prev);
#if JNSTL_EXCEPTIONS_ENABLED
    } catch(...) {
      jnstl::_Destruct(first, cur);
      throw;
    }
#endif
  }
};

template<>
struct uninitialized_construct_buf_impl<true> {
  template<typename Pointer, typename ForwardIterator>
  static void
  ucr(Pointer, Pointer, ForwardIterator) { }
};

template <typename Pointer, typename ForwardIterator>
inline void
uninitialized_construct_buf(Pointer first, Pointer last, ForwardIterator it) {
  typedef typename jnstl::iterator_traits<Pointer>::value_type value_type;

  jnstl::uninitialized_construct_buf_impl<
    std::is_trivially_constructible<value_type>::value>::
      ucr(first, last, it);
}

template <typename ForwardIterator, typename T>
class temporary_buffer {
 public:
  typedef T            value_type;
  typedef value_type*  pointer;
  typedef pointer      iterator;
  typedef ptrdiff_t    size_type;

 protected:
  size_type M_req_len;
  size_type M_len;
  pointer   M_buffer;

 public:
  temporary_buffer(ForwardIterator first, ForwardIterator last);

  ~temporary_buffer();

  size_type
  size() const {
    return M_len;
  }

  size_type
  requested_size() const {
    return M_req_len;
  }

  iterator
  begin() {
    return M_buffer;
  }

  iterator
  end() {
    return M_buffer + M_len;
  }

 private:
  temporary_buffer(const temporary_buffer&);

  void
  operator=(const temporary_buffer&);
};

template <typename ForwardIterator, typename T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last)
    : M_req_len(jnstl::distance(first, last)),
      M_len(0), M_buffer(nullptr) {
#if JNSTL_EXCEPTIONS_ENABLED
  try {
#endif
    jnstl::pair<pointer, size_type> p(
        jnstl::get_temporary_buffer<value_type>(M_req_len));

    M_buffer = p.first;
    M_len    = p.second;

    if (M_buffer != nullptr)
      jnstl::uninitialized_construct_buf(M_buffer, M_buffer + M_len, first);
#if JNSTL_EXCEPTIONS_ENABLED
  } catch (...) {
    jnstl::return_temporary_buffer(M_buffer);
    M_buffer = 0;
    M_len    = 0;
    throw;
  }
#endif
}

template <typename ForwardIterator, typename T>
temporary_buffer<ForwardIterator, T>::~temporary_buffer() {
  jnstl::Destruct(M_buffer, M_buffer + M_len);
  jnstl::return_temporary_buffer(M_buffer);
}

}  // namespace jnstl

#endif /* JNSTL_TEMPBUF_H_ */
