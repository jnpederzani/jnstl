#ifndef JNSTL_ALGORITHM_H_
#define JNSTL_ALGORITHM_H_

#include <string.h>

#include <utility>
#include <limits>
#include <functional>
#include <cmath>

#include "JNSTL/bits/config.h"
#include "JNSTL/bits/tempbuf.h"
#include "JNSTL/bits/heap.h"
#include "JNSTL/bits/sort.h"
#include "JNSTL/iterator.h"

namespace jnstl {
/**
 * @brief Returns the greater of the given values.
 * @ingroup sorting_algorithms
 * @params a First value to compare.
 * @params b Second value to compare.
 * @return The greater of the given values.
 */
template <typename T>
inline const T&
max(const T& a, const T& b) {
  return a < b ? b : a;
}

/**
 * @brief Returns the lesser of the given values.
 * @ingroup sorting_algorithms
 * @params a First value to compare.
 * @params b Second value to compare.
 * @return The lesser of the given values.
 */
template <typename T>
inline const T&
min(const T& a, const T& b) {
  return b < a ? a : b;
}

template <bool IsSame>
struct iter_swap_impl {
  template <typename ForwardIt1, typename ForwardIt2>
  static void
  iter_swap(ForwardIt1 a, ForwardIt2 b) {
    typedef typename iterator_traits<ForwardIt1>::value_type value_type;
    value_type tmp = LIB::move(*a);
    *a = LIB::move(*b);
    *b = LIB::move(tmp);
  }
};

template <>
struct iter_swap_impl<true> {
  template<typename ForwardIt1, typename ForwardIt2>
  static void
  iter_swap(ForwardIt1 a, ForwardIt2 b) {
    LIB::swap(*a, *b);
  }
};

/**
 * @bried Swaps the values of the elements the iterators are pointing at.
 * @ingroup modifying algorithms
 * @param a An iterator.
 * @param b An iterator.
 * @return Nothing.
 *
 */
template <typename ForwardIt1, typename ForwardIt2>
inline void
iter_swap(ForwardIt1 a, ForwardIt2 b) {
  typedef typename iterator_traits<ForwardIt1>::value_type value_type1;
  typedef typename iterator_traits<ForwardIt2>::value_type value_type2;


  typedef typename iterator_traits<ForwardIt1>::reference reference1;
  typedef typename iterator_traits<ForwardIt2>::reference reference2;

  jnstl::iter_swap_impl<std::is_same<value_type1, value_type2>::value
                        && std::is_same<value_type1&, reference1>::value
                        && std::is_same<value_type2&, reference2>::value>::
      iter_swap(a, b);
}


template <typename ForwardIterator, typename T>
inline void
fill_impl(ForwardIterator first, ForwardIterator last, const T& value) {
  for (; first != last; ++first)
    *first = value;
}

/**
 * @brief Assigns the given value to the elements in the range [first, last).
 * @ingroup modifying_algorithms
 * @params first  A forward iterator.
 * @params second A forward iterator.
 * @params value  A reference-to-const of arbitrary type.
 * @return Nothing.
 */
template <typename ForwardIterator, typename T>
inline void
fill(ForwardIterator first, ForwardIterator last, const T& value) {
  jnstl::fill_impl(jnstl::niter_base(first), jnstl::niter_base(last), value);
}

template <bool, bool, typename>
struct copy_move {
  template <typename II, typename OI>
  static OI
  copy_m(II first, II last, OI d_first) {
    for (; first != last; ++d_first, ++first)
      *d_first = *first;
    return d_first;
  }
};

template <typename Category>
struct copy_move<true, false, Category> {
  template <typename II, typename OI>
  static OI
  copy_m(II first, II last, OI d_first) {
    for (; first != last; ++d_first, ++first)
      *d_first = LIB::move(*first);
    return d_first;
  }
};

template <>
struct copy_move<false, false, random_access_iterator_tag> {
  template <typename II, typename OI>
  static OI
  copy_m(II first, II last, OI d_first) {
    typedef typename iterator_traits<II>::difference_type difference_type;

    for (difference_type n = last -first; n > 0; --n) {
      *d_first = *first;
      ++first;
      ++d_first;
    }

    return d_first;
  }
};

template <>
struct copy_move<true, false, random_access_iterator_tag> {
  template <typename II, typename OI>
  static OI
  copy_m(II first, II last, OI d_first) {
    typedef typename iterator_traits<II>::difference_type difference_type;

    for (difference_type n = last -first; n > 0; --n) {
      *d_first = LIB::move(*first);
      ++first;
      ++d_first;
    }
    return d_first;
  }
};

template <bool IsMove>
struct copy_move<IsMove, true, random_access_iterator_tag> {
  template <typename Tp>
  static Tp*
  copy_m(const Tp* first, const Tp* last, Tp* d_first) {
    const ptrdiff_t num = last - first;
    if (num)
      memmove(d_first, first, sizeof(Tp) * num);
    return d_first + num;
  }
};

template <bool IsMove, typename II, typename OI>
inline OI
copy_move_impl(II first, II last, OI d_first) {
  typedef typename iterator_traits<II>::value_type         ValueTypeI;
  typedef typename iterator_traits<OI>::value_type         ValueTypeO;
  typedef typename iterator_traits<II>::iterator_category  Category;

  const bool is_simple = (std::is_trivially_copyable<ValueTypeI>::value &&
                          std::is_same<ValueTypeI, ValueTypeO>::value &&
                          std::is_pointer<II>::value &&
                          std::is_pointer<OI>::value);
  return jnstl::copy_move<IsMove, is_simple, Category>::copy_m(first, last,
                                                               d_first);
}

template <bool isMove, typename II, typename OI>
inline OI
copy_move_impl2(II first, II last, OI d_first) {
  return jnstl::copy_move_impl<isMove>(jnstl::niter_base(first),
                                       jnstl::niter_base(last),
                                       jnstl::niter_base(d_first));
  }

/**
 * @brief Copies the range [first, last) to another range at d_first.
 * @ingroup modifying_algorithms
 * @params first   An input iterator.
 * @params second  An input iterator.
 * @params d_first An output iterator.
 * @return Output iterator to the element in the destination range,
 *         one past the last element copied
 *
 * This functions will call memmove whenever possible. If not and a random
 * access iterator is passed the loop count will be known allowing possible
 * compiler optimization. If not element by element copy will be performed.
 * Copies the elements in the range [first, last) to another range
 * beginning at d_first. The behavior is undefined if d_first is contained
 * within [first, last); the copy_backward function should be used instead.
 */
template <typename InputIterator, typename OutputIterator>
inline OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator d_first) {
  const bool isMove = jnstl::is_move_iterator<InputIterator>::value;

  return jnstl::copy_move_impl2<isMove>(jnstl::miter_base(first),
                                        jnstl::miter_base(last),
                                        d_first);
}


