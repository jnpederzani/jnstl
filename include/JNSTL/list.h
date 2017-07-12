#ifndef JNSTL_LIST_H
#define JNSTL_LIST_H

#include <cstddef>
#include <utility>
#include <functional>
#include <type_traits>
#include <algorithm>

#include "JNSTL/bits/config.h"

#include "JNSTL/allocator.h"
#include "JNSTL/iterator.h"
#include "JNSTL/algorithm.h"

namespace jnstl {
template <typename T>
struct ListNode {
  ListNode* mNext;
  ListNode* mPrev;
  T         mValue;

  static void swap(ListNode& lhs, ListNode& rhs);
  void insert(ListNode* pNext);
  void remove();
  void splice(ListNode* pFirst, ListNode* pLast);
  void reverse();
  void insert_range(ListNode* pFirst, ListNode* pLast);
  static void remove_range(ListNode* pFirst, ListNode* pLast);
};

template <typename T>
struct ListIterator {
  typedef ListIterator<T>                        this_type;
  typedef ListNode<T>                            node_type;

  typedef size_t                                 size_type;
  typedef ptrdiff_t                              difference_type;
  typedef T                                      value_type;
  typedef T*                                     pointer;
  typedef T&                                     reference;
  typedef jnstl::bidirectional_iterator_tag      iterator_category;

 public:
  node_type* mNode;

  ListIterator()
      : mNode(nullptr) {}

  explicit
  ListIterator(const node_type* pNode)
      : mNode(const_cast<node_type*>(pNode)) {}

  reference operator*()  const {
    return mNode->mValue;
  }
  pointer   operator->() const {
    return &mNode->mValue;
  }

  this_type& operator++() {
    mNode = mNode->mNext;
    return *this;
  }
  this_type  operator++(int) {
    this_type temp(*this);
    mNode = mNode->mNext;
    return temp;
  }

  this_type& operator--() {
    mNode = mNode->mPrev;
    return *this;
  }

  this_type  operator--(int) {
    this_type temp(*this);
    mNode = mNode->mPrev;
    return temp;
  }

  bool operator==(const this_type& x) const {
    return mNode == x.mNode;
  }

  bool operator!=(const this_type& x) const {
    return mNode != x.mNode;
  }
};

template <typename T>
struct ListConstIterator {
  typedef ListConstIterator<T>                   this_type;
  typedef const ListNode<T>                      node_type;
  typedef ListIterator<T>                        iterator;

  typedef size_t                                 size_type;
  typedef ptrdiff_t                              difference_type;
  typedef T                                      value_type;
  typedef const T*                               pointer;
  typedef const T&                               reference;
  typedef jnstl::bidirectional_iterator_tag      iterator_category;

 public:
  node_type* mNode;

  ListConstIterator()
      : mNode(nullptr) {}
  explicit
  ListConstIterator(const node_type* pNode)
      : mNode(pNode) {}
  ListConstIterator(const iterator& rhs)
      : mNode(rhs.mNode) {}

  reference operator*()  const {
    return mNode->mValue;
  }
  pointer   operator->() const {
    return &mNode->mValue;
  }

  this_type& operator++() {
    mNode = mNode->mNext;
    return *this;
  }
  this_type  operator++(int) {
    this_type temp(*this);
    mNode = mNode->mNext;
    return temp;
  }

  this_type& operator--() {
    mNode = mNode->mPrev;
    return *this;
  }

  this_type  operator--(int) {
    this_type temp(*this);
    mNode = mNode->mPrev;
    return temp;
  }

  bool operator==(const this_type& x) const {
    return mNode == x.mNode;
  }

  bool operator!=(const this_type& x) const {
    return mNode != x.mNode;
  }
};

template <typename T, typename Allocator>
class ListBase {
 public:
  typedef T           value_type;
  typedef Allocator   allocator_type;
  typedef ListNode<T> node_type;
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;

 protected:
  node_type     mNode;
  allocator_type mAllocator;

 public:
  ListBase();
  explicit ListBase(const allocator_type& allocator);

  ~ListBase();

