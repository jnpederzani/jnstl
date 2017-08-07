#ifndef JNSTL_VECTOR_H_
#define JNSTL_VECTOR_H_

#include <cstddef>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <memory>

#if JNSTL_EXCEPTIONS_ENABLED
#include <stdexcept>
#endif

#include "JNSTL/bits/config.h"
#include "JNSTL/bits/construct.h"

#include "JNSTL/algorithm.h"
#include "JNSTL/allocator.h"
#include "JNSTL/iterator.h"
#include "JNSTL/memory.h"

namespace jnstl {
template<typename T, typename Allocator>
struct VectorBase {
  typedef Allocator allocator_type;
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;

 protected:
  T*             mBegin;
  T*             mEnd;
  T*             mCapacity;
  allocator_type mAllocator;

 public:
  VectorBase();
  explicit VectorBase(const allocator_type& allocator);
  VectorBase(size_type n, const allocator_type& allocator);

  ~VectorBase();

#if 0
  const allocator_type& get_allocator() const;
  allocator_type&       get_allocator();
  void                  set_allocator(const allocator_type& allocator);
#endif

 protected:
  T*        DoAllocate(size_type n);
  void      DoFree(T* p, size_type n);
  size_type GetNewCapacity(size_type current);
};


template <typename T, typename Allocator = jnstl::allocator>
class vector : private VectorBase<T, Allocator> {
  typedef VectorBase<T, Allocator>                base_type;
  typedef vector<T, Allocator>                    this_type;

 public:
  typedef T                                       value_type;
  typedef T*                                      pointer;
  typedef const T*                                const_pointer;
  typedef T&                                      reference;
  typedef const T&                                const_reference;
  typedef pointer                                 iterator;
  typedef const_pointer                           const_iterator;
  // typedef jnstl::normal_iterator<pointer, vector>       iterator;
  // typedef jnstl::normal_iterator<const_pointer, vector> const_iterator;
  typedef jnstl::reverse_iterator<iterator>       reverse_iterator;
  typedef jnstl::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef typename base_type::size_type           size_type;
  typedef typename base_type::difference_type     difference_type;
  typedef typename base_type::allocator_type      allocator_type;

  using base_type::mBegin;
  using base_type::mEnd;
  using base_type::mCapacity;
  using base_type::mAllocator;
  using base_type::DoAllocate;
  using base_type::DoFree;
  using base_type::GetNewCapacity;

  vector();
  explicit vector(const allocator_type& allocator);
  explicit vector(size_type n,
                  const allocator_type& allocator = allocator_type{});

  vector(size_type n, const value_type& value,
         const allocator_type& allocator = allocator_type{});

  template <typename InputIterator>
  vector(InputIterator first, InputIterator last,
         const allocator_type& allocator = allocator_type{});

  // Copy Constructor
  vector(const this_type& rhs);
  vector(const this_type& rhs, const allocator_type& allocator);
  // Move Constructor
  vector(this_type&& rhs);
  vector(this_type&& rhs, const allocator_type&);

  vector(std::initializer_list<value_type> ilist,
         const allocator_type & allocator = allocator_type{});

  // Destructor
  ~vector();

  // Assignment
  this_type& operator=(const this_type& rhs);

  // Move
  this_type& operator=(this_type&& rhs);

  void swap(this_type& rhs);

        reference operator[](size_type i);
  const_reference operator[](size_type i) const;

        reference at(size_type i);
  const_reference at(size_type i) const;

        value_type* data();
  const value_type* data() const;

  iterator       begin();
  const_iterator begin() const;

  iterator       end();
  const_iterator end() const;

  reverse_iterator       rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator       rend();
  const_reverse_iterator rend() const;

  bool      empty() const;
  size_type size() const;
  size_type capacity() const;

  void resize(size_type n, const value_type& value = value_type{});
  void reserve(size_type n);
  void shrink_to_fit();

  reference       front();
  const_reference front() const;

  reference       back();
  const_reference back() const;

  void     push_back(const value_type&  value);
  void     push_back(      value_type&& value);

  void     pop_back();

  void     emplace_back(const value_type&  value);
  void     emplace_back(      value_type&& value);
  iterator emplace(const_iterator position, const value_type&  value);
  iterator emplace(const_iterator position,       value_type&& value);

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last);
  void assign(size_type n, const value_type& value);
  void assign(std::initializer_list<value_type> il);