/**
 * @brief Moves the range [first, last) to another range at d_first.
 * @ingroup modifying_algorithms
 * @params first   An input iterator.
 * @params second  An input iterator.
 * @params d_first An output iterator.
 * @return Output iterator to the element in the destination range,
 *         one past the last moved element
 *
 * This functions will call memmove whenever possible. If not and a random
 * access iterator is passed the loop count will be known allowing possible
 * compiler optimization. If not element by element move will be performed.
 * Moves the elements in the range [first, last) to another range
 * beginning at d_first. The behavior is undefined if d_first is contained
 * within [first, last); the move_backward function should be used instead.
 */
template <typename InputIterator, typename OutputIterator>
inline OutputIterator
move(InputIterator first, InputIterator last, OutputIterator d_first) {
  return jnstl::copy_move_impl2<true>(jnstl::miter_base(first),
                                      jnstl::miter_base(last), d_first);
}

template <bool, bool, typename>
struct copy_move_backward {
  template <typename BI1, typename BI2>
  static BI2
  copy_move_b(BI1 first, BI1 last, BI2 d_last) {
    while (first != last)
      *--d_last = *--last;
    return d_last;
  }
};

template <typename Category>
struct copy_move_backward<true, false, Category> {
  template <typename BI1, typename BI2>
  static BI2
  copy_move_b(BI1 first, BI1 last, BI2 d_last) {
    while (first != last)
      *--d_last = LIB::move(*--last);
    return d_last;
  }
};

