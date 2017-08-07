#ifndef JNSTL_ITERATOR_H_
#define JNSTL_ITERATOR_H_

#include <utility>
#include <cstddef>

#include "JNSTL/bits/config.h"
#include "JNSTL/type_traits.h"

namespace jnstl {

enum iterator_status_flag {
  isf_none            = 0x00, /// This is called none and not called invalid because it is not strictly the opposite of invalid.
  isf_valid           = 0x01, /// The iterator is valid, which means it is in the range of [begin, end].
  isf_current         = 0x02, /// The iterator is valid and points to the same element it did when created. For example, if an iterator points to vector::begin() but an element is inserted at the front, the iterator is valid but not current. Modification of elements in place do not make iterators non-current.
  isf_can_dereference = 0x04  /// The iterator is dereferencable, which means it is in the range of [begin, end). It may or may not be current.
};

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag       : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguos_iterator_tag     : public random_access_iterator_tag {};

template <typename Category, typename T, typename Distance = ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
struct iterator {
  typedef Category  iterator_category;
  typedef T         value_type;
  typedef Distance  difference_type;
  typedef Pointer   pointer;
  typedef Reference reference;
};

template <typename Iterator>
struct iterator_traits {
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type        value_type;
  typedef typename Iterator::difference_type   difference_type;
  typedef typename Iterator::pointer           pointer;
  typedef typename Iterator::reference         reference;
};

template <typename T>
struct iterator_traits<T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef T*                         pointer;
  typedef T&                         reference;
};

template <typename T>
struct iterator_traits<const T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef const T*                   pointer;
  typedef const T&                   reference;
};

template <typename Iterator, bool HasBase>
struct Iter_base {
  typedef Iterator iterator_type;
  static iterator_type get_base(Iterator it) {
    return it;
  }
};

template <typename Iterator>
struct Iter_base<Iterator, true> {
  typedef typename Iterator::iterator_type iterator_type;
  static iterator_type get_base(Iterator it) {
    return it.base();
  }
};

template <typename Iterator>
struct Niter_base
    : Iter_base<Iterator, is_normal_iterator<Iterator>::value> {};

template <typename Iterator>
inline typename Niter_base<Iterator>::iterator_type
niter_base(Iterator it) {
  return Niter_base<Iterator>::get_base(it);
}

template <typename Iterator>
struct Miter_base
    : Iter_base<Iterator, is_move_iterator<Iterator>::value> {};

template <typename Iterator>
inline typename Miter_base<Iterator>::iterator_type
miter_base(Iterator it) {
  return Miter_base<Iterator>::get_base(it);
}

template <typename Iterator>
class reverse_iterator
    : public iterator<typename iterator_traits<Iterator>::iterator_category,
                      typename iterator_traits<Iterator>::value_type,
                      typename iterator_traits<Iterator>::difference_type,
                      typename iterator_traits<Iterator>::pointer,
                      typename iterator_traits<Iterator>::reference> {
 protected:
  typedef iterator_traits<Iterator>               traits_type;

  Iterator mCurrent;

 public:
  typedef Iterator                                iterator_type;
  typedef typename traits_type::difference_type   difference_type;
  typedef typename traits_type::pointer           pointer;
  typedef typename traits_type::reference         reference;

  reverse_iterator()
      : mCurrent() {}

  explicit
  reverse_iterator(iterator_type x)
      : mCurrent(x) {}

  reverse_iterator(const reverse_iterator& x)
      : mCurrent(x.mCurrent) {}

  template <typename Iter>
  reverse_iterator(const reverse_iterator<Iter>& x)
      : mCurrent(x.base()) {}

  iterator_type
  base() const {
    return mCurrent;
  }

  reference
  operator*() const {
    Iterator tmp = mCurrent;
    return *(--tmp);
  }

  pointer
  operator->() const {
    return &(operator*());
  }

  reverse_iterator&
  operator++() {
    --mCurrent;
    return *this;
  }

  reverse_iterator
  operator++(int) {
    reverse_iterator tmp = *this;
    --mCurrent;
    return tmp;
  }

  reverse_iterator&
  operator--() {
    ++mCurrent;
    return *this;
  }

  reverse_iterator
  operator--(int) {
    reverse_iterator tmp = *this;
    ++mCurrent;
    return tmp;
  }

  reverse_iterator
  operator+(difference_type n) const {
    return reverse_iterator(mCurrent - n);
  }

  reverse_iterator
  operator+=(difference_type n) const {
    mCurrent -= n;
    return *this;
  }

  reverse_iterator
  operator-(difference_type n) const {
    return reverse_iterator(mCurrent + n);
  }

  reverse_iterator
  operator-=(difference_type n) const {
    mCurrent += n;
    return *this;
  }

  reference
  operator[](difference_type n) const {
    return *(this + n);
  }
};