  template <typename InputIterator>
  iterator insert(const_iterator position, InputIterator first,
                  InputIterator last);
  iterator insert(const_iterator position, const value_type& value);
  iterator insert(const_iterator position, value_type&& value);
  iterator insert(const_iterator position, size_type n,
                  const value_type& value);
  iterator insert(const_iterator position,
                  std::initializer_list<value_type> il);

  iterator erase(const_iterator position);
  iterator erase(const_iterator first, const_iterator last);

  void clear();

  bool validate() const;

 protected:
  template <typename Integer>
  void DoInit(Integer n, Integer value, std::true_type);

  template <typename InputIterator>
  void DoInit(InputIterator first, InputIterator last, std::false_type);

  template <typename InputIterator>
  void DoInitFromIterator(InputIterator first, InputIterator last,
                          jnstl::input_iterator_tag);

  template <typename ForwardIterator>
  void DoInitFromIterator(ForwardIterator first, ForwardIterator last,
                          jnstl::forward_iterator_tag);


  template <typename Integer>
  void DoAssign(Integer n, Integer value, std::true_type);
  template <typename InputIterator>
  void DoAssign(InputIterator first, InputIterator last, std::false_type);

  void DoAssignValues(size_type n, const value_type& value);

  template <typename InputIterator>
  void DoAssignFromIterator(InputIterator first, InputIterator last,
                            jnstl::input_iterator_tag);

  template <typename ForwardIterator>
  void DoAssignFromIterator(ForwardIterator first, ForwardIterator last,
                            jnstl::forward_iterator_tag);

  template <typename Integer>
  iterator DoInsert(const_iterator position, Integer n, Integer value,
                    std::true_type);

  template <typename InputIterator>
  iterator DoInsert(const_iterator position, InputIterator first,
                    InputIterator last, std::false_type);

  template <typename InputIterator>
  iterator DoInsertFromIterator(const_iterator position, InputIterator first,
                                InputIterator last, jnstl::input_iterator_tag);

  template <typename BidirectionalIterator>
  iterator DoInsertFromIterator(const_iterator position,
                                BidirectionalIterator first,
                                BidirectionalIterator last,
                                jnstl::bidirectional_iterator_tag);

  void     DoInsertValue(const_iterator position, const value_type& value);
  iterator DoInsertValues(const_iterator position, size_type n,
                          const value_type& value);

  void DoInsertValueEnd(const value_type& value);
  void DoInsertValueEnd(value_type&& value);
  void DoInsertValuesEnd(size_type n, const value_type& value);

  void DoGrow(size_type n);
  void DoSwap(this_type& rhs);
};


// VectorBase //

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase()
    : mBegin(nullptr),
      mEnd(nullptr),
      mCapacity(nullptr),
      mAllocator() {}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase(const allocator_type& allocator)
    : mBegin(nullptr),
      mEnd(nullptr),
      mCapacity(nullptr),
      mAllocator(allocator) {}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase(size_type n,
                                            const allocator_type& allocator)
    : mAllocator(allocator) {
  mBegin = DoAllocate(n);
  mEnd = mBegin;
  mCapacity = mBegin + n;
}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::~VectorBase() {
  if (mBegin != nullptr)
    mAllocator.deallocate(static_cast<void *>(mBegin),
                          (mCapacity - mBegin) * sizeof(T));
}

#if 0
template <typename T, typename Allocator>
inline const typename VectorBase<T, Allocator>::allocator_type&
VectorBase<T, Allocator>::get_allocator() const {
  return mAllocator;
}

template <typename T, typename Allocator>
inline typename VectorBase<T, Allocator>::allocator_type&
VectorBase<T, Allocator>::get_allocator() {
  return mAllocator;
}

template <typename T, typename Allocator>
inline void VectorBase<T, Allocator>::set_allocator(
    const allocator_type& allocator) {
  mAllocator = allocator;
}
#endif

template <typename T, typename Allocator>
inline T* VectorBase<T, Allocator>::DoAllocate(size_type n) {
  return static_cast<T*>(mAllocator.allocate(n * sizeof(T)));
}