template <>
struct copy_move_backward<false, false, random_access_iterator_tag> {
  template <typename BI1, typename BI2>
  static BI2
  copy_move_b(BI1 first, BI1 last, BI2 d_last) {
    typedef typename iterator_traits<BI1>::difference_type difference_type;

    for (difference_type n = last -first; n > 0; --n) {
      *--d_last = *--last;
    }
    return d_last;
  }
};

template <>
struct copy_move_backward<true, false, random_access_iterator_tag> {
  template <typename BI1, typename BI2>
  static BI2
  copy_move_b(BI1 first, BI1 last, BI2 d_last) {
    typedef typename iterator_traits<BI1>::difference_type difference_type;

    for (difference_type n = last -first; n > 0; --n) {
      *--d_last = LIB::move(*--last);
    }
    return d_last;
  }
};

template <bool IsMove>
struct copy_move_backward<IsMove, true, random_access_iterator_tag> {
  template <typename Tp>
  static Tp*
  copy_move_b(const Tp* first, const Tp* last, Tp* d_last) {
    const ptrdiff_t num = last - first;
    if (num)
      memmove(d_last -num, first, sizeof(Tp) * num);
    return d_last - num;
  }
};

template <bool IsMove, typename BI1, typename BI2>
inline BI2
copy_move_backward_impl(BI1 first, BI1 last, BI2 d_last) {
  typedef typename iterator_traits<BI1>::value_type         ValueType1;
  typedef typename iterator_traits<BI2>::value_type         ValueType2;
  typedef typename iterator_traits<BI1>::iterator_category  Category;

  const bool is_simple = (std::is_trivially_copyable<ValueType1>::value &&
                          std::is_same<ValueType1, ValueType2>::value &&
                          std::is_pointer<BI1>::value &&
                          std::is_pointer<BI2>::value);
  return jnstl::copy_move_backward<IsMove, is_simple, Category>::
      copy_move_b(first, last,  d_last);
}

template <bool isMove, typename BI1, typename BI2>
inline BI2
copy_move_backward_impl2(BI1 first, BI1 last, BI2 d_last) {
  return jnstl::copy_move_backward_impl<isMove>(jnstl::niter_base(first),
                                                jnstl::niter_base(last),
                                                jnstl::niter_base(d_last));
  }

/**
 * @brief Copies the range [first, last) to another range at d_last.
 * @ingroup modifying_algorithms
 * @params first   A bidirectional iterator.
 * @params second  A bidirectional iterator.
 * @params d_last  A bidirectional iterator to he end of the destination range.
 * @return Output iterator to the element in the destination range,
 *         one past the last element copied
 *
 * This function has the same effect as copy, but starts at the end of the
 * range and works its way to the start, returning the first of the destination.
 * This functions will call memmove whenever possible. If not and a random
 * access iterator is passed the loop count will be known allowing possible
 * compiler optimization. If not element by element copy will be performed.
 *
 * The behavior is undefined if d_last is contained within [first, last);
 * the copy should be used instead.
 */
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
              BidirectionalIterator2 d_last) {
  const bool isMove = jnstl::is_move_iterator<BidirectionalIterator1>::value;

  return jnstl::copy_move_backward_impl2<isMove>(jnstl::miter_base(first),
                                                 jnstl::miter_base(last),
                                                 d_last);
}

/**
 * @brief Moves the range [first, last) to another range at d_last.
 * @ingroup modifying_algorithms
 * @params first   A bidirectional iterator.
 * @params second  A bidirectional iterator.
 * @params d_last  A bidirectional iterator to he end of the destination range.
 * @return Output iterator to the element in the destination range,
 *         one past the last element copied
 *
 * This function has the same effect as copy, but starts at the end of the
 * range and works its way to the start, returning the first of the destination.
 * This functions will call memmove whenever possible. If not and a random
 * access iterator is passed the loop count will be known allowing possible
 * compiler optimization. If not element by element move will be performed.
 *
 * The behavior is undefined if d_last is contained within [first, last);
 * the copy should be used instead.
 */
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2
move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
              BidirectionalIterator2 d_last) {
  return jnstl::copy_move_backward_impl2<true>(jnstl::miter_base(first),
                                               jnstl::miter_base(last),
                                               d_last);
}