 protected:
  node_type* DoAllocateNode();
  void       DoFreeNode(node_type * pNode);
  void       DoInit();
  void       DoClear();
};

template <typename T, typename Allocator = jnstl::allocator>
class list : private ListBase<T, Allocator> {
  typedef ListBase<T, Allocator>                  base_type;
  typedef list<T, Allocator>                      this_type;


 public:
  typedef T                                       value_type;
  typedef T*                                      pointer;
  typedef const T*                                const_pointer;
  typedef T&                                      reference;
  typedef const T&                                const_reference;
  typedef ListIterator<T>                         iterator;
  typedef ListConstIterator<T>                    const_iterator;
  typedef typename base_type::size_type           size_type;
  typedef typename base_type::difference_type     difference_type;
  typedef typename base_type::allocator_type      allocator_type;
  typedef typename base_type::node_type           node_type;

  using base_type::mNode;
  using base_type::mAllocator;
  using base_type::DoAllocateNode;
  using base_type::DoFreeNode;
  using base_type::DoInit;
  using base_type::DoClear;

  list();
  explicit list(const allocator_type& allocator);
  explicit list(size_type n,
                const allocator_type& allocator = allocator_type{});
  list(size_type n, const value_type& value,
       const allocator_type& allocator = allocator_type{});

  template <typename InputIterator>
  list(InputIterator first, InputIterator last,
       const allocator_type& allocator = allocator_type{});

  list(std::initializer_list<value_type> ilist,
         const allocator_type& allocator = allocator_type{});

  list(const this_type& rhs);
  list(const this_type& rhs, const allocator_type& allocator);
  list(this_type&& rhs);
  list(this_type&& rhs, const allocator_type&);
  //  ~list();

  this_type& operator=(const this_type& rhs);
  this_type& operator=(this_type&& rhs);

  void swap(this_type& rhs);

  iterator       begin();
  const_iterator begin() const;

  iterator       end();
  const_iterator end() const;

  bool      empty() const;
  size_type size() const;

  iterator erase(const_iterator position);
  iterator erase(const_iterator first, const_iterator last);

  void resize(size_type n, const value_type& value = value_type());

  reference       front();
  const_reference front() const;

  reference       back();
  const_reference back() const;

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last);
  void assign(size_type n, const value_type& val);
  void assign(std::initializer_list<value_type> ilist);

  void push_front(const value_type& value);
  void push_front(value_type&& value);

  void push_back(const value_type& value);
  void push_back(value_type&& value);

  void pop_front();
  void pop_back();

  iterator emplace(const_iterator position, value_type&& value);
  iterator emplace(const_iterator position, const value_type& value);

  iterator insert(const_iterator position,
                  const value_type& value = value_type());
  iterator insert(const_iterator position, size_type n,
                  const value_type& value);
  template <typename InputIterator>
  iterator insert(const_iterator position, InputIterator first,
                  InputIterator last);
  iterator insert(const_iterator position, value_type&& value);
  iterator insert(const_iterator position,
                  std::initializer_list<value_type> ilist);

  void clear();



  void splice(const_iterator position, this_type& x);
  void splice(const_iterator position, this_type& x, const_iterator i);
  void splice(const_iterator position, this_type& x,
              const_iterator first, const_iterator last);

  void remove(const value_type& value);

  void unique();

  void merge(this_type& x);
  template <typename Compare>
  void merge(this_type& x, Compare compare);

  void sort();
  template <typename Compare>
  void sort(Compare compare);

  void reverse();

  bool validate() const;
  int  validate_iterator(const_iterator i) const;

 protected:
  node_type* DoCreateNode(value_type&& value);
  node_type* DoCreateNode(const value_type& value);

  template <typename Integer>
  void DoAssign(Integer n, Integer value, std::true_type);

  template <typename InputIterator>
  void DoAssign(InputIterator first, InputIterator last, std::false_type);

  void DoAssignValues(size_type n, const value_type& value);

  template <typename Integer>
  void DoInsert(node_type* pNode, Integer n, Integer value, std::true_type);

  template <typename InputIterator>
  void DoInsert(node_type* pNode, InputIterator first, InputIterator last,
                std::false_type);

