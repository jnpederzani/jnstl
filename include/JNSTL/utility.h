#ifndef JNSTL_UTILITY_H_
#define JNSTL_UTILITY_H_

#include <functional>

#include "JNSTL/bits/config.h"

namespace jnstl {
template <typename T>
struct select_self {
  typedef T result_type;

  const T& operator()(const T& x) const {
    return x;
  }
};

template <typename Pair>
struct select_first {
  typedef Pair argument_type;
  typedef typename Pair::first_type result_type;

  const result_type& operator()(const Pair& x) const {
    return x.first;
  }
};

template <typename Pair>
struct select_second {
  typedef Pair argument_type;
  typedef typename Pair::second_type result_type;

  const result_type& operator()(const Pair& x) const {
    return x.second;
  }
};

template <typename T1, typename T2>
struct pair {
  typedef T1            first_type;
  typedef T2           second_type;
  typedef pair<T1, T2>   this_type;

  first_type first;
  second_type second;

  pair()
      : first(), second() {}

  pair(const first_type& x, const second_type& y)
      : first(x), second(y) {}

  template <typename U, typename V>
  pair(const pair<U, V>& p)
      : first(p.first), second(p.second) {}
};

template <typename T1, typename T2>
inline bool
operator==(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return (a.first == b.first) && (a.second == b.second);
}

template <typename T1, typename T2>
inline bool
operator<(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return ((a.first < b.first) ||
          (!(b.first < a.first) && (a.second < b.second)));
}

template <typename T1, typename T2>
inline bool
operator!=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(a == b);
}

template <typename T1, typename T2>
inline bool
operator>(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return b < a;
}

template <typename T1, typename T2>
inline bool
operator>=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(a < b);
}

template <typename T1, typename T2>
inline bool
operator<=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(b < a);
}



}  // namespace jnstl
#endif  // JNSTL_UTILITY_H_ //