template<bool BoolType>
struct equal_a {
  template<typename II1, typename II2>
  static bool
  equal(II1 first1, II1 last1, II2 first2) {
    for (; first1 != last1; ++first1, ++first2)
      if (!(*first1 == *first2))
        return false;
    return true;
  }
};

template<>
struct equal_a<true> {
  template<typename Tp>
  static bool
  equal(const Tp* first1, const Tp* last1, const Tp* first2) {
    if (const size_t len = (last1 - first1))
      return (memcmp(first1, first2, sizeof(Tp) *len) == 0);
    return true;
  }
};

template <typename II1, typename II2>
inline bool
equal_impl(II1 first1, II1 last1, II2 first2) {
  typedef typename iterator_traits<II1>::value_type ValueType1;
  typedef typename iterator_traits<II2>::value_type ValueType2;

  const bool is_simple = ((std::numeric_limits<ValueType1>::is_integer
                           || std::is_pointer<ValueType1>::value)
                          && std::is_pointer<II1>::value
                          && std::is_pointer<II2>::value
                          && std::is_same<ValueType1, ValueType2>::value);

  return jnstl::equal_a<is_simple>::equal(first1, last1, first2);
}

/**
 * @brief Tests two ranges for element-wise equality.
 * @ingroup non_modifying_algorithms
 * @param first1 An input iterator.
 * @param last1  An input iterator.
 * @param first2 An input iterator.
 * @return A boolean, true or false.
 *
 * This function compares the elements of the two ranges using @c == and
 * returns true if all the elements in the range are equal, false otherwise.
 */
template <typename InputIterator1, typename InputIterator2>
inline bool
equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
  return jnstl::equal_impl(jnstl::niter_base(first1),
                           jnstl::niter_base(last1),
                           jnstl::niter_base(first2));
}

/**
 * @brief Finds the largest range in range [first, last) that is a max heap.
 * @ingroup heap_algorithms
 * @param first An iterator.
 * @param last  An iterator.
 * @param comp A function to compare elements.
 * @return An iterator.
 *
 * This functions checks the elements in range [first, last) using the function
 * @p comp to compare elements and returns the last iterator it for which
 * the range [first, it) is a max heap.
 */
template <typename RandomIt, typename Compare>
RandomIt
is_heap_until(RandomIt first, RandomIt last, Compare comp) {
  typedef typename
      jnstl::iterator_traits<RandomIt>::difference_type difference_type;

  const difference_type heapSize(last - first);
  difference_type parentIdx = 0;
  for (difference_type childIdx = 1; childIdx < heapSize; ++childIdx) {
    if (comp(*(first + parentIdx), *(first + childIdx)))
      return first + childIdx;

    if ((childIdx & 1) == 0)
      ++parentIdx;
  }

  return first + heapSize;
}

/**
 * @brief Finds the largest range in range [first, last) that is a max heap.
 * @ingroup heap_algorithms
 * @param first An iterator.
 * @param last  An iterator.
 * @return An iterator.
 *
 * This functions checks the elements in range [first, last) using
 * operator < to compare elements and returns the last iterator it for which
 * the range [first, it) is a max heap.
 */
template <typename RandomIt>
inline RandomIt
is_heap_until(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  return jnstl::is_heap_until(first, last, comp);
}

/**
 * @brief Checks if the element in range [first, last) are a max heap.
 * @ingroup heap_algorithms
 * @param first An iterator.
 * @param last  An iterator.
 * @param comp  A function to compare elements.
 * @return A boolean, true or false.
 *
 * This functions checks the elements in range [first, last) are a max heap
 * using the function @p comp to compare elements.
 */
template <typename RandomIt, typename Compare>
inline bool
is_heap(RandomIt first, RandomIt last, Compare comp) {
  return (jnstl::is_heap_until(first, last, comp) == last);
}