template <typename Iterator>
inline bool
operator==(const reverse_iterator<Iterator>& x,
           const reverse_iterator<Iterator>& y) {
  return x.base() == y.base();
}

template <typename Iterator>
inline bool
operator<(const reverse_iterator<Iterator>& x,
          const reverse_iterator<Iterator>& y) {
  return x.base() < y.base();
}

template <typename Iterator>
inline bool
operator!=(const reverse_iterator<Iterator>& x,
           const reverse_iterator<Iterator>& y) {
  return !(x == y);
}

template <typename Iterator>
inline bool
operator>(const reverse_iterator<Iterator>& x,
          const reverse_iterator<Iterator>& y) {
  return y < x;
}

template <typename Iterator>
inline bool
operator<=(const reverse_iterator<Iterator>& x,
           const reverse_iterator<Iterator>& y) {
  return !(y < x);
}

template <typename Iterator>
inline bool
operator>=(const reverse_iterator<Iterator>& x,
           const reverse_iterator<Iterator>& y) {
  return !(x < y);
}

template <typename Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& x,
          const reverse_iterator<Iterator>& y) {
  return y.base() - x.base();
}

template <typename Iterator>
inline reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n,
          const reverse_iterator<Iterator>& x) {
  return reverse_iterator<Iterator>(x.base() - n);
}

template<typename Iterator, typename Container>
class normal_iterator {
 protected:
  Iterator mIterator;

  typedef iterator_traits<Iterator>                traits_type;

 public:
  typedef Iterator                                 iterator_type;
  typedef typename traits_type::iterator_category  iterator_category;
  typedef typename traits_type::value_type         value_type;
  typedef typename traits_type::difference_type    difference_type;
  typedef typename traits_type::reference          reference;
  typedef typename traits_type::pointer            pointer;

  typedef Container                                container_type;
  typedef normal_iterator<Iterator, Container>     this_type;

  normal_iterator()
      : mIterator(iterator_type()) {}

  explicit
  normal_iterator(const Iterator& it)
      : mIterator(it) { }

  template<typename Iter>
  normal_iterator(const normal_iterator<Iter, Container>& x)
      : mIterator(x.base()) { }

  reference
  operator*() const {
    return *mIterator;
  }

  pointer
  operator->() const {
    return mIterator;
  }

  normal_iterator&
  operator++() {
    ++mIterator;
    return *this;
  }

  normal_iterator
  operator++(int) {
    return normal_iterator(mIterator++);
  }

  normal_iterator&
  operator--() {
    --mIterator;
    return *this;
  }

  normal_iterator
  operator--(int) {
    return normal_iterator(mIterator--);
  }

  // Random access iterator requirements
  reference
  operator[](difference_type n) const {
    return mIterator[n];
  }

  normal_iterator&
  operator+=(difference_type n) {
    mIterator += n;
    return *this;
  }

  normal_iterator
  operator+(difference_type n) const {
    return normal_iterator(mIterator + n);
  }

  normal_iterator&
  operator-=(difference_type n) {
    mIterator -= n;
    return *this;
  }

  normal_iterator
  operator-(difference_type n) const {
    return normal_iterator(mIterator - n);
  }

  const Iterator&
  base() const {
    return mIterator;
  }
};