  void DoInsertValues(node_type* pNode, size_type n, const value_type& value);
  void DoInsertValue(node_type* pNode, value_type&& value);
  void DoInsertValue(node_type* pNode, const value_type& value);

  void DoErase(node_type* pNode);

  void DoSwap(this_type& rhs);

  template <typename Compare>
  iterator DoSort(iterator a, iterator b, size_type n, Compare& compare);
};

// ListNode //

template <typename T>
inline void ListNode<T>::swap(ListNode& lhs, ListNode& rhs) {
  const ListNode temp(lhs);

  lhs = rhs;
  rhs = temp;

  if (lhs.mNext == &rhs) {  // rhs was empty //
    lhs.mNext = &lhs;
    lhs.mPrev = &lhs;
  } else {
    lhs.mNext->mPrev = &lhs;
    lhs.mPrev->mNext = &lhs;
  }

  if (rhs.mNext == &lhs) {  // lhs was empty //
    rhs.mNext = &rhs;
    rhs.mPrev = &rhs;
  } else {
    rhs.mNext->mPrev = &rhs;
    rhs.mPrev->mNext = &rhs;
  }
}

template <typename T>
inline void ListNode<T>::insert(ListNode<T>* pNext) {
  mNext = pNext;
  mPrev = pNext->mPrev;
  mPrev->mNext = this;
  pNext->mPrev = this;
}

template <typename T>
inline void ListNode<T>::remove() {
  mNext->mPrev = mPrev;
  mPrev->mNext = mNext;
}

template <typename T>
inline void ListNode<T>::splice(ListNode<T>* pFirst, ListNode<T>* pLast) {
  pLast->mPrev->mNext = this;
  pFirst->mPrev->mNext = pLast;
  this->mPrev->mNext = pFirst;

  ListNode<T>* const pTemp = this->mPrev;
  this->mPrev = pLast->mPrev;
  pLast->mPrev = pFirst->mPrev;
  pFirst->mPrev = pTemp;
}

template <typename T>
inline void ListNode<T>::reverse() {
  ListNode<T>* pRun = this;

  do {
    ListNode<T>* const pTemp = pRun->mNext;

    pRun->mNext = pRun->mPrev;
    pRun->mPrev = pTemp;
    pRun = pRun->mPrev;
  } while (pRun != this);
}

template <typename T>
inline void ListNode<T>::insert_range(ListNode<T>* pFirst, ListNode<T>* pLast) {
  pFirst->mPrev = mPrev;
  pLast->mNext = this;

  mPrev->mNext = pFirst;
  mPrev = pLast;
}

template <typename T>
inline void ListNode<T>::remove_range(ListNode<T>* pFirst, ListNode<T>* pLast) {
  pFirst->mPrev->mNext = pLast->mNext;
  pLast->mNext->mPrev = pFirst->mPrev;
}

// ListBase //

template <typename T, typename Allocator>
inline ListBase<T, Allocator>::ListBase()
    : mNode(),
      mAllocator(allocator_type{}) {
  DoInit();
}

template <typename T, typename Allocator>
inline ListBase<T, Allocator>::ListBase(const allocator_type& allocator)
    : mNode(),
      mAllocator(allocator) {
  DoInit();
}

template <typename T, typename Allocator>
inline ListBase<T, Allocator>::~ListBase() {
  DoClear();
}

template <typename T, typename Allocator>
inline typename ListBase<T, Allocator>::node_type*
ListBase<T, Allocator>::DoAllocateNode() {
  return static_cast<node_type*>(mAllocator.allocate(sizeof(node_type)));
}

template <typename T, typename Allocator>
inline void ListBase<T, Allocator>::DoFreeNode(node_type* p) {
  mAllocator.deallocate(static_cast<void *>(p), sizeof(node_type));
}

template <typename T, typename Allocator>
inline void ListBase<T, Allocator>::DoInit() {
  mNode.mNext = &mNode;
  mNode.mPrev = &mNode;
}

template <typename T, typename Allocator>
inline void ListBase<T, Allocator>::DoClear() {
  node_type* p = mNode.mNext;

  while (p != &mNode) {
    node_type* const pTemp = p;
    p = p->mNext;
    pTemp->~node_type();
    mAllocator.deallocate(static_cast<void *>(pTemp), sizeof(node_type));
  }
}