/**
 * @brief Checks if the element in range [first, last) are a max heap.
 * @ingroup heap_algorithms
 * @param first An iterator.
 * @param last  An iterator.
 * @return A boolean, true or false.
 *
 * This functions checks the elements in range [first, last) are a max heap
 * using operator < to compare elements.
 */
template <typename RandomIt>
inline bool
is_heap(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  return (jnstl::is_heap_until(first, last, comp) == last);
}

/**
 * @brief Pushes last elemnt onto the heap.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 * @param comp  A function to compare elements.
 *
 * This funtion insertes the element at the position @p last - 1 into the max
 * heap defined by the range [first, last -1) using the  function @p comp to
 * compare elements.
 */
template <typename RandomIt, typename Compare>
inline void
push_heap(RandomIt first, RandomIt last, Compare comp) {
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;
  typedef typename
      jnstl::iterator_traits<RandomIt>::difference_type difference_type;

  value_type val = LIB::move(*(last -1));
  jnstl::promote_heap(first, difference_type(last - first -1),
                      difference_type(0), val, comp);
}

/**
 * @brief Pushes last elemnt onto the heap.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 *
 * This funtion insertes the element at the position @p last - 1 into the max
 * heap defined by the range [first, last -1) using the  operator < to compare
 * elements.
 */
template <typename RandomIt>
inline void
push_heap(RandomIt first, RandomIt last) {
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;
  typedef typename
      jnstl::iterator_traits<RandomIt>::difference_type difference_type;

  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;

  value_type val = LIB::move(*(last -1));
  jnstl::promote_heap(first, difference_type(last - first -1),
                      difference_type(0), val, comp);
}

/**
 * @brief Constructs a max heap in the range [first, last).
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 * @param comp  A function to compare elements.
 *
 * This funtion constructs a max heap in the range [first, last) using the
 * function @p comp to compare elements.
 */
template <typename RandomIt, typename Compare>
inline void
make_heap(RandomIt first, RandomIt last, Compare comp) {
  jnstl::make_heap_impl(first, last, comp);
}

/**
 * @brief Constructs a max heap in the range [first, last).
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 *
 * This funtion constructs a max heap in the range [first, last) using the
 * operator < to compare elements.
 */
template <typename RandomIt>
inline void
make_heap(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::make_heap(first, last, comp);
}

/**
 * @brief Pop an elemnt off the top of the heap.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 * @param comp  A comparison function.
 *
 * This funtion pops the top of the heap. The elements in first and last-1 are
 * swapped and the range [first, last-1) is made into a max heap using the
 * function @p comp to compare elements.
 * This allows the function calling pop_heap to easily remove the element
 * in position last-1 from the end of the range.
 */
template <typename RandomIt, typename Compare>
inline void
pop_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    --last;
    jnstl::pop_heap_impl(first, last, last, comp);
  }
}

/**
 * @brief Pop an elemnt off the top of the heap.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 *
 * This funtion pops the top of the heap. The elements in first and last-1 are
 * swapped and the range [first, last-1) is made into a max heap using the
 * operator < to compare elements.
 * This allows the function calling pop_heap to easily remove the element
 * in position last-1 from the end of the range.
 */
template <typename RandomIt>
inline void
pop_heap(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::pop_heap(first, last, comp);
}

/**
 * @brief Convertes the max heap into a sorted range.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 * @param comp  A comparison function.
 *
 * This funtion converts the max heap [first, last) into a sorted range using
 * the function @p comp to compare elements.
 * The resulting sorted range [first, last) no longer satisfies the heap
 * property.
 */
template <typename RandomIt, typename Compare>
inline void
sort_heap(RandomIt first, RandomIt last, Compare comp) {
  while (last - first > 1) {
    --last;
    /* Largest values are successively popped in posistion --last, resulting
       in a sorted range */
    jnstl::pop_heap_impl(first, last, last, comp);
  }
}