// Forward iterator requirements
template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator==(const normal_iterator<IteratorL, Container>& lhs,
           const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() == rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator==(const normal_iterator<Iterator, Container>& lhs,
           const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() == rhs.base();
}

template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator!=(const normal_iterator<IteratorL, Container>& lhs,
           const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() != rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator!=(const normal_iterator<Iterator, Container>& lhs,
           const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() != rhs.base();
}

// Random access iterator requirements
template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator<(const normal_iterator<IteratorL, Container>& lhs,
          const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() < rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator<(const normal_iterator<Iterator, Container>& lhs,
          const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() < rhs.base();
}

template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator>(const normal_iterator<IteratorL, Container>& lhs,
          const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() > rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator>(const normal_iterator<Iterator, Container>& lhs,
          const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() > rhs.base();
}

template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator<=(const normal_iterator<IteratorL, Container>& lhs,
           const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() <= rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator<=(const normal_iterator<Iterator, Container>& lhs,
           const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() <= rhs.base();
}

template<typename IteratorL, typename IteratorR, typename Container>
inline bool
operator>=(const normal_iterator<IteratorL, Container>& lhs,
           const normal_iterator<IteratorR, Container>& rhs) {
  return lhs.base() >= rhs.base();
}

template<typename Iterator, typename Container>
inline bool
operator>=(const normal_iterator<Iterator, Container>& lhs,
           const normal_iterator<Iterator, Container>& rhs) {
  return lhs.base() >= rhs.base();
}

template <typename Iterator>
class move_iterator {
  // Don't inherit from iterator.
 protected:
  typedef iterator_traits<Iterator>               traits_type;
  typedef typename traits_type::reference         base_ref;

  Iterator mIterator;

 public:
  typedef Iterator                                iterator_type;
  typedef typename traits_type::iterator_category iterator_category;
  typedef typename traits_type::value_type        value_type;
  typedef typename traits_type::difference_type   difference_type;
  typedef Iterator                                pointer;

  typedef typename std::conditional<std::is_reference<base_ref>::value,
                               typename std::remove_reference<base_ref>::type&&,
                               base_ref>::type          reference;

  move_iterator()
      : mIterator() {}

  explicit move_iterator(iterator_type mi)
      : mIterator(mi) {}

  template<typename U>
  move_iterator(const move_iterator<U>& mi)
      : mIterator(mi.base()) {}

  iterator_type base() const {
    return mIterator;
  }

  reference operator*() const {
    return static_cast<reference>(*mIterator);
  }

  pointer operator->() const {
    return mIterator;
  }

  move_iterator& operator++() {
    ++mIterator;
    return *this;
  }

  move_iterator operator++(int) {
    move_iterator tmp = *this;
    ++mIterator;
    return tmp;
  }

  move_iterator& operator--() {
    --mIterator;
    return *this;
  }

  move_iterator  operator--(int) {
    move_iterator tmp = *this;
    --mIterator;
    return tmp;
  }

  move_iterator operator+(difference_type n) const {
    return move_iterator(mIterator + n);
  }

  move_iterator& operator+=(difference_type n) {
    mIterator += n;
    return *this;
  }

  move_iterator operator-(difference_type n) const {
    return move_iterator(mIterator - n);
  }

  move_iterator& operator-=(difference_type n) {
    mIterator -= n;
    return *this;
  }

  reference operator[](difference_type n) const {
    return LIB::move(mIterator[n]);
  }
};

template<typename Iterator1, typename Iterator2>
inline bool operator==(const move_iterator<Iterator1>& a,
                       const move_iterator<Iterator2>& b) {
  return a.base() == b.base();
}

template<typename Iterator1, typename Iterator2>
inline bool operator!=(const move_iterator<Iterator1>& a,
                       const move_iterator<Iterator2>& b) {
  return !(a == b);
}
template<typename Iterator1, typename Iterator2>
inline bool operator<(const move_iterator<Iterator1>& a,
                      const move_iterator<Iterator2>& b) {
  return a.base() < b.base();
}

template<typename Iterator1, typename Iterator2>
inline bool operator<=(const move_iterator<Iterator1>& a,
                       const move_iterator<Iterator2>& b) {
  return !(b < a);
}

template<typename Iterator1, typename Iterator2>
inline bool operator>(const move_iterator<Iterator1>& a,
                      const move_iterator<Iterator2>& b) {
  return b < a;
}

template<typename Iterator1, typename Iterator2>
inline bool operator>=(const move_iterator<Iterator1>& a,
                       const move_iterator<Iterator2>& b) {
  return !(a < b);
}

template<typename Iterator1, typename Iterator2>
inline auto operator-(const move_iterator<Iterator1>& a,
                      const move_iterator<Iterator2>& b)
    -> decltype(a.base() - b.base()) {
  return a.base() - b.base();
}

template<typename Iterator>
inline bool operator==(const move_iterator<Iterator>& a,
                       const move_iterator<Iterator>& b) {
  return a.base() == b.base();
}

template<typename Iterator                     >
inline bool operator!=(const move_iterator<Iterator>& a,
                       const move_iterator<Iterator>& b) {
  return !(a == b);
}
template<typename Iterator>
inline bool operator<(const move_iterator<Iterator>& a,
                      const move_iterator<Iterator>& b) {
  return a.base() < b.base();
}

template<typename Iterator>
inline bool operator<=(const move_iterator<Iterator>& a,
                       const move_iterator<Iterator>& b) {
  return !(b < a);
}

template<typename Iterator>
inline bool operator>(const move_iterator<Iterator>& a,
                      const move_iterator<Iterator>& b) {
  return b < a;
}

template<typename Iterator>
inline bool operator>=(const move_iterator<Iterator>& a,
                       const move_iterator<Iterator>& b) {
  return !(a < b);
}

template<typename Iterator>
inline auto operator-(const move_iterator<Iterator>& a,
                      const move_iterator<Iterator>& b)
    -> decltype(a.base() - b.base()) {
  return a.base() - b.base();
}

template<typename Iterator>
inline move_iterator<Iterator>
operator+(typename move_iterator<Iterator>::difference_type n,
          const move_iterator<Iterator>& a) {
  return a + n;
}

template<typename Iterator>
inline move_iterator<Iterator> make_move_iterator(Iterator i) {
  return move_iterator<Iterator>(i);
}

template <typename InputIterator>
inline typename jnstl::iterator_traits<InputIterator>::difference_type
distance_impl(InputIterator first, InputIterator last,
              jnstl::input_iterator_tag) {
  typename jnstl::iterator_traits<InputIterator>::difference_type n = 0;

  while (first != last) {
    ++first;
    ++n;
  }

  return n;
}

template <typename RandomAccessIterator>
inline typename jnstl::iterator_traits<RandomAccessIterator>::difference_type
distance_impl(RandomAccessIterator first, RandomAccessIterator last,
              jnstl::random_access_iterator_tag) {
  return last - first;
}

template <typename InputIterator>
inline typename jnstl::iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
  typedef typename jnstl::iterator_traits<InputIterator>::iterator_category IC;

  return distance_impl(first, last, IC());
}

template <typename InputIterator, typename Distance>
inline void advance_impl(InputIterator& i, Distance n,
                         jnstl::input_iterator_tag) {
  while (n--)
    ++i;
}

template <bool isSigned>
struct advance_bi_impl {
  template <typename BidirectionalIterator, typename Distance>
  static void advance_impl(BidirectionalIterator& i, Distance n) {
    while (n--)
      ++i;
  }
};

template <>
struct advance_bi_impl<true> {
  template <typename BidirectionalIterator, typename Distance>
  static void advance_impl(BidirectionalIterator& i, Distance n) {
    if (n > 0) {
      while (n--)
        ++i;
    } else {
      while (n++)
        --i;
    }
  }
};

template <typename BidirectionalIterator, typename Distance>
inline void advance_impl(BidirectionalIterator& i, Distance n,
                         jnstl::bidirectional_iterator_tag) {
  advance_bi_impl<std::is_signed<Distance>::value>::advance_impl(i, n);
}

template <typename RandomAccessIterator, typename Distance>
inline void advance_impl(RandomAccessIterator& i, Distance n,
                         jnstl::random_access_iterator_tag) {
  i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i,  Distance n) {
  typedef typename jnstl::iterator_traits<InputIterator>::iterator_category IC;

  advance_impl(i, n, IC());
}

template <typename InputIterator>
inline InputIterator
next(InputIterator i,
     typename jnstl::iterator_traits<InputIterator>::difference_type n = 1) {
  jnstl::advance(i, n);
  return i;
}

template <typename InputIterator>
inline InputIterator
prev(InputIterator i,
     typename jnstl::iterator_traits<InputIterator>::difference_type n = 1) {
  jnstl::advance(i, -n);
  return i;
}

}  // namespace jnstl

#endif /* JNSTL_ITERATOR_H_ */
