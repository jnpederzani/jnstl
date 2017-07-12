#ifndef JNSTL_MEMORY_H_
#define JNSTL_MEMORY_H_

//#include <type_traits>
#include "JNSTL/bits/config.h"

#include "JNSTL/algorithm.h"
#include "JNSTL/iterator.h"


namespace jnstl {
template <typename ForwardIterator, typename T>
inline void uninitialized_fill_impl(ForwardIterator first, ForwardIterator last,
                                    const T& value, std::true_type) {
  jnstl::fill(first, last, value);
}

template <typename ForwardIterator, typename T>
void uninitialized_fill_impl(ForwardIterator first, ForwardIterator last,
                             const T& value, std::false_type) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;
  ForwardIterator current(first);

#if JNSTL_EXCEPTIONS_ENABLED
  try {
    for (; current != last; ++current)
      ::new(static_cast<void*>(&*current)) value_type(value);
  }
  catch(...) {
    for (; first != current; ++first)
      (&*first)->~value_type();
    throw;
  }
#else
  for (; current != last; ++current)
    ::new(static_cast<void*>(&*current)) value_type(value);
#endif
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                               const T& value) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;

  uninitialized_fill_impl(first, last, value, std::is_pod<value_type>());
}

template <typename ForwardIterator>
inline void uninitialized_default_fill(ForwardIterator first,
                                       ForwardIterator last) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;
  ForwardIterator current(first);

#if JNSTL_EXCEPTIONS_ENABLED
  try {
    for (; current != last; ++current)
      ::new(static_cast<void*>(&*current)) value_type();
  }
  catch(...) {
    for (; first != current; ++first)
      (&*first)->~value_type();
    throw;
  }
#else
  for (; current != last; ++current)
    ::new(static_cast<void*>(&*current)) value_type();
#endif
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_impl(InputIterator first,
                                               InputIterator last,
                                               ForwardIterator dest,
                                               std::true_type) {
  return jnstl::copy(first, last, dest);
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_impl(InputIterator first,
                                               InputIterator last,
                                               ForwardIterator dest,
                                               std::false_type) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;

  ForwardIterator current(dest);

#if JNSTL_EXCEPTIONS_ENABLED
  try {
    for (; first != last; ++first, ++current)
      ::new(static_cast<void*>(&*current)) value_type(*first);
  }
  catch (...) {
    for (; dest != current; ++dest)
      (&*dest)->~value_type();
    throw;
  }
#else
  for (; first != last; ++first, ++current)
    ::new(static_cast<void*>(&*current)) value_type(*first);
#endif
  return current;
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first,
                                          InputIterator last,
                                          ForwardIterator dest) {
  typedef typename
      jnstl::iterator_traits<ForwardIterator>::value_type value_type;

  return uninitialized_copy_impl(first, last, dest, std::is_pod<value_type>());
}


template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_move(InputIterator first,
                                          InputIterator last,
                                          ForwardIterator dest) {
  return jnstl::uninitialized_copy(jnstl::make_move_iterator(first),
                                   jnstl::make_move_iterator(last),
                                   dest);
}

}  // namespace jnstl

#endif /* JNSTL_MEMORY_H_ */