template <typename T, typename Allocator>
inline void VectorBase<T, Allocator>::DoFree(T* p, size_type n) {
  mAllocator.deallocate(static_cast<void *>(p), n * sizeof(T));
}

template <typename T, typename Allocator>
inline typename VectorBase<T, Allocator>::size_type
VectorBase<T, Allocator>::GetNewCapacity(size_type current) {
  return (size_type)(current > 0) ? (1.5 * current) : 1;
}


// Vector //

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector()
    : base_type() {}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(const allocator_type& allocator)
    : base_type(allocator) {}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type n,
                                    const allocator_type& allocator)
    : base_type(n, allocator) {
  jnstl::uninitialized_default_fill(mBegin, mBegin + n);
  mEnd = mBegin + n;
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type n,
                                    const value_type& value,
                                    const allocator_type& allocator)
    : base_type(n, allocator) {
  jnstl::uninitialized_fill(mBegin, mBegin + n, value);
  mEnd = mBegin + n;
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline vector<T, Allocator>::vector(InputIterator first, InputIterator last,
                                    const allocator_type& allocator)
    : base_type(allocator) {
  DoInit(first, last, std::is_integral<InputIterator>());
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(const this_type& rhs)
    : base_type(rhs.size(), rhs.mAllocator) {
  mEnd = jnstl::uninitialized_copy(rhs.mBegin, rhs.mEnd, mBegin);
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(const this_type& rhs,
                                    const allocator_type& allocator)
    : base_type(rhs.size(), allocator) {
  mEnd = jnstl::uninitialized_copy(rhs.mBegin, rhs.mEnd, mBegin);
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(this_type&& rhs)
    : base_type(LIB::move(rhs.mAllocator)) {
  DoSwap(rhs);
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(this_type&& rhs,
                                    const allocator_type& allocator)
    : base_type(allocator) {
  DoSwap(rhs);
}

template <typename T, typename Allocator>
inline vector<T, Allocator>::vector(std::initializer_list<value_type> ilist,
                                    const allocator_type& allocator)
    : base_type(allocator) {
  DoInit(ilist.begin(), ilist.end(), std::false_type());
}

template<typename T, typename Allocator>
vector<T, Allocator>::~vector() {
  jnstl::Destruct(mBegin, mEnd);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::this_type&
vector<T, Allocator>::operator=(const this_type& rhs) {
  this_type tmp(rhs);

  DoSwap(tmp);
  return *this;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::this_type&
vector<T, Allocator>::operator=(this_type&& rhs) {
  if (this != &rhs) {
    clear();
    DoSwap(rhs);
  }
  return *this;
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::swap(this_type& rhs) {
  DoSwap(rhs);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reference
vector<T, Allocator>::operator[](size_type i) {
  return *(mBegin+i);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reference
vector<T, Allocator>::operator[](size_type i) const {
  return *(mBegin+i);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reference
vector<T, Allocator>::at(size_type i) {
#if JNSTL_EXCEPTIONS_ENABLED
  if (i >= size()) {
    throw;
  }
#endif
  return (*this)[i];
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reference
vector<T, Allocator>::at(size_type i) const {
#if JNSTL_EXCEPTIONS_ENABLED
  if (i >= size()) {
    throw;
  }
#endif
  return (*this)[i];
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::value_type*
vector<T, Allocator>::data() {
  return mBegin;
}

template <typename T, typename Allocator>
inline const typename vector<T, Allocator>::value_type*
vector<T, Allocator>::data() const {
  return mBegin;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::begin() {
  return iterator(mBegin);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_iterator
vector<T, Allocator>::begin() const {
  return const_iterator(mBegin);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::end() {
  return iterator(mEnd);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_iterator
vector<T, Allocator>::end() const {
  return const_iterator(mEnd);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reverse_iterator
vector<T, Allocator>::rbegin() {
  return reverse_iterator(mEnd);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reverse_iterator
vector<T, Allocator>::rbegin() const {
  return const_reverse_iterator(mEnd);;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reverse_iterator
vector<T, Allocator>::rend() {
  return reverse_iterator(mBegin);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reverse_iterator
vector<T, Allocator>::rend() const {
  return const_reverse_iterator(mBegin);
}

template <typename T, typename Allocator>
bool vector<T, Allocator>::empty() const {
  return (mBegin == mEnd);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::size_type
vector<T, Allocator>::size() const {
  return (size_type)(mEnd - mBegin);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::size_type
vector<T, Allocator>::capacity() const {
  return (size_type)(mCapacity - mBegin);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::resize(size_type n, const value_type& value) {
  if (n > (size_type)(mEnd - mBegin)) {
    DoInsertValuesEnd(n - (size_type)(mEnd -mBegin),  value);
  } else {
    jnstl::Destruct(mBegin + n, mEnd);
    mEnd = mBegin + n;
  }
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::reserve(size_type n) {
  if (n > (size_type)(mCapacity - mBegin)) {
    DoGrow(n);
  }
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::shrink_to_fit() {
  if (mEnd < mCapacity) {
    this_type tmp(move_iterator<iterator>(begin()),
                  move_iterator<iterator>(end()), mAllocator);

    DoSwap(tmp);
  }
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reference
vector<T, Allocator>::front() {
  return *mBegin;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reference
vector<T, Allocator>::front() const {
  return *mBegin;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::reference
vector<T, Allocator>::back() {
  return *(mEnd - 1);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::const_reference
vector<T, Allocator>::back() const {
  return *(mEnd - 1);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::push_back(const value_type& value) {
  if (mEnd < mCapacity)
    jnstl::Construct(LIB::addressof(*mEnd++), value);
  else
    DoInsertValueEnd(value);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::push_back(value_type&& value) {
  if (mEnd < mCapacity)
    jnstl::Construct(LIB::addressof(*mEnd++), LIB::move(value));
  else
    DoInsertValueEnd(LIB::move(value));
}

template <typename T, typename Allocator>
inline void
vector<T, Allocator>::pop_back() {
  --mEnd;
  jnstl::Destruct(mEnd);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::emplace_back(const value_type& value) {
  if (mEnd < mCapacity)
    jnstl::Construct(LIB::addressof(*mEnd++), value);
  else
    DoInsertValueEnd(value);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::emplace_back(value_type&& value) {
  if (mEnd < mCapacity)
    jnstl::Construct(LIB::addressof(*mEnd++), LIB::move(value));
  else
    DoInsertValueEnd(LIB::move(value));
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::emplace(const_iterator position,
                              const value_type& value) {
  const ptrdiff_t n = position - mBegin;

  if ((mEnd == mCapacity) || !(position == mEnd)) {
    DoInsertValue(position, value);
  } else {
    jnstl::Construct(LIB::addressof(*mEnd), value);
    mEnd++;
  }

  return mBegin + n;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::emplace(const_iterator position,
                              value_type&& value) {
  const ptrdiff_t n = position - mBegin;

  if ((mEnd == mCapacity) || !(position == mEnd)) {
    DoInsertValue(position, LIB::move(value));
  } else {
    jnstl::Construct(LIB::addressof(*mEnd), LIB::move(value));
    mEnd++;
  }

  return mBegin + n;
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void
vector<T, Allocator>::assign(InputIterator first, InputIterator last) {
  DoAssign(first, last, std::is_integral<InputIterator>());
}

template <typename T, typename Allocator>
inline void
vector<T, Allocator>::assign(std::initializer_list<value_type> il) {
  DoAssign(il.begin(), il.end(), std::false_type());
}

template <typename T, typename Allocator>
inline void
vector<T, Allocator>::assign(size_type n, const value_type& value) {
  DoAssignValues(n, value);
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(const_iterator position, const value_type& value) {
  const ptrdiff_t n = position - mBegin;

  if ((mEnd == mCapacity) || !(position == mEnd)) {
    DoInsertValue(position, value);
  } else {
    jnstl::Construct(LIB::addressof(*mEnd), value);
    mEnd++;
  }

  return mBegin + n;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(const_iterator position, value_type&& value) {
  const ptrdiff_t n = position - mBegin;

  if ((mEnd == mCapacity) || !(position == mEnd)) {
    DoInsertValue(position, LIB::move(value));
  } else {
    jnstl::Construct(LIB::addressof(*mEnd), LIB::move(value));
    mEnd++;
  }

  return mBegin + n;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(const_iterator position, size_type n,
                             const value_type& value) {
  return DoInsertValues(position, n, value);
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(const_iterator position,
                             InputIterator first,
                             InputIterator last) {
  return DoInsert(position, first, last, std::is_integral<InputIterator>());
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(const_iterator position,
                             std::initializer_list<value_type> il) {
  return DoInsert(position, il.begin(), il.end(), std::false_type());
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::erase(const_iterator position) {
  iterator destPosition = const_cast<value_type*>(position);
  if ((position + 1) < mEnd)
    jnstl::move(destPosition + 1, mEnd, destPosition);

  --mEnd;
  mEnd->~value_type();
  return destPosition;
}

template <typename T, typename Allocator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::erase(const_iterator first, const_iterator last) {
  iterator const position =
      const_cast<value_type*>(jnstl::move(const_cast<value_type*>(last), mEnd,
                                          const_cast<value_type*>(first)));

  jnstl::Destruct(position, mEnd);
  mEnd -= (last - first);

  return const_cast<value_type*>(first);
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::clear() {
  jnstl::Destruct(mBegin, mEnd);
  mEnd = mBegin;
}

template <typename T, typename Allocator>
inline bool vector<T, Allocator>::validate() const {
  if (mEnd < mBegin)
    return false;
  if (mCapacity < mEnd)
    return false;

  return true;
}

template <typename T, typename Allocator>
template <typename Integer>
inline void vector<T, Allocator>::DoInit(Integer n, Integer value,
                                         std::true_type) {
  mBegin    = DoAllocate((size_type)n);
  mEnd      = mBegin + n;
  mCapacity = mEnd;
  jnstl::uninitialized_fill(mBegin, mBegin + n, value);
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void vector<T, Allocator>::DoInit(InputIterator first,
                                         InputIterator last,
                                         std::false_type) {
  typedef typename
      jnstl::iterator_traits<InputIterator>::iterator_category IC;

  DoInitFromIterator(first, last, IC());
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void vector<T, Allocator>::DoInitFromIterator(
    InputIterator first, InputIterator last, jnstl::input_iterator_tag) {
  for (; first != last; ++first)
    push_back(*first);
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
inline void vector<T, Allocator>::DoInitFromIterator(
    ForwardIterator first, ForwardIterator last,
    jnstl::forward_iterator_tag) {
  const size_type n = static_cast<size_type>(jnstl::distance(first, last));
  mBegin = DoAllocate(n);
  mCapacity = mBegin + n;
  mEnd = jnstl::uninitialized_copy(first, last, mBegin);
}

template <typename T, typename Allocator>
template <typename Integer>
inline void
vector<T, Allocator>::DoAssign(Integer n, Integer value,
                               std::true_type) {
  DoAssignValues(static_cast<size_type>(n), static_cast<value_type>(value));
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void
vector<T, Allocator>::DoAssign(InputIterator first, InputIterator last,
                               std::false_type) {
  typedef typename
      jnstl::iterator_traits<InputIterator>::iterator_category IC;

  DoAssignFromIterator(first, last, IC());
}

template <typename T, typename Allocator>
void
vector<T, Allocator>::DoAssignValues(size_type n, const value_type& value) {
  if (n > static_cast<size_type>(mCapacity - mBegin)) {
    const size_type nPrev = static_cast<size_type>(mEnd - mBegin);
    const size_type nGrow = GetNewCapacity(nPrev);
    const size_type nNew  = jnstl::max(nGrow, n);

    pointer const pNewData = DoAllocate(nNew);

    jnstl::uninitialized_fill(pNewData, pNewData + n, value);

    jnstl::Destruct(mBegin, mEnd);
    DoFree(mBegin, static_cast<size_type>(mCapacity - mBegin));

    mBegin    = pNewData;
    mEnd      = pNewData + n;
    mCapacity = pNewData + nNew;
  } else if (size() >= n) {
    jnstl::fill(mBegin, mBegin + n, value);
    jnstl::Destruct(mBegin+n, mEnd);
    mEnd = mBegin + n;
  } else {
    jnstl::fill(mBegin, mEnd, value);
    jnstl::uninitialized_fill(mEnd, mBegin + n, value);
    mEnd = mBegin + n;
  }
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void
vector<T, Allocator>::DoAssignFromIterator(InputIterator first,
                                           InputIterator last,
                                           jnstl::input_iterator_tag) {
  iterator cur = begin();

  for (; first != last && cur != end(); ++first, ++cur) {
    *cur = *first;
  }

  if (first == last) {
    jnstl::Destruct(cur, mEnd);
    mEnd = cur;
  } else {
    jnstl::uninitialized_copy(first, last, cur);
  }
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
void
vector<T, Allocator>::DoAssignFromIterator(ForwardIterator first,
                                           ForwardIterator last,
                                           jnstl::forward_iterator_tag) {
  const size_type n = static_cast<size_type>(jnstl::distance(first, last));

  if (n > static_cast<size_type>(mCapacity - mBegin)) {
    const size_type nPrev = static_cast<size_type>(mEnd - mBegin);
    const size_type nGrow = GetNewCapacity(nPrev);
    const size_type nNew  = jnstl::max(nGrow, n);

    pointer const pNewData = DoAllocate(nNew);
    pointer       pNewEnd  = pNewData;

    pNewEnd = jnstl::uninitialized_copy(first, last, pNewData);

    jnstl::Destruct(mBegin, mEnd);
    DoFree(mBegin, static_cast<size_type>(mCapacity - mBegin));

    mBegin    = pNewData;
    mEnd      = pNewEnd;
    mCapacity = pNewData + nNew;

  } else if (size() >= n) {
    jnstl::copy(first, last, mBegin);
    jnstl::Destruct(mBegin + n, mEnd);
    mEnd = mBegin + n;
  } else {
    ForwardIterator mid = first;

           jnstl::advance(mid, size());
           jnstl::copy(first, mid, mBegin);
    mEnd = jnstl::uninitialized_copy(mid, last, mEnd);
  }
}

template <typename T, typename Allocator>
template <typename Integer>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::DoInsert(const_iterator position,
                               Integer n, Integer value,
                               std::true_type) {
  if (n > 0)
    return DoInsertValues(position, static_cast<size_type>(n),
                          static_cast<value_type>(value));
  else
    return iterator(position);
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::DoInsert(const_iterator position,
                               InputIterator first,
                               InputIterator last,
                               std::false_type) {
  typedef typename
      jnstl::iterator_traits<InputIterator>::iterator_category IC;

  return DoInsertFromIterator(position, first, last, IC());
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline typename vector<T, Allocator>::iterator
vector<T, Allocator>::DoInsertFromIterator(const_iterator position,
                                           InputIterator first,
                                           InputIterator last,
                                           jnstl::input_iterator_tag) {
  for (; first != last; ++first, ++position)
    position = insert(position, *first);

  return position;
}

template <typename T, typename Allocator>
template <typename BidirectionalIterator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::DoInsertFromIterator(const_iterator position,
                                           BidirectionalIterator first,
                                           BidirectionalIterator last,
                                           jnstl::bidirectional_iterator_tag) {
  iterator destPosition = const_cast<value_type*>(position);

  if (first != last) {
    const size_type n = static_cast<size_type>(jnstl::distance(first, last));

    if (n <= static_cast<size_type>(mCapacity - mEnd)) {
      const size_type nExtra = static_cast<size_type>(mEnd - destPosition);

      if (n < nExtra) {
        jnstl::uninitialized_move(mEnd - n, mEnd, mEnd);
        jnstl::move_backward(destPosition, mEnd - n, mEnd);
        jnstl::copy(first, last, destPosition);
      } else {
        BidirectionalIterator iTemp = first;
        jnstl::advance(iTemp, nExtra);
        jnstl::uninitialized_copy(iTemp, last, mEnd);
        jnstl::uninitialized_move(destPosition, mEnd, mEnd + n - nExtra);
        jnstl::copy_backward(first, iTemp, destPosition + nExtra);
      }

      mEnd += n;
    } else {
      const size_type nPrev = (size_type)(mEnd - mBegin);
      const size_type nGrow = GetNewCapacity(nPrev);
      const size_type nNew  = jnstl::max(nGrow, n);

      pointer const pNewData = DoAllocate(nNew);

      pointer pNewEnd = pNewData;

      pNewEnd = jnstl::uninitialized_move(mBegin, destPosition, pNewData);
      pNewEnd = jnstl::uninitialized_copy(first, last, pNewEnd);
      pNewEnd = jnstl::uninitialized_move(destPosition, mEnd, pNewEnd);

      jnstl::Destruct(mBegin, mEnd);
      DoFree(mBegin, (size_type)(mCapacity - mBegin));

      mBegin    = pNewData;
      mEnd      = pNewEnd;
      mCapacity = pNewData + nNew;
    }
  }
  return mEnd;
}

template <typename T, typename Allocator>
void vector<T, Allocator>::DoInsertValue(const_iterator position,
                                         const value_type& value) {
  iterator destPosition = const_cast<value_type*>(position);
  if (mEnd != mCapacity) {
    const T* pValue = &value;

    if ((pValue >= destPosition) && (pValue < mEnd))
      ++pValue;

    jnstl::Construct(LIB::addressof(*mEnd), *(mEnd - 1));
    jnstl::move_backward(destPosition, mEnd - 1, mEnd);

    *destPosition = *pValue;

    ++mEnd;
  } else {
    const size_type nPrev = (size_type)(mEnd - mBegin);
    const size_type nGrow = GetNewCapacity(nPrev);
    const size_type nNew  = jnstl::max(nGrow, nPrev + 1);

    pointer const pNewData = DoAllocate(nNew);

    pointer pNewEnd = pNewData;

    jnstl::Construct(LIB::addressof(*(pNewData +
                                      (size_type)(destPosition - mBegin))),
                     value);

    pNewEnd = jnstl::uninitialized_move(mBegin, destPosition, pNewData);

    pNewEnd = jnstl::uninitialized_move(destPosition, mEnd, ++pNewEnd);

    jnstl::Destruct(mBegin, mEnd);
    DoFree(mBegin, (size_type)(mCapacity - mBegin));

    mBegin    = pNewData;
    mEnd      = pNewEnd;
    mCapacity = pNewData + nNew;
  }
}

template <typename T, typename Allocator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::DoInsertValues(const_iterator position,
                                     size_type n,
                                     const value_type& value) {
  iterator destPosition = const_cast<value_type*>(position);

  if (n <= (size_type)(mCapacity - mEnd)) {
    const value_type temp = value;
    const size_type nExtra = static_cast<size_type>(mEnd - destPosition);

    if (n < nExtra) {
      jnstl::uninitialized_move(mEnd - n, mEnd, mEnd);
      jnstl::move_backward(destPosition, mEnd - n, mEnd);
      jnstl::fill(destPosition, destPosition + n, temp);
    } else {
      jnstl::uninitialized_fill(mEnd, mEnd + n - nExtra, temp);
      jnstl::uninitialized_move(destPosition, mEnd, mEnd + n - nExtra);
      jnstl::fill(destPosition, mEnd, temp);
    }

    mEnd += n;
  } else {
    const size_type nPrev = (size_type)(mEnd - mBegin);
    const size_type nGrow = GetNewCapacity(nPrev);
    const size_type nNew  = jnstl::max(nGrow, n + nPrev);

    pointer const pNewData = DoAllocate(nNew);

    pointer pNewEnd = pNewData;

    pNewEnd = jnstl::uninitialized_move(mBegin, destPosition, pNewData);
    jnstl::uninitialized_fill(pNewEnd, pNewEnd + n, value);
    pNewEnd = jnstl::uninitialized_move(destPosition, mEnd, pNewEnd + n);

    jnstl::Destruct(mBegin, mEnd);
    DoFree(mBegin, (size_type)(mCapacity - mBegin));

    mBegin    = pNewData;
    mEnd      = pNewEnd;
    mCapacity = pNewData + nNew;
  }
  return mEnd;
}

template <typename T, typename Allocator>
void vector<T, Allocator>::DoInsertValueEnd(const value_type& value) {
  const size_type nPrev = (size_type)(mEnd - mBegin);
  const size_type nGrow = GetNewCapacity(nPrev);
  const size_type nNew  = jnstl::max(nGrow, nPrev + 1);

  pointer const pNewData = DoAllocate(nNew);

  pointer pNewEnd = jnstl::uninitialized_move(mBegin, mEnd, pNewData);

  jnstl::Construct(LIB::addressof(*pNewEnd++), value);

  jnstl::Destruct(mBegin, mEnd);
  DoFree(mBegin, (size_type)(mCapacity - mBegin));

  mBegin    = pNewData;
  mEnd      = pNewEnd;
  mCapacity = pNewData + nNew;
}

template <typename T, typename Allocator>
void vector<T, Allocator>::DoInsertValueEnd(value_type&& value) {
  const size_type nPrev = (size_type)(mEnd - mBegin);
  const size_type nGrow = GetNewCapacity(nPrev);
  const size_type nNew  = jnstl::max(nGrow, nPrev + 1);

  pointer const pNewData = DoAllocate(nNew);

  pointer pNewEnd = jnstl::uninitialized_move(mBegin, mEnd, pNewData);

  jnstl::Construct(LIB::addressof(*pNewEnd++), LIB::move(value));

  jnstl::Destruct(mBegin, mEnd);
  DoFree(mBegin, (size_type)(mCapacity - mBegin));

  mBegin    = pNewData;
  mEnd      = pNewEnd;
  mCapacity = pNewData + nNew;
}

template <typename T, typename Allocator>
void vector<T, Allocator>::DoInsertValuesEnd(size_type n,
                                             const value_type& value) {
  if (n > (size_type)(mCapacity - mEnd)) {
    const size_type nPrev = (size_type)(mEnd - mBegin);
    const size_type nGrow = GetNewCapacity(nPrev);
    const size_type nNew  = jnstl::max(nGrow, nPrev + n);

    pointer const pNewData = DoAllocate(nNew);

    pointer pNewEnd = jnstl::uninitialized_move(mBegin, mEnd, pNewData);

    jnstl::uninitialized_fill(pNewEnd, pNewEnd + n, value);
    pNewEnd += n;

    jnstl::Destruct(mBegin, mEnd);
    DoFree(mBegin, (size_type)(mCapacity - mBegin));

    mBegin    = pNewData;
    mEnd      = pNewEnd;
    mCapacity = pNewData + nNew;
  } else {
    jnstl::uninitialized_fill(mEnd, mEnd + n, value);
    mEnd += n;
  }
}

template <typename T, typename Allocator>
void vector<T, Allocator>::DoGrow(size_type n) {
  const size_type nPrev = (size_type)(mEnd - mBegin);
  const size_type nGrow = GetNewCapacity(nPrev);
  const size_type nNew  = jnstl::max(nGrow, n);

  pointer const pNewData = DoAllocate(nNew);

  pointer pNewEnd = jnstl::uninitialized_move(mBegin, mEnd, pNewData);

  jnstl::Destruct(mBegin, mEnd);
  DoFree(mBegin, (size_type)(mCapacity - mBegin));

  mBegin    = pNewData;
  mEnd      = pNewEnd;
  mCapacity = pNewData + nNew;
}

template <typename T, typename Allocator>
inline void vector<T, Allocator>::DoSwap(this_type& rhs) {
  LIB::swap(mBegin,     rhs.mBegin);
  LIB::swap(mEnd,       rhs.mEnd);
  LIB::swap(mCapacity,  rhs.mCapacity);
  LIB::swap(mAllocator, rhs.mAllocator);
}

// Global //
template <typename T, typename Allocator>
inline bool operator==(const vector<T, Allocator>& a,
                       const vector<T, Allocator>& b) {
  return ((a.size() == b.size()) &&
          jnstl::equal(a.begin(), a.end(), b.begin()));
}

template <typename T, typename Allocator>
inline bool operator<(const vector<T, Allocator>& a,
                      const vector<T, Allocator>& b) {
  return std::lexicographical_compare(a.begin(), a.end(),
                                      b.begin(), b.end());
}

template <typename T, typename Allocator>
inline bool operator!=(const vector<T, Allocator>& a,
                       const vector<T, Allocator>& b) {
  return !(a == b);
}

template <typename T, typename Allocator>
inline bool operator>(const vector<T, Allocator>& a,
                      const vector<T, Allocator>& b) {
  return b < a;
}

template <typename T, typename Allocator>
inline bool operator<=(const vector<T, Allocator>& a,
                       const vector<T, Allocator>& b) {
  return !(b < a);
}

template <typename T, typename Allocator>
inline bool operator>=(const vector<T, Allocator>& a,
                       const vector<T, Allocator>& b) {
  return !(a < b);
}

template <typename T, typename Allocator>
inline void swap(vector<T, Allocator>& a, vector<T, Allocator>& b) {
  a.swap(b);
}
}  // namespace jnstl

#endif /* JNSTL_VECTOR_H_ */