/**
 * @brief Convertes the max heap into a sorted range.
 * @ingroup heap_algorithms
 * @param first Start of heap.
 * @param last  End of heap.
 *
 * This funtion converts the max heap [first, last) into a sorted range using
 * the operator < to compare elements.
 * The resulting sorted range [first, last) no longer satisfies the heap
 * property.
 */
template <typename RandomIt>
inline void
sort_heap(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::sort_heap(first, last, comp);
}

/**
 * @brief Tests two ranges for element-wise equality.
 * @ingroup non_modifying_algorithms
 * @param first1 An input iterator.
 * @param last1  An input iterator.
 * @param first2 An input iterator.
 * @param p      A binarypredicate.
 * @return A boolean, true or false.
 *
 * This function compares the elements of the two ranges using the binary
 * predicate parametert p and returns true if all the elements in the rang
 * are equal, false otherwise.
 */
template <typename InputIter1, typename InputIter2, typename BinaryPredicate>
inline bool
equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
      BinaryPredicate p) {
  for (; first1 != last1; ++first1, ++first2)
    if (!static_cast<bool>(p(*first1, *first2)))
      return false;
  return true;
}



/**
 * @brief Merges the elements of the two sorted ranges.
 * @ingroup sorting_algorithms
 * @params first1  An input iterator.
 * @params last1   An input iterator.
 * @params first2  An input iterator.
 * @params last2   An input iterator.
 * @params d_first An output iterator pointing to the beginning of the
 *                 destination rang
 * @params comp  A functor to use for comparison.
 * @return An iterator to the element past the last merged element.
 *
 * This function merges two sorted ranges @p [first1, last1) and
 * @p [first2, last2) into the sorted range starting at @p d_first.
 * The elements are compared using the given comparison function @p comp.
 * The merge is stable, for equivalent elements in the two original ranges,
 * the elements from the first range (preserving their original order) precede
 * the elements form the second range (preserving their original order).
 * Both input ranges must be sorted and the output range must not overlap
 * with either of the input ranges.
 */
template <typename InputIt1, typename InputIt2, typename OutputIt,
          typename Compare>
inline OutputIt
merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
      OutputIt d_first, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first2, *first1)) {
      *d_first = *first2;
      ++first2;
    } else {
      *d_first = *first1;
      ++first1;
    }
    ++d_first;
  }
  return jnstl::copy(first2, last2, jnstl::copy(first1, last1, d_first));
}

/**
 * @brief Merges the elements of the two sorted ranges.
 * @ingroup sorting_algorithms
 * @params first1  An input iterator.
 * @params last1   An input iterator.
 * @params first2  An input iterator.
 * @params last2   An input iterator.
 * @params d_first An output iterator pointing to the beginning of the
 *                 destination rang
 * @return An iterator to the element past the last merged element.
 *
 * This function merges two sorted ranges @p [first1, last1) and
 * @p [first2, last2) into the sorted range starting at @p d_first.
 * The elements are compared using operator <. The merge is stable, for
 * equivalent elements in the two original ranges,
 * the elements from the first range (preserving their original order) precede
 * the elements form the second range (preserving their original order).
 * Both input ranges must be sorted and the output ranfe must not overlap
 * with either of the input ranges.
 */
template <typename InputIt1, typename InputIt2, typename OutputIt>
inline OutputIt
merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
      OutputIt d_first) {
  return jnstl::merge(
      first1, last1, first2, last2, d_first,
      std::less<typename iterator_traits<InputIt1>::value_type>());
}

#if 0
/**
 * @brief Merges two consecutive sorted ranges.
 * @ingroup sorting_algorithms
 * @params first  An iterator.
 * @params middle An iterator.
 * @params last   An iterator.
 * @return Nothing.
 *
 * This function merges two consecutive sorted ranges @p [first, middle) and
 * @p [middle, last) into one sorted range starting @p [first, last).
 * The elements are compared using operator <. The merge is stable, for
 * equivalent elements in the two original ranges,
 * the elements from the first range (preserving their original order) precede
 * the elements form the second range (preserving their original order).
 * Both input ranges must be sorted using the same operator <.
 */
template <typename BidirIt>
inline
inplace_merge(BidirIt first, BidirIt middle, BidirIt last) {
  jnstl::inplace_merge(first, middle, last,
               std::less<typename iterator_traits<InputIt>::value_type>());
}