// list //

template <typename T, typename Allocator>
inline list<T, Allocator>::list()
    : base_type() {}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(const allocator_type& allocator)
    : base_type(allocator) {}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(size_type n, const allocator_type& allocator)
    : base_type(allocator) {
  DoInsertValues(&mNode, n, value_type());
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(size_type n, const value_type& value,
                              const allocator_type& allocator)
    : base_type(allocator) {
  DoInsertValues(&mNode, n, value);
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline list<T, Allocator>::list(InputIterator first, InputIterator last,
                                const allocator_type& allocator)
    : base_type(allocator) {
  DoInsert(&mNode, first, last, std::is_integral<InputIterator>());
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(std::initializer_list<value_type> ilist,
                                const allocator_type& allocator)
    : base_type(allocator) {
  DoInsert(&mNode, ilist.begin(), ilist.end(), std::false_type());
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(const this_type& rhs)
    : base_type(rhs.mAllocator) {
  DoInsert(&mNode, rhs.begin(), rhs.end(), std::false_type());
}

template <typename T, typename Allocator>
inline
list<T, Allocator>::list(const this_type& rhs, const allocator_type& allocator)
    : base_type(allocator) {
  DoInsert(&mNode, rhs.begin(), rhs.end(), std::false_type());
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(this_type&& rhs)
    : base_type(LIB::move(rhs.mAllocator)) {
  swap(rhs);
}

template <typename T, typename Allocator>
inline
list<T, Allocator>::list(this_type&& rhs, const allocator_type& allocator)
    : base_type(allocator) {
  swap(rhs);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::this_type&
list<T, Allocator>::operator=(const this_type& rhs) {
  if (this != &rhs) {
    if (mAllocator != rhs.mAllocator) {
      clear();
      mAllocator = rhs.mAllocator;
    }

    DoAssign(rhs.begin(), rhs.end(), std::false_type());
  }

  return *this;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::this_type&
list<T, Allocator>::operator=(this_type&& rhs) {
  if (this != &rhs) {
    clear();
    swap(rhs);
  }

  return *this;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::swap(this_type& rhs) {
  if (mAllocator == rhs.mAllocator) {
    DoSwap(rhs);
  } else {
    this_type temp(*this);

    *this = rhs;
    rhs = temp;
  }
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::begin() {
  return iterator(mNode.mNext);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator
list<T, Allocator>::begin() const {
  return const_iterator(mNode.mNext);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::end() {
  return iterator(&mNode);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator
list<T, Allocator>::end() const {
  return const_iterator(&mNode);
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::empty() const {
  return mNode.mNext == &mNode;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::size_type
list<T, Allocator>::size() const {
  return static_cast<size_type>(jnstl::distance(const_iterator(mNode.mNext),
                                                const_iterator(&mNode)));
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::erase(const_iterator position) {
  ++position;
  DoErase(position.mNode->mPrev);

  return iterator(position.mNode);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::erase(const_iterator first, const_iterator last) {
  while (first != last)
    first = erase(first);

  return iterator(last.mNode);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::resize(size_type n, const value_type& value) {
  iterator position(mNode.mNext);
  size_type i = 0;

  while (position.mNode != &mNode && i < n) {
    ++position;
    ++i;
  }

  if (i == n) {
    erase(position, end());
  } else {
    insert(end(), n - i, value);
  }
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reference
list<T, Allocator>::front() {
  return mNode.mNext->mValue;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reference
list<T, Allocator>::front() const {
  return mNode.mNext->mValue;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reference
list<T, Allocator>::back() {
  return mNode.mPrev->mValue;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reference
list<T, Allocator>::back() const {
  return mNode.mPrev->mValue;
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::push_front(const value_type& value) {
  DoInsertValue(mNode.mNext, value);
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::push_front(value_type&& value) {
  emplace(begin(), LIB::move(value));
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::push_back(const value_type& value) {
  DoInsertValue(&mNode, value);
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::push_back(value_type&& value) {
  emplace(end(), LIB::move(value));
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::pop_front() {
  DoErase(mNode.mNext);
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::pop_back() {
  DoErase(mNode.mPrev);
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::assign(size_type n, const value_type& value) {
  DoAssignValues(n, value);
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void
list<T, Allocator>::assign(InputIterator first, InputIterator last) {
  DoAssign(first, last, std::is_integral<InputIterator>());
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::assign(std::initializer_list<value_type> ilist) {
  DoAssign(ilist.begin(), ilist.end(), std::false_type());
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::emplace(const_iterator position, value_type&& value) {
  DoInsertValue(const_cast<node_type*>(position.mNode), LIB::move(value));
  return iterator(position.mNode->mPrev);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::emplace(const_iterator position, const value_type& value) {
  DoInsertValue(const_cast<node_type*>(position.mNode), value);
  return iterator(position.mNode->mPrev);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::insert(const_iterator position, const value_type& value) {
  iterator it(position.mNode);
  --it;
  DoInsertValue(const_cast<node_type*>(position.mNode), value);
  return ++it;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::insert(const_iterator position, size_type n,
                           const value_type& value) {
  iterator it(position.mNode);
  --it;
  DoInsertValues(const_cast<node_type*>(position.mNode), n, value);
  return ++it;
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::insert(const_iterator position, InputIterator first,
                           InputIterator last) {
  iterator it(position.mNode);
  --it;
  DoInsert(const_cast<node_type*>(position.mNode),
           first, last, std::is_integral<InputIterator>());
  return ++it;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::insert(const_iterator position, value_type&& value) {
  return emplace(position, LIB::move(value));
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::insert(const_iterator position,
                           std::initializer_list<value_type> ilist) {
  iterator it(position.mNode);
  --it;
  DoInsert(const_cast<node_type*>(position.mNode),
           ilist.begin(), ilist.end(), std::false_type());
  return ++it;
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::splice(const_iterator position, this_type& x) {
  if (mAllocator == x.mAllocator) {
    if (!x.empty())
      const_cast<node_type*>(position.mNode)->splice(x.mNode.mNext, &x.mNode);
  } else {
    insert(position, x.begin(), x.end());
    x.clear();
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::splice(const_iterator position, this_type& x,
                           const_iterator i) {
  if (mAllocator == x.mAllocator) {
    iterator i2(i.mNode);
    ++i2;
    if (position != i && position != i2)
      const_cast<node_type*>(position.mNode)->splice(
          const_cast<node_type*>(i.mNode), i2.mNode);
  } else {
    insert(position, *i);
    x.erase(i);
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::splice(const_iterator position, this_type& x,
                           const_iterator first, const_iterator last) {
  if (mAllocator == x.mAllocator) {
    if (first != last)
      const_cast<node_type*>(position.mNode)->splice(
          const_cast<node_type*>(first.mNode),
          const_cast<node_type*>(last.mNode));
  } else {
    insert(position, first, last);
    x.erase(first, last);
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::remove(const value_type& value) {
  node_type *pNode = mNode.mNext;
  while (pNode != &mNode) {
    if (pNode->mValue == value) {
      pNode = pNode->mNext;
      DoErase(pNode->mPrev);
    } else {
      pNode = pNode->mNext;
    }
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::unique() {
  node_type *pNode = mNode.mNext->mNext;
  while (pNode != &mNode) {
    if (pNode->mValue == pNode->mPrev->mValue) {
      pNode = pNode->mNext;
      DoErase(pNode->mPrev);
    } else {
      pNode = pNode->mNext;
    }
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::merge(this_type& x) {
  std::less<value_type> compare;
  merge(x, compare);
}

template <typename T, typename Allocator>
template <typename Compare>
inline void
list<T, Allocator>::merge(this_type& x, Compare compare) {
  iterator first(begin());
  const iterator last(end());
  iterator xfirst(x.begin());
  const iterator xlast(x.end());

  if (this != &x) {
    while (first != last && xfirst != last) {
      if (compare(*xfirst, *first)) {
        ++xfirst;
        splice(first, x, iterator(xfirst.mNode->mPrev));
      } else {
        ++first;
      }
    }

    if (xfirst != xlast)
      splice(last, x, xfirst, xlast);
  }
}

template <typename T, typename Allocator>
inline void
list<T, Allocator>::sort() {
  std::less<value_type> compare;
  DoSort(begin(), end(), size(), compare);
}

template <typename T, typename Allocator>
template <typename Compare>
inline void
list<T, Allocator>::sort(Compare compare) {
  DoSort(begin(), end(), size(), compare);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::reverse() {
  mNode.reverse();
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::clear() {
  DoClear();
  DoInit();
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::validate() const {
  return true;
}

template <typename T, typename Allocator>
inline int list<T, Allocator>::validate_iterator(const_iterator i) const {
  const_iterator temp = begin();
  const_iterator tempEnd = end();
  for (; temp != tempEnd; ++ temp) {
    if (temp == i)
      return (isf_valid | isf_current | isf_can_dereference);
  }
  if (i == tempEnd)
    return (isf_valid | isf_current);

  return isf_none;
}

template <typename T, typename Allocator>
typename list<T, Allocator>::node_type*
list<T, Allocator>::DoCreateNode(value_type&& value) {
  node_type* const pNode = DoAllocateNode();

#if JNSTL_EXCEPTIONS_ENABLED
  try {
    ::new(static_cast<void*>(&(pNode->mValue))) value_type(LIB::move(value));
  }
  catch (...) {
    DoFreeNode(pNode);
    throw;
  }
#else
  ::new(static_cast<void*>(&(pNode->mValue))) value_type(LIB::move(value));
#endif

  return pNode;
}

template <typename T, typename Allocator>
typename list<T, Allocator>::node_type*
list<T, Allocator>::DoCreateNode(const value_type& value) {
  node_type* const pNode = DoAllocateNode();

#if JNSTL_EXCEPTIONS_ENABLED
  try {
    ::new(static_cast<void*>(&(pNode->mValue))) value_type(value);
  }
  catch (...) {
    DoFreeNode(pNode);
    throw;
  }
#else
  ::new(static_cast<void*>(&(pNode->mValue))) value_type(value);
#endif

  return pNode;
}

template <typename T, typename Allocator>
template <typename Integer>
inline void list<T, Allocator>::DoAssign(Integer n, Integer value,
                                         std::true_type) {
  DoAssignValues(static_cast<size_type>(n), static_cast<value_type>(value));
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void list<T, Allocator>::DoAssign(InputIterator first,
                                         InputIterator last,
                                         std::false_type) {
  node_type* pNode = mNode.mNext;

  for (; first != last && pNode != &mNode; ++first) {
    pNode->mValue = *first;
    pNode = pNode->mNext;
  }

  if (first != last)
    DoInsert(&mNode, first, last, std::false_type());
  else
    erase(const_iterator(pNode), const_iterator(&mNode));
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::DoAssignValues(size_type n,
                                               const value_type& value) {
  node_type* pNode = mNode.mNext;

  for (; n > 0 && pNode != &mNode; --n) {
    pNode->mValue = value;
    pNode = pNode->mNext;
  }

  if (n > 0)
    DoInsertValues(&mNode, n, value);
  else
    erase(const_iterator(pNode), const_iterator(&mNode));
}

template <typename T, typename Allocator>
template <typename Integer>
inline void list<T, Allocator>::DoInsert(node_type* pNode, Integer n,
                                         Integer value, std::true_type) {
  DoInsertValues(pNode, static_cast<size_type>(n),
                 static_cast<value_type>(value));
}

template <typename T, typename Allocator>
template <typename InputIterator>
inline void list<T, Allocator>::DoInsert(node_type* pNode, InputIterator first,
                                         InputIterator last, std::false_type) {
  for (; first != last; ++first)
    DoInsertValue(pNode, *first);
}


template <typename T, typename Allocator>
inline void list<T, Allocator>::DoInsertValues(node_type *pNode, size_type n,
                                               const value_type& value) {
  for (; n > 0; --n) {
    DoInsertValue(pNode, value);
  }
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::DoInsertValue(node_type *pNode,
                                              value_type&& value) {
  node_type * pNewNode = DoCreateNode(LIB::move(value));
  pNewNode->insert(pNode);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::DoInsertValue(node_type *pNode,
                                              const value_type& value) {
  node_type * pNewNode = DoCreateNode(value);
  pNewNode->insert(pNode);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::DoErase(node_type* pNode) {
  pNode->remove();
  pNode->~node_type();
  DoFreeNode(pNode);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::DoSwap(this_type& rhs) {
  ListNode<T>::swap(mNode, rhs.mNode);
  LIB::swap(mAllocator, rhs.mAllocator);
}

template <typename T, typename Allocator>
template <typename Compare>
typename list<T, Allocator>::iterator
list<T, Allocator>::DoSort(iterator lo1, iterator hi2, size_type n,
                           Compare& compare) {
  switch (n) {
    case 0:
    case 1:
      return lo1;
    case 2:
      if (compare(*--hi2, *lo1)) {
        hi2.mNode->remove();
        hi2.mNode->insert(lo1.mNode);

        return hi2;
      }
      return lo1;
  }

  size_type nMid = n/2;
  iterator hi1(lo1);

  advance(hi1, nMid);

           lo1 = DoSort(lo1, hi1, nMid, compare);
  iterator lo2 = DoSort(hi1, hi2, n - nMid, compare);

  iterator start;

  if (compare(*lo2, *lo1)) {
    iterator curr(lo2.mNode->mNext);

    while (curr != hi2 && compare(*curr, *lo1))
      ++curr;

    node_type* hCut = curr.mNode->mPrev;

    node_type::remove_range(lo2.mNode, hCut);
    lo1.mNode->insert_range(lo2.mNode, hCut);

    start = lo2;
    lo2 = curr;
    hi1 = lo2;

  } else {
    start = lo1;
    hi1 = lo2;
  }

  for (++lo1; (lo1 != hi1) && (lo2 != hi2); ++lo1) {
    if (compare(*lo2, *lo1)) {
      iterator curr(lo2.mNode->mNext);

      while (curr != hi2 && compare(*curr, *lo1))
        ++curr;

      node_type* hCut = curr.mNode->mPrev;

      node_type::remove_range(lo2.mNode, hCut);
      lo1.mNode->insert_range(lo2.mNode, hCut);

      if (hi1 == lo2)
        hi1 = curr;
      lo2 = curr;
    }
  }
  return start;
}

// Global //

template <typename T>
inline bool operator==(const ListIterator<T>& a,
                       const ListIterator<T>& b) {
  return a.mNode == b.mNode;
}

template <typename T>
inline bool operator!=(const ListIterator<T>& a,
                       const ListIterator<T>& b) {
  return !(a == b);
}

template <typename T>
inline bool operator==(const ListConstIterator<T>& a,
                       const ListConstIterator<T>& b) {
  return a.mNode == b.mNode;
}

template <typename T>
inline bool operator!=(const ListConstIterator<T>& a,
                       const ListConstIterator<T>& b) {
  return !(a == b);
}

template <typename T, typename Allocator>
inline bool operator==(const list<T, Allocator>& a,
                       const list<T, Allocator>& b) {
  return equal(a.begin(), a.end(), b.begin());
}

template <typename T, typename Allocator>
inline bool operator<(const list<T, Allocator>& a,
                      const list<T, Allocator>& b) {
  return std::lexicographical_compare(a.begin(), a.end(),
                                      b.begin(), b.end());
}

template <typename T, typename Allocator>
inline bool operator!=(const list<T, Allocator>& a,
                       const list<T, Allocator>& b) {
  return !(a == b);
}

template <typename T, typename Allocator>
inline bool operator>(const list<T, Allocator>& a,
                      const list<T, Allocator>& b) {
  return b < a;
}

template <typename T, typename Allocator>
inline bool operator<=(const list<T, Allocator>& a,
                       const list<T, Allocator>& b) {
  return !(b < a);
}

template <typename T, typename Allocator>
inline bool operator>=(const list<T, Allocator>& a,
                       const list<T, Allocator>& b) {
  return !(a < b);
}

template <typename T, typename Allocator>
void swap(list<T, Allocator>& a, list<T, Allocator>& b) {
  a.swap(b);
}

}  // namespace jnstl
#endif  // JNSTL_LIST_H //
