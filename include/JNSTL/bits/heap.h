#ifndef JNSTL_HEAP_H_
#define JNSTL_HEAP_H_

#include "JNSTL/bits/config.h"

namespace jnstl {
/* Start from Idx, go up to parents and if < push back the parent, otherwhise
   found position to insert */
template <typename RandomIt, typename Distance, typename T, typename Compare>
void
promote_heap(RandomIt first, Distance Idx, Distance topIdx,
             T val, Compare comp) {
  Distance parentIdx = (Idx - 1) / 2;

  for (; Idx > topIdx && comp(*(first + parentIdx), val);
       parentIdx = (Idx - 1) /2) {
    *(first + Idx) = LIB::move(*(first + parentIdx));
    Idx = parentIdx;
  }
  *(first + Idx) = LIB::move(val);
}


template<typename RandomIt, typename Distance, typename T, typename Compare>
void
adjust_heap(RandomIt first, Distance Idx, Distance heapSize,
            T val, Compare comp) {
  /* Conventional approach of moving Idx down by swapping it with it biggest
     child (irrespective of it's comparison with the children) then go back
     up comparing val to it's parent and swap accordignly.
     Other approach would be to compare val with the children and swap only
     if needed and then call adjust_heap on the swapped child */
  const Distance topIdx = Idx;
  Distance rIdx = 2 * Idx + 2;
  Distance lIdx = rIdx - 1;

  while (rIdx < heapSize) {
    if (comp(*(first + rIdx), *(first + lIdx))) { /* pick largest child */
      *(first + Idx) = LIB::move(*(first + lIdx));
      Idx = lIdx;
      rIdx = 2 * Idx + 2;
      lIdx = rIdx - 1;
    } else {
      *(first + Idx) = LIB::move(*(first + rIdx));
      Idx = rIdx;
      rIdx = 2 * Idx + 2;
      lIdx = rIdx - 1;
    }
  }

  /* we are at the bottom of the tree, if rIdx is 1 past the end pick lIdx.
     This is needed because in the while loop above we only check for rIdx,
     causing to skip the last two children if rIdx == heapSize and lIdx ==
     heaSize - 1. */
  if (rIdx == heapSize) {
    *(first + Idx) = LIB::move(*(first + lIdx));
   Idx = lIdx;
  }
  /* pushed Idx to the bottom now put it back in at top to ensure proper head */
  jnstl::promote_heap(first, Idx, topIdx, LIB::move(val), comp);
}

template <typename RandomIt, typename Compare>
inline void
pop_heap_impl(RandomIt first, RandomIt last, RandomIt result, Compare comp) {
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;
  typedef typename
      jnstl::iterator_traits<RandomIt>::difference_type difference_type;

    value_type val = LIB::move(*result);
    *result = LIB::move(*first);
    /* Element at the top of the heap is now at the end of the heap.
       Now to to adjust the heap by inserting the value (val) that was in
       last position. */
    jnstl::adjust_heap(first, difference_type(0), difference_type(last - first),
                       LIB::move(val), comp);
}

template <typename RandomIt, typename Compare>
void
make_heap_impl(RandomIt first, RandomIt last, Compare comp) {
  /* Bottom up construction */
  typedef typename jnstl::iterator_traits<RandomIt>::value_type value_type;
  typedef typename
      jnstl::iterator_traits<RandomIt>::difference_type difference_type;

  if (last - first > 1) {
    const difference_type heapSize = last - first;
    const difference_type firstLeafIdx = (heapSize - 1) / 2 + 1;
    /* All Idxs >= than firstLeafIdx are leafs and automatically satisfy
       the heap property. Pick the first parent before firstLeafIdx and
       proceed up the tree adjusting the heap */
    difference_type parentIdx = firstLeafIdx - 1;
    while (parentIdx >= 0) {
      value_type val = LIB::move(*(first + parentIdx));

      jnstl::adjust_heap(first, parentIdx, heapSize, LIB::move(val), comp);
      --parentIdx;
    }
  }
}

template <typename RandomIt, typename Compare>
void
heap_select(RandomIt first, RandomIt middle, RandomIt last, Compare comp) {
  jnstl::make_heap_impl(first, middle, comp);
  for (RandomIt it = middle; it != last; ++it) {
    if (comp(*it, *first))
      jnstl::pop_heap_impl(first, middle, it, comp);
  }
}
}  // namespace jnstl

#endif /* JNSTL_HEAP_H_ */
