#ifndef JNSTL_SORT_H_
#define JNSTL_SORT_H_

#include "JNSTL/bits/config.h"

namespace jnstl {
template <typename InputIt1, typename InputIt2,
          typename OutputIt, typename Compare>
OutputIt
merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
      OutputIt d_first, Compare comp);

template <typename ForwardIt1, typename ForwardIt2>
void
iter_swap(ForwardIt1 a, ForwardIt2 b);

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2
move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
              BidirectionalIterator2 d_last);

template <typename RandomIt, typename Compare>
void
sort_heap(RandomIt first, RandomIt last, Compare comp);

template <typename Iter, typename Compare>
void
move_median_to_first(Iter first, Iter a, Iter b, Iter c, Compare comp) {
  if (comp(*a, *b)) {
    if (comp(*b, *c))
      jnstl::iter_swap(first, b);
    else if (comp(*a, *c))
      jnstl::iter_swap(first, c);
    else
      jnstl::iter_swap(first, a);
  } else if (comp(*a, *c)) {
    jnstl::iter_swap(first, a);
  } else if (comp(*b, *c)) {
    jnstl::iter_swap(first, c);
  } else {
    jnstl::iter_swap(first, b);
  }
}

template <typename RandomIt, typename Pointer, typename Compare>
void
merge_sort_buffer(RandomIt first, RandomIt last, Pointer buffer, Compare comp) {
  typedef typename iterator_traits<RandomIt>::difference_type  difference_type;

  const difference_type count = last -first;

  if (count > 1) {
    const difference_type mid = count / 2;
    const RandomIt middle = first + mid;

    /* split one more time at this level to reduce recursive depth */
    if (mid > 1) {
      const difference_type q1 = mid / 2;
      const RandomIt part(first + q1);

      jnstl::merge_sort_buffer(first, part, buffer, comp);
      jnstl::merge_sort_buffer(part, middle, buffer + q1, comp);
      jnstl::merge(first, part, part, middle, buffer, comp);
    } else {
      *buffer = *first;
    }

    if (count - mid > 1) {
      const difference_type q3 = mid + (count - mid) / 2;
      const RandomIt part(first + q3);

      jnstl::merge_sort_buffer(middle, part, buffer + mid, comp);
      jnstl::merge_sort_buffer(part, last, buffer + q3, comp);
      jnstl::merge(middle, part, part, last, buffer + mid, comp);
    } else {
      *(buffer + mid) = *middle;
    }

    jnstl::merge(buffer, buffer + mid, buffer + mid, buffer + count,
                 first, comp);
  }
}

template <typename RandomIt, typename Compare>
inline void
unsafe_linear_insert(RandomIt last, Compare comp) {
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;

  value_type val = LIB::move(*last);
  RandomIt prev = last;

  --prev;
  while (comp(val, *prev)) {
    *last = LIB::move(*prev);
    last = prev;
    --prev;
  }
  *last = LIB::move(val);
}

template <typename RandomIt, typename Compare>
inline void
insertion_sort(RandomIt first, RandomIt last, Compare comp) {
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;
  if (first != last) {
    for (RandomIt it = first + 1; it != last; ++it) {
      /* Check if it* < *first so that, if not, the linear insert
         can safely loop down from it without worrying about dereferncing
         --first */
      if (comp(*it, *first)) {
        value_type val(LIB::move(*it));
        jnstl::move_backward(first, it, it + 1);
        *first = LIB::move(val);
      } else {
        jnstl::unsafe_linear_insert(it, comp);
      }
    }
  }
}

template <typename RandomIt, typename Compare>
inline void
unsafe_insertion_sort(RandomIt first, RandomIt last, Compare comp) {
  for (RandomIt it = first; it != last; ++it)
    jnstl::unsafe_linear_insert(it, comp);
}

const int S_threshold = 16;

template <typename RandomIt, typename Compare>
void
final_insertion_sort(RandomIt first, RandomIt last, Compare comp) {
  /* After the introsort_loop the partition will be properly order.
     Need to only sort inside the partitions . */
  if (last - first > S_threshold) {
    jnstl::insertion_sort(first, first + S_threshold, comp);
    jnstl::unsafe_insertion_sort(first + S_threshold, last, comp);
  } else {
    jnstl::insertion_sort(first, last, comp);
  }
}

/* pivot located at first-1, so it never moves during the swaps
   unsafe parition is also guaranteed by S_threshold > 1.
 */
template <typename RandomIt, typename Compare>
RandomIt
unsafe_partition(RandomIt first, RandomIt last, RandomIt pivot, Compare comp) {
  while (true) {
    while (comp(*first, *pivot))
      ++first;
    --last;
    /* if last is decreased all the way down it will point to first-1
       i.e. the pivot, comp will be false and we will not decrease past
       the lower limit */
    while (comp(*pivot, *last))
      --last;
    if (!(first < last)) /* Comparing iterators not the values pointed at */
      return first;
    jnstl::iter_swap(first, last);
    ++first;
  }
}

/* We split the range [first, last) into two ranges [first, cut) and
   [cut, last) .
   The pivot is the median of three.
*/
template <typename RandomIt, typename Compare>
inline RandomIt
unsafe_partition_pivot(RandomIt first, RandomIt last, Compare comp) {
  RandomIt middle = first + (last - first) / 2;

  jnstl::move_median_to_first(first, first + 1, middle, last -1, comp);
  /* The median value is picked as pivot and moved to first to provide
     a sentinel on the way down. On the way up we are guaranteed a sentinel
     i.e. a value such that !(x < pivot) or x >= pivot, becuase pivot is the
     median of three values, so one is >= pivot by definition. */
  /* The resulting partition does not have the pivot at the cut position.
     This could be achieved by doing one final swap, but it's not
     necessary as the ranges are properly divided at the cut with the exception
     of the pivot. We could achieve that by swap(first, cut-1), cut = cut -1 */
  return jnstl::unsafe_partition(first + 1 , last, first, comp);
}

template <typename RandomIt, typename Compare>
inline void
partial_sort_impl(RandomIt first, RandomIt middle, RandomIt last,
                  Compare comp) {
  jnstl::heap_select(first, middle, last, comp);
  jnstl::sort_heap(first, middle, comp);
}

template <typename RandomIt, typename Size, typename Compare>
void
introsort_loop(RandomIt first, RandomIt last, Size depth_limit, Compare comp) {
  while (last - first > S_threshold) {
    if (depth_limit == 0) {
      jnstl::partial_sort_impl(first, last, last, comp);
      return;
    }
    --depth_limit;
    /* Whether we do a recursive call or go back at the top of the loop we
       are working on a split range, i.e. depth has decreased in both */
    RandomIt cut = jnstl::unsafe_partition_pivot(first, last, comp);
    jnstl::introsort_loop(cut, last, depth_limit, comp);
    last = cut;
  }
}

}  // namespace jnstl

#endif /* JNSTL_SORT_H_ */