/**
 * @brief Merges two consecutive sorted ranges.
 * @ingroup sorting_algorithms
 * @params first  An iterator.
 * @params middle An iterator.
 * @params last   An iterator.
 * @params comp  A functor to use for comparison.
 * @return Nothing.
 *
 * This function merges two consecutive sorted ranges @p [first, middle) and
 * @p [middle, last) into one sorted range starting @p [first, last).
 * The elements are compared using the function @p comp. The merge is stable,
 * for equivalent elements in the two original ranges,
 * the elements from the first range (preserving their original order) precede
 * the elements form the second range (preserving their original order).
 * Both input ranges must be sorted using the same function @p comp.
 */
template <typename BidirIt>
inline
inplace_merge(BidirIt first, BidirIt middle, BidirIt last, Compare comp) {
  typedef typename iterator_traits<RandomIt>::value_type        value_type;
  typedef typename iterator_traits<RandomIt>::differrence_type  size_type;

  if (first == middle || middle == last)
    return;

  const size_type count = last - first;
  const size_type len1 = middle - first;
  const size_type len2 = last - middle;

  value_type* const buffer = static_cast<value_type*>(
      jnstl::allocator::allocate(count * sizeof(value_type)));

  jnstl::merge_with_buffer(first, middle, last, len1, len2,
                           buffer, count, comp);

  jnstl::allocator::deallocate(buffer, count * sizeof(value_type));
}
#endif

/**
 * @brief Finds the largest range in range [first, last) that is sorted.
 * @param first An iterator.
 * @param last  An iterator.
 * @param comp  A function to compare elements.
 * @retrun Returns a boolean, true or false.
 *
 * This function checks  the elements in the range [first, last) using the
 * function @p comp to compare elements and returns the last iterator it for
 * which the range [first, it) is sorted.
 */
template <typename ForwardIt, typename Compare>
inline ForwardIt
is_sorted_until(ForwardIt first, ForwardIt last, Compare comp) {
  if (first == last)
    return last;

  ForwardIt next = first;
  ++next;
  for (; next != last; first = next, ++next) {
    if (comp(*next, *first))
      return next;
  }
  return next;
}

/**
 * @brief Finds the largest range in range [first, last) that is sorted.
 * @param first An iterator.
 * @param last  An iterator.
 * @retrun Returns a boolean, true or false.
 *
 * This function checks  the elements in the range [first, last) using the
 * operator < to compare elements and returns the last iterator it for which
 * the range [first, it) is sorted.
 */
template <typename ForwardIt>
inline ForwardIt
is_sorted_until(ForwardIt first, ForwardIt last) {
  std::less<typename jnstl::iterator_traits<ForwardIt>::value_type> comp;
  return jnstl::is_sorted_until(first, last, comp);
}

/**
 * @brief Checks if the elements in range are sorted.
 * @param first An iterator.
 * @param last  An iterator.
 * @param comp  A function to compare elements.
 * @retrun Returns a boolean, true or false.
 *
 * This function checks if the elements in the range [first, last) are sorted
 * using the function @p comp to compare elements.
 */
template <typename ForwardIt, typename Compare>
inline bool
is_sorted(ForwardIt first, ForwardIt last, Compare comp) {
  return (jnstl::is_sorted_until(first, last, comp) == last);
}

/**
 * @brief Checks if the elements in range are sorted.
 * @param first An iterator.
 * @param last  An iterator.
 * @retrun Returns a boolean, true or false.
 *
 * This function checks if the elements in the range [first, last) are sorted
 * using the operator < to compare elements.
 */
template <typename ForwardIt>
inline bool
is_sorted(ForwardIt first, ForwardIt last) {
  std::less<typename jnstl::iterator_traits<ForwardIt>::value_type> comp;
  return (jnstl::is_sorted_until(first, last, comp) == last);
}

/**
 * @brief Sorts the elements in the range [first, last).
 * @ingroup sorting_algorithms
 * @params first A random access iterator.
 * @params last  A random access iterator.
 * @params comp  A functor to use for comparison.
 * @return Nothing.
 *
 * This function sorts the elements in the range @p [first, last) in ascending
 * order. The elements are compared using the given comparison function @p comp.
 * The order of equal elements is guaranteed to be preserved.
 */
template <typename RandomIt, typename Compare>
inline void
stable_sort(RandomIt first, RandomIt last, Compare comp) {
  typedef typename iterator_traits<RandomIt>::value_type        value_type;
  typedef typename iterator_traits<RandomIt>::difference_type  difference_type;

  const difference_type count = last - first;

  typedef temporary_buffer<RandomIt, value_type> tmp_buf;
  tmp_buf buffer(first, last);

  jnstl::merge_sort_buffer(first, last, buffer.begin(), comp);
}

/**
 * @brief Sorts the elements in the range [first, last).
 * @ingroup sorting_algorithms
 * @params first A random access iterator.
 * @params last  A random access iterator.
 * @return Nothing.
 *
 * This function sorts the elements in the range @p [first, last) in ascending
 * order. The elements are compared using operator <. The order of equal
 * elements is guaranteed to be preserved.
 */
template <typename RandomIt>
inline void
stable_sort(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::stable_sort(first, last, comp);
}

/**
 * @brief Sorts the elements in the range [first, last).
 * @ingroup sorting_algorithms
 * @params first A random access iterator.
 * @params last  A random access iterator.
 * @params comp  A functor to use for comparison.
 * @return Nothing.
 *
 * This function sorts the elements in the range @p [first, last) in ascending
 * order. The elements are compared using the given comparison function @p comp.
 * The order of equal elements is not guaranteed to be preserved, use @p
 * stable_sort() if this is needed.
 */
template <typename RandomIt, typename Compare>
inline void
sort(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    jnstl::introsort_loop(first, last, std::log(last - first) * 2, comp);
    jnstl::final_insertion_sort(first, last, comp);
  }
}

/**
 * @brief Sorts the elements in the range [first, last).
 * @ingroup sorting_algorithms
 * @params first A random access iterator.
 * @params last  A random access iterator.
 * @return Nothing.
 *
 * This function sorts the elements in the range @p [first, last) in ascending
 * order. The elements are compared using operator <. The order of equal
 * elements is not guaranteed to be preserved, use @p stable_sort() if this is
 * needed.
 */
template <typename RandomIt>
inline void
sort(RandomIt first, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::sort(first, last, comp);
}

/**
 * @brief Sorts the smallest elements in a sequence
 * @ingroup sorting_algorithms
 * @params first  A random access iterator.
 * @params middle A random access iterator.
 * @params last   A random access iterator.
 * @params comp   A function to compare elements.
 * @return Nothing.
 *
 * This function sorts the smallest (middle - first) number of elements in
 * the range @p [first, last) using the function @p comp and rearranges them
 * such that they occupy the range [first, middle).
 * The order of equal elements is not guaranteed to be preserved and the order
 * of the elements in the range [middle, last) is unspecified.
 */
template <typename RandomIt, typename Compare>
inline void
partial_sort(RandomIt first, RandomIt middle, RandomIt last, Compare comp) {
  jnstl::partial_sort_impl(first, middle, last, comp);
}

/**
 * @brief Sorts the smallest elements in a sequence
 * @ingroup sorting_algorithms
 * @params first A random access iterator.
 * @params middle A random access iterator.
 * @params last  A random access iterator.
 * @return Nothing.
 *
 * This function sorts the smallest (middle - first) number of elements in
 * the range @p [first, last) using the operator < and rearranges them such
 * that they occupy the range [first, middle).
 * The order of equal elements is not guaranteed to be preserved and the order
 * of the elements in the range [middle, last) is unspecified.
 */
template <typename RandomIt>
inline void
partial_sort(RandomIt first, RandomIt middle, RandomIt last) {
  std::less<typename jnstl::iterator_traits<RandomIt>::value_type> comp;
  jnstl::partial_sort_impl(first, middle, last, comp);
}


}  // namespace jnstl

#endif /* JNSTL_ALGORITHM_H_ */
