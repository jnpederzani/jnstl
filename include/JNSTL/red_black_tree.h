#ifndef JNSTL_RBTREE_H
#define JNSTL_RBTREE_H

#include <cstddef>
#include <utility>
#include <functional>
#include <type_traits>
#include <algorithm>

#include "JNSTL/bits/config.h"

#include "JNSTL/allocator.h"
#include "JNSTL/iterator.h"
#include "JNSTL/algorithm.h"
#include "JNSTL/utility.h"

namespace jnstl {

enum rbtree_color {
  mRed,
  mBlack
};

struct rbtree_node_base {
  typedef       rbtree_node_base          node_base_type;
  typedef const rbtree_node_base    const_node_base_type;

  node_base_type*    mRight;
  node_base_type*    mLeft;
  node_base_type*    mParent;
  rbtree_color       mColor;

  static void
  swap(node_base_type& x, node_base_type& y);

  static node_base_type*
  sMinimum(node_base_type* x) {
    while(x->mLeft != nullptr)
      x = x->mLeft;
    return x;
  }

  static const_node_base_type*
  sMinimum(const_node_base_type* x) {
    while(x->mLeft != nullptr)
      x = x->mLeft;
    return x;
  }

  static node_base_type*
  sMaximum(node_base_type* x) {
    while(x->mRight != nullptr)
      x = x->mRight;
    return x;
  }

  static const_node_base_type*
  sMaximum(const_node_base_type* x) {
    while(x->mRight != nullptr)
      x = x->mRight;
    return x;
  }
};

#if JNSTL_BEBUG_RBTREE_NODE
template <typename TN>
struct rbtree_node_base_proxy {
  TN*           mRight;
  TN*           mLeft;
  TN*           mParent;
  rbtree_color mColor;
};

template <typename T>
struct rbtree_node : public rbtree_node_base_proxy< rbtree_node<T> > {
  T mValue;
};
#else
template <typename T>
struct rbtree_node : public rbtree_node_base{
  T mValue;
};
#endif

rbtree_node_base*
RBTreeIncrement(const rbtree_node_base* pNode);

rbtree_node_base*
RBTreeDecrement(const rbtree_node_base* pNode);

size_t
RBTreeBlackCount(rbtree_node_base* pTop,
                    rbtree_node_base* pBottom);

void
RBTreeInsert(rbtree_node_base* pNew, rbtree_node_base* pParent,
             rbtree_node_base* mHeader, bool insert_left);

void
RBTreeErase(rbtree_node_base* pNode, rbtree_node_base* mHeader);

template <typename T>
struct rbtree_iterator {
  typedef T   value_type;
  typedef T*  pointer;
  typedef T&  reference;

  typedef rbtree_iterator<T>  this_type;
  typedef rbtree_node_base    base_node_type;
  typedef rbtree_node<T>      node_type;
  typedef ptrdiff_t           difference_type;

  typedef jnstl::bidirectional_iterator_tag iterator_category;

  node_type* mNode;

  rbtree_iterator()
      : mNode(nullptr) {}

  explicit
  rbtree_iterator(node_type* x)
      : mNode(static_cast<node_type*>(const_cast<node_type*>(x))) {}

  reference
  operator*() const {
    return mNode->mValue;
  }

  pointer
  operator->() const {
    return &mNode->mValue;
  }

  this_type&
  operator++() {
    mNode = static_cast<node_type*>(RBTreeIncrement(mNode));
    return *this;
  }

  this_type
  operator++(int) {
    this_type temp(*this);
    mNode = static_cast<node_type*>(RBTreeIncrement(mNode));
    return temp;
  }

  this_type&
  operator--() {
    mNode = static_cast<node_type*>(RBTreeDecrement(mNode));
    return *this;
  }

  this_type
  operator--(int) {
    this_type temp(*this);
    mNode = static_cast<node_type*>(RBTreeDecrement(mNode));
    return temp;
  }

  bool
  operator==(const this_type& x) const {
    return mNode == x.mNode;
  }

  bool
  operator!=(const this_type& x) const {
    return mNode != x.mNode;
  }
};

template <typename T>
struct rbtree_const_iterator {
  typedef T         value_type;
  typedef const T*  pointer;
  typedef const T&  reference;

  typedef rbtree_const_iterator<T>  this_type;
  typedef rbtree_iterator<T>        iterator;
  typedef rbtree_node_base          base_node_type;
  typedef const rbtree_node<T>      node_type;
  typedef ptrdiff_t                 difference_type;

  typedef jnstl::bidirectional_iterator_tag iterator_category;

  node_type* mNode;

  rbtree_const_iterator()
      : mNode(nullptr) {}

  explicit
  rbtree_const_iterator(node_type* x)
      : mNode(static_cast<node_type*>(const_cast<node_type*>(x))) {}

  rbtree_const_iterator(const iterator& x)
      : mNode(x.mNode) {}

  reference
  operator*() const {
    return mNode->mValue;
  }

  pointer
  operator->() const {
    return &mNode->mValue;
  }

  this_type&
  operator++() {
    mNode =static_cast<node_type*>(RBTreeIncrement(mNode));
    return *this;
  }

  this_type
  operator++(int) {
    this_type temp(*this);
    mNode =static_cast<node_type*>(RBTreeIncrement(mNode));
    return temp;
  }

  this_type&
  operator--() {
    mNode =static_cast<node_type*>(RBTreeDecrement(mNode));
    return *this;
  }

  this_type
  operator--(int) {
    this_type temp(*this);
    mNode =static_cast<node_type*>(RBTreeDecrement(mNode));
    return temp;
  }

  bool
  operator==(const this_type& x) const {
    return mNode == x.mNode;
  }

  bool
  operator!=(const this_type& x) const {
    return mNode != x.mNode;
  }
};

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator = jnstl::allocator>
class rbtree {
  typedef rbtree<Key, T, KeyOfT,
                 Compare, Allocator>      this_type;
  typedef       rbtree_node_base                node_base_type;
  typedef const rbtree_node_base          const_node_base_type;
  typedef       rbtree_node_base*               node_base_type_ptr;
  typedef const rbtree_node_base*         const_node_base_type_ptr;
 public:
  typedef Key                             key_type;
  typedef T                               value_type;
  typedef       rbtree_node<T>                  node_type;
  typedef const rbtree_node<T>            const_node_type;
  //typedef       rbtree_node<T>*                 node_type_ptr;
  typedef        value_type*                     pointer;
  typedef const value_type*               const_pointer;
  typedef       value_type&                     reference;
  typedef const value_type&               const_reference;
  typedef rbtree_iterator<T>                    iterator;
  typedef rbtree_const_iterator<T>        const_iterator;
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef Allocator                       allocator_type;


  rbtree();

  rbtree(const allocator_type& allocator);
  rbtree(const Compare& compare,
         const allocator_type& allocator = allocator_type{});

  //  template <typename InputIterator>
  //rbtree(InputIterator first, InputIterator last, const Compare& compare,
  //       const allocator_type& allocator = allocator_type{});

  rbtree(const this_type& x);
  rbtree(this_type&& x);
  rbtree(this_type&& x, const allocator_type& allocator);

  ~rbtree();


  this_type& operator=(const this_type& x);
  this_type& operator=(this_type&& x);

  Compare key_comp() const;

        iterator begin();
  const_iterator begin() const;
        iterator   end();
  const_iterator   end() const;

  bool      empty() const;
  size_type  size() const;

  void swap(this_type& x);

  void clear();

        iterator find(const key_type& x);
  const_iterator find(const key_type& x) const;

        iterator lower_bound(const key_type& x);
  const_iterator lower_bound(const key_type& x) const;

        iterator upper_bound(const key_type& x);
  const_iterator upper_bound(const key_type& x) const;

  iterator  erase(      iterator position);
  iterator  erase(const_iterator position);

  iterator  erase(const_iterator first, const_iterator last);
  void      erase(const key_type* first, const key_type* last);

  size_type erase(const key_type& x);

  pair<      iterator,       iterator> equal_range(const key_type& key);
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

  bool validate() const;
  bool validate_iterator(const_iterator i) const;

  pair<iterator, bool> DoInsertUnique(const value_type& value);
              iterator DoInsertUnique(const_iterator position,
                                      const value_type& value);

  template<typename InputIterator>
  void DoInsertUnique(InputIterator first, InputIterator last);

  iterator DoInsertMulti(const value_type& value);
  iterator DoInsertMulti(const_iterator position, const value_type& value);

  template<typename InputIterator>
  void DoInsertMulti(InputIterator first, InputIterator last);

  template<typename InputIterator>
  void DoAssignUnique(InputIterator first, InputIterator last);

  template<typename InputIterator>
  void DoAssignMulti(InputIterator first, InputIterator last);

 protected:
  template <typename KeyComp>
  struct rbtree_impl {
    KeyComp          KeyCompare;
    rbtree_node_base mHeader;
    size_type        mSize;
    allocator_type   mAllocator;

    rbtree_impl()
        : KeyCompare(), mHeader(), mSize(0), mAllocator() {
      DoInit();
    }

    rbtree_impl(const Compare& compare, const allocator_type& allocator)
        : KeyCompare(compare), mHeader(), mSize(0), mAllocator(allocator) {
      DoInit();
    }

    rbtree_impl(const Compare& compare, allocator_type&& allocator)
        : KeyCompare(compare), mHeader(), mSize(0),
          mAllocator(LIB::move(allocator)) {
      DoInit();
    }

    void reset() {
      mHeader.mRight  = &mHeader;
      mHeader.mLeft   = &mHeader;
      mHeader.mParent = nullptr;
      mSize           = 0;
    }
   private:
    void
    DoInit() {
      mHeader.mRight  = &mHeader;
      mHeader.mLeft   = &mHeader;
      mHeader.mParent = nullptr;
      mHeader.mColor  = mRed;
    }
  };

  rbtree_impl<Compare> mImpl;

 private:
  node_type* DoAllocateNode();
  void       DoFreeNode(node_type* pNode);

  node_type* DoCreateNode(const value_type& value);
  node_type* DoCreateNode(const node_type* pSource, node_type* pParent);
  void       DoDestroyNode(node_type* pNode);

  node_type* DoCreateTree(const node_type* pSource, node_type* pParent);
  void       DoDestroyTree(node_type* pTop);

  node_base_type*
  FindEqual(node_base_type_ptr& pPparent, const key_type& key);

  node_base_type*
  FindMulti(node_base_type_ptr& pParent, const key_type& key);

  node_base_type*
  FindEqual(const_iterator position, node_base_type_ptr& pParent,
            node_base_type_ptr& dummy, const key_type& key);

  iterator
  LowerBound(node_type* first, node_type* last, const key_type& key);

  const_iterator
  LowerBound(const node_type* first, const node_type* last,
             const key_type& key) const;

  iterator
  UpperBound(node_type* first, node_type* last, const key_type& key);

  const_iterator
  UpperBound(const node_type* first, const node_type* last,
             const key_type& key) const;

  node_base_type*
  FindLeafLow(node_base_type_ptr& pParent, const key_type& key);

  node_base_type*
  FindLeaf(const_iterator position, node_base_type_ptr& pParent,
           const key_type& key);

  iterator
  DoInsert(node_base_type* pParent, node_base_type* pChild,
           const value_type& value);

  void DoSwap(this_type& x);

  node_base_type_ptr&
  mRoot() {
    return this->mImpl.mHeader.mParent;
  }

  const_node_base_type*
  mRoot() const {
    return this->mImpl.mHeader.mParent;
  }

  node_base_type_ptr&
  mLeftMost() {
    return this->mImpl.mHeader.mLeft;
  }

  const_node_base_type*
  mLeftMost() const {
    return this->mImpl.mHeader.mLeft;
  }

  node_base_type_ptr&
  mRightMost() {
    return this->mImpl.mHeader.mRight;
  }

  const_node_base_type*
  mRightMost() const {
    return this->mImpl.mHeader.mRight;
  }

  node_type*
  mBegin() {
    return static_cast<node_type*>(this->mImpl.mHeader.mParent);
  }

  const_node_type*
  mBegin() const {
    return static_cast<const_node_type*>(this->mImpl.mHeader.mParent);
  }

  node_type*
  mEnd() {
    return reinterpret_cast<node_type*>(&this->mImpl.mHeader);
  }

  const_node_type*
  mEnd() const {
    return reinterpret_cast<const_node_type*>(&this->mImpl.mHeader);
  }

  static const_reference
  sValue(const_node_type* x) {
    return x->mValue;
  }

  static const Key&
  sKey(const_node_type* x) {
    return KeyOfT()(sValue(x));
  }

  static const_reference
  sValue(const_node_base_type* x) {
    return static_cast<const_node_type*>(x)->mValue;
  }

  static const Key&
  sKey(const_node_base_type* x) {
    return KeyOfT()(sValue(x));
  }

  static node_base_type*
  sMinimum(node_base_type* x) {
    return rbtree_node_base::sMinimum(x);
  }

  static const_node_base_type*
  sMinimum(const_node_base_type* x) {
    return rbtree_node_base::sMinimum(x);
  }

  static node_base_type*
  sMaximum(node_base_type* x) {
    return rbtree_node_base::sMaximum(x);
  }

  static const_node_base_type*
  sMaximum(const_node_base_type* x) {
    return rbtree_node_base::sMaximum(x);
  }

  static node_type*
  sLeft(node_base_type* x) {
    return static_cast<node_type*>(x->mLeft);
  }

  static const_node_type*
  sLeft(const_node_base_type* x) {
    return static_cast<const_node_type*>(x->mLeft);
  }

  static node_type*
  sRight(node_base_type* x) {
    return static_cast<node_type*>(x->mRight);
  }

  static const_node_type*
  sRight(const_node_base_type* x) {
    return static_cast<const_node_type*>(x->mRight);
  }
};

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree()
    : mImpl() {}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
    const allocator_type& allocator)
    : mImpl(Compare(), allocator) {}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
    const Compare& compare, const allocator_type& allocator)
    : mImpl(compare, allocator) {}

//template <typename Key, typename T, typename KeyOfT,
//          typename Compare, typename Allocator>
//template <typename InputIterator>
//inline
//rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
//    InputIterator first, InputIterator last,
//    const Compare& compare, const allocator_type& allocator)
//    : mImpl(compare, allocator) {
//
//
//}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
    const this_type& x)
    : mImpl(x.mImpl.KeyCompare, x.mImpl.mAllocator) {
  if (x.mRoot() != nullptr) {
    this->mRoot()      = DoCreateTree(x.mBegin(), this->mEnd());
    this->mLeftMost()  = sMinimum(this->mRoot());
    this->mRightMost() = sMaximum(this->mRoot());
    this->mImpl.mSize  = x.mImpl.mSize;
  }
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
    this_type&& x)
    : mImpl(x.mImpl.KeyCompare, x.mImpl.mAllocator) {
  swap(x);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::rbtree(
    this_type&& x, const allocator_type& allocator)
    : mImpl(x.mImpl.KeyCompare, allocator) {
  swap(x);
}
template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline
rbtree<Key, T, KeyOfT, Compare, Allocator>::~rbtree() {
  DoDestroyTree(this->mBegin());
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::this_type&
rbtree<Key, T, KeyOfT, Compare, Allocator>::operator=(const this_type& x) {
  if (*this != x) {
    clear();

    mImpl.mAllocator = x.mImpl.mAllocator;
    mImpl.KeyCompare = x.mImpl.KeyCompare;

    if (x.mRoot() != nullptr) {
      this->mRoot()      = DoCreateTree(x.mBegin(), this->mEnd());
      this->mLeftMost()  = sMinimum(this->mRoot());
      this->mRightMost() = sMaximum(this->mRoot());
      this->mImpl.mSize  = x.mImpl.mSize;
    }
  }
  return *this;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::this_type&
rbtree<Key, T, KeyOfT, Compare, Allocator>::operator=(this_type&& x) {
  if (*this != x) {
    clear();
    swap(x);
  }
  return *this;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline Compare
rbtree<Key, T, KeyOfT, Compare, Allocator>::key_comp() const {
  return mImpl.KeyCompare;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::begin() {
  return iterator(static_cast<node_type*>(mImpl.mHeader.mLeft));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::begin() const {
  return const_iterator(static_cast<node_type*>
                        (const_cast<node_base_type*>(mImpl.mHeader.mLeft)));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::end() {
  return iterator(static_cast<node_type*>(&mImpl.mHeader));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::end() const {
  return const_iterator(static_cast<node_type*>
                        (const_cast<node_base_type*>(&mImpl.mHeader)));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
rbtree<Key, T, KeyOfT, Compare, Allocator>::empty() const {
  return mImpl.mSize == 0;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::size_type
rbtree<Key, T, KeyOfT, Compare, Allocator>::size() const {
  return mImpl.mSize;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
rbtree<Key, T, KeyOfT, Compare, Allocator>::swap(this_type& x) {
  if (this->mImpl.mAllocator == x.mImpl.mAllocator) {
    DoSwap(x);
  } else {
    this_type tmp(*this);

    *this = x;
    x   = tmp;
  }
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::erase(
    const_iterator position) {
  iterator it_erase = iterator(const_cast<node_type*>(position.mNode));
  ++position;
  RBTreeErase(it_erase.mNode, &mImpl.mHeader);
  DoDestroyNode(it_erase.mNode);
  --mImpl.mSize;
  return iterator(const_cast<node_type*>(position.mNode));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::erase(
    const_iterator first, const_iterator last) {
  //int error = 0;
  while (first != last) {
    first = erase(first);
    //if (!validate())
    //error++;
  }
  return iterator(const_cast<node_type*>(first.mNode));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::size_type
rbtree<Key, T, KeyOfT, Compare, Allocator>::erase(
    const key_type& key) {
  pair<iterator, iterator> p = equal_range(key);
  const size_type old_size = size();
  //int error = 0;
  erase(p.first, p.second);
  //if (!validate())
  //error++;
  return old_size - size();
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
rbtree<Key, T, KeyOfT, Compare, Allocator>::erase(
    const key_type* first, const key_type* last) {
  while (first != last)
    erase(*first++);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
rbtree<Key, T, KeyOfT, Compare, Allocator>::clear() {
  DoDestroyTree(this->mBegin());
  mImpl.reset();
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::find(
    const key_type& key) {
  iterator x = LowerBound(mBegin(), mEnd(), key);

  if (x == end() || mImpl.KeyCompare(key, sKey(x.mNode)))
    return end();
  else
    return x;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::find(
    const key_type& key) const {
  const_iterator x = LowerBound(mBegin(), mEnd(), key);

  if (x == end() || mImpl.KeyCompare(key, sKey(x.mNode)))
    return end();
  else
    return x;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::lower_bound(
    const key_type& key) {
  return LowerBound(mBegin(), mEnd(), key);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::lower_bound(
    const key_type& key) const {
  return LowerBound(mBegin(), mEnd(), key);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::upper_bound(
    const key_type& key) {
  return UpperBound(mBegin(), mEnd(), key);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::upper_bound(
    const key_type& key) const {
  return UpperBound(mBegin(), mEnd(), key);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
pair<typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator,
     typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator>
rbtree<Key, T, KeyOfT, Compare, Allocator>::equal_range(
    const key_type& key) {
  node_type* x = mBegin();
  node_type* y = mEnd();

  while (x != nullptr) {
    if (mImpl.KeyCompare(sKey(x), key)) {
        x = sRight(x);
    }
    else if (mImpl.KeyCompare(key, sKey(x))) {
      y = x;
      x = sLeft(x);
    }
    else {
      node_type* xu(x);
      node_type* yu(y);
      y = x;
      x = sLeft(x);
      xu = sRight(xu);
      return pair<iterator, iterator>(LowerBound(x, y, key),
                                      UpperBound(xu, yu, key));
    }
  }
  // unique values
  return pair<iterator, iterator>(iterator(y), iterator(y));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
pair<typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator,
     typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator>
rbtree<Key, T, KeyOfT, Compare, Allocator>::equal_range(
    const key_type& key) const {
  const_node_type* x = mBegin();
  const_node_type* y = mEnd();

  while (x != nullptr) {
    if (mImpl.KeyCompare(sKey(x), key)) {
        x = sRight(x);
    }
    else if (mImpl.KeyCompare(key, sKey(x))) {
      y = x;
      x = sLeft(x);
    }
    else {
      const_node_type* xu(x);
      const_node_type* yu(y);
      y = x;
      x = sLeft(x);
      xu = sRight(xu);
      return pair<const_iterator, const_iterator>(LowerBound(x, y, key),
                                                  UpperBound(xu, yu, key));
    }
  }
  // unique values
  return pair<const_iterator, const_iterator>(const_iterator(y),
                                              const_iterator(y));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
bool
rbtree<Key, T, KeyOfT, Compare, Allocator>::validate() const {
  if(mImpl.mSize) {
    if(mImpl.mHeader.mLeft != sMinimum(mImpl.mHeader.mParent))
      return false;

    if(mImpl.mHeader.mRight != sMaximum(mImpl.mHeader.mParent))
      return false;

    size_t nBlackCount = RBTreeBlackCount(mImpl.mHeader.mParent,
                                             mImpl.mHeader.mLeft);

    if (RBTreeBlackCount(mImpl.mHeader.mParent, mImpl.mHeader.mRight) !=
        nBlackCount)
      return false;

    size_type     nIteratedSize = 0;

    for(const_iterator it = begin(); it != end(); ++it, ++nIteratedSize)
    {
      const node_type* const pNode      = (const node_type*)it.mNode;
      const node_type* const pNodeRight = (const node_type*)pNode->mRight;
      const node_type* const pNodeLeft  = (const node_type*)pNode->mLeft;

      if(pNodeRight && mImpl.KeyCompare(sKey(pNodeRight),
                                        sKey(pNode)) &&
         mImpl.KeyCompare(sKey(pNode),
                          sKey(pNodeRight)))
        return false;

      if(pNodeLeft && mImpl.KeyCompare(sKey(pNodeLeft),
                                       sKey(pNode)) &&
         mImpl.KeyCompare(sKey(pNode),
                          sKey(pNodeLeft)))
        return false;

      // Verify item #1 above.
      if((pNode->mColor != mRed) && (pNode->mColor != mBlack))
        return false;

      // Verify item #3 above.
      if(pNode->mColor == mRed)
      {
        if((pNodeRight && (pNodeRight->mColor == mRed)) ||
           (pNodeLeft  && (pNodeLeft->mColor  == mRed)))
          return false;
      }

      // Verify item #6 above.
      if(pNodeRight && mImpl.KeyCompare(sKey(pNodeRight),
                                        sKey(pNode)))
        return false;

      if(pNodeLeft && mImpl.KeyCompare(sKey(pNode),
                                       sKey(pNodeLeft)))
        return false;

      if(!pNodeRight && !pNodeLeft) // If we are at a bottom node of the tree...
      {
        // Verify item #4 above.
        node_base_type* y = static_cast<node_base_type*>(const_cast<node_type*>(pNode));
        if(RBTreeBlackCount(mImpl.mHeader.mParent, y) != nBlackCount)
          return false;
      }
    }

    // Verify item #5 above.
    if(nIteratedSize != mImpl.mSize)
      return false;

    return true;
  }
  else
  {
    if((mImpl.mHeader.mLeft != &mImpl.mHeader) ||
       (mImpl.mHeader.mRight != &mImpl.mHeader))
      return false;
  }

  return true;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
bool
rbtree<Key, T, KeyOfT, Compare, Allocator>::validate_iterator(const_iterator i) const {
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

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
pair<typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator,bool>
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertUnique(
    const value_type& value) {
  node_base_type_ptr parent;
  node_base_type* child = FindEqual(parent, KeyOfT()(value));
  bool inserted = false;
  iterator it(static_cast<node_type*>(child));

  // if child != nullptr value already exists
  if (child == nullptr) {
    it = DoInsert(parent, child, value);
    inserted = true;
  }

  return pair<iterator, bool>(it, inserted);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertUnique(
    const_iterator position, const value_type& value) {
  node_base_type_ptr parent;
  node_base_type_ptr dummy;
  node_base_type* child = FindEqual(position, parent, dummy,
                                    KeyOfT()(value));
  iterator it(static_cast<node_type*>(child));

  if (child == nullptr) {
    it = DoInsert(parent, child, value);
  }

  return it;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
template<typename InputIterator>
void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertUnique(
    InputIterator first, InputIterator last) {
  for (; first != last; ++first)
    DoInsertUnique(end(), *first);
  //DoInsertUnique(*first);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertMulti(
    const value_type& value) {
  node_base_type_ptr parent;
  node_base_type* child = FindMulti(parent, KeyOfT()(value));

  return iterator(DoInsert(parent, child, value));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertMulti(
    const_iterator position, const value_type& value) {
  node_base_type_ptr parent;
  node_base_type* child = FindLeaf(position, parent, KeyOfT()(value));

  return iterator(DoInsert(parent, child, value));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
template<typename InputIterator>
void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsertMulti(
    InputIterator first, InputIterator last) {
  for (; first != last; ++first)
    DoInsertMulti(end(), *first);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
template<typename InputIterator>
void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoAssignUnique(
    InputIterator first, InputIterator last) {
  clear();
  for (; first != last; ++first)
    DoInsertUnique(end(), *first);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
template<typename InputIterator>
void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoAssignMulti(
    InputIterator first, InputIterator last) {
  clear();
  for (; first != last; ++first)
    DoInsertMulti(end(), *first);
}


// Finds location of node with equal key if present in tree
// or location of leaf where is possible to insert a node with key.
// If key exists, set parent to nullptr and return node with equal key
// If not set parent to parent of null leaf and return null (key not found)
template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_base_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::FindEqual(
    node_base_type_ptr& pParent, const key_type& key) {
  node_type* child = mBegin();
  bool leftside = true;
  pParent = mEnd();

  while (child != nullptr) {
    pParent = child;
    leftside = mImpl.KeyCompare(key, sKey(child));
    child = leftside ? sLeft(child) : sRight(child);
  }

  // found insertion place, just need to check that value not already present
  // if on leftside possible equal value is prev(y)
  // if on rightside possible equal value is y
  // if leftmost and leftside no need to check can only be unique
  node_base_type *y = pParent;
  if (leftside) {
    if (pParent == mLeftMost()) {
      return static_cast<node_base_type*>(child);;
    } else {
      y = RBTreeDecrement(y);
    }
  }

  // y is <= key
  if (mImpl.KeyCompare(sKey(y), key)) {
    // y < key
    return static_cast<node_base_type*>(child);
  } else {
    // y == key
    pParent = nullptr;
    return y;
  }
}

// No need to check for equal, simply walk down the tree
// Leaf will always be null
// Set parent to parent of null leaf
// Return null leaf
template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_base_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::FindMulti(
    node_base_type_ptr& pParent, const  key_type& key) {
  node_type* child = mBegin();
  node_type* y = mEnd();

  while (child != nullptr) {
    y = child;
    if (mImpl.KeyCompare(key, sKey(child)))
      child = sLeft(child);
    else
      child = sRight(child);
  }
  pParent = static_cast<node_base_type_ptr>(y);
  return static_cast<node_base_type*>(child);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::LowerBound(
    node_type* x, node_type* y, const key_type& key) {

  while (x != nullptr) {
    if (!mImpl.KeyCompare(sKey(x), key)) {
      y = x;
      x = sLeft(x);
    }
    else {
      x = sRight(x);
    }
  }
  return iterator(y);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::LowerBound(
    const node_type* x, const node_type* y, const key_type& key) const {

  while (x != nullptr) {
    if (!mImpl.KeyCompare(sKey(x), key)) {
      y = x;
      x = sLeft(x);
    }
    else {
      x = sRight(x);
    }
  }
  return const_iterator(y);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::UpperBound(
    node_type* x, node_type* y, const key_type& key) {

  while (x != nullptr) {
    if (mImpl.KeyCompare(key, sKey(x))) {
      y = x;
      x = sLeft(x);
    }
    else {
      x = sRight(x);
    }
  }
  return iterator(y);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::const_iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::UpperBound(
    const node_type* x, const node_type* y, const key_type& key) const {

  while (x != nullptr) {
    if (mImpl.KeyCompare(key, sKey(x))) {
      y = x;
      x = sLeft(x);
    }
    else {
      x = sRight(x);
    }
  }
  return const_iterator(y);
}


// Find lower bound (parent that is <= key)
// Leaf will always be null
// Set parent to parent of null leaf
// Return reference to null leaf
template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_base_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::FindLeafLow(
    node_base_type_ptr& pParent, const  key_type& key) {
  node_type* child = mBegin();
  node_type* y = mEnd();

  while (child != nullptr) {
    y = child;
    if (mImpl.KeyCompare(sKey(child), key))
      child = sRight(child);
    else
      child = sLeft(child);
  }
  pParent = static_cast<node_base_type_ptr>(y);
  return static_cast<node_base_type*>(child);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_base_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::FindEqual(
    const_iterator hint, node_base_type_ptr& pParent,
    node_base_type_ptr& dummy, const  key_type& key) {
  if (hint == end()) {
    if (size() > 0 && mImpl.KeyCompare(sKey(mRightMost()), key)) {
      pParent = mRightMost();
      return nullptr;
    } else {
      return FindEqual(pParent, key);
    }
  } else if ( mImpl.KeyCompare(key, sKey(hint.mNode))) {
    //  key < *hint
    const_iterator prior = hint;
    if (hint == begin() || mImpl.KeyCompare(sKey((--prior).mNode), key)) {
      // *prev(hint) < key < *hint
      if (hint.mNode->mLeft == nullptr) {
        pParent = static_cast<node_base_type_ptr>(
            const_cast<node_type*>(hint.mNode));
        return pParent->mLeft;
      } else {
        pParent = static_cast<node_base_type_ptr>(
            const_cast<node_type*>(prior.mNode));
        return pParent->mRight;
      }
    }
    // key <= *prev(hint)
    return FindEqual(pParent, key);
  } else if (mImpl.KeyCompare(sKey(hint.mNode), key)) {
    // *hint < key
    const_iterator next = hint;
    ++next;
    if (next == end() || mImpl.KeyCompare(key, sKey(next.mNode))) {
      // *hint < key < *next(hint)
      if (hint.mNode->mRight == nullptr) {
        pParent = static_cast<node_base_type_ptr>(
            const_cast<node_type*>(hint.mNode));
        return pParent->mRight;
      } else {
        pParent =static_cast<node_base_type_ptr>(
            const_cast<node_type*>(next.mNode));
        return pParent->mLeft;
      }
    }
    // *next(hint) <= key
    return FindEqual(pParent, key);
  }
  // else key == *hint
  pParent = static_cast<node_base_type_ptr>(const_cast<node_type*>(hint.mNode));
  dummy = static_cast<node_base_type_ptr>(const_cast<node_type*>(hint.mNode));
  return dummy;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_base_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::FindLeaf(
    const_iterator hint, node_base_type_ptr& pParent,
    const  key_type& key) {

  if (hint == end() || !mImpl.KeyCompare(sKey(hint.mNode), key)) {
    // key <= *hint
    const_iterator prior = hint;
    // prior is either in hint left subtree or above
    if (hint == begin() || !mImpl.KeyCompare(key, sKey((--prior).mNode))) {
      // *prev(hint) <= key <= *hint
      if (hint.mNode->mLeft == nullptr) {
        pParent = static_cast<node_base_type_ptr>(
            const_cast<node_type*>(hint.mNode));
        return pParent->mLeft; // nullptr insert will then put it to the left
                              // if key < hint or right if key == hint;
      } else {
        pParent = static_cast<node_base_type_ptr>(
            const_cast<node_type*>(prior.mNode));
         return pParent->mRight; // nullptr
      }
    }
    // key < *prev(hint)
    return FindMulti(pParent, key);
  }
  // key > *hint
  return FindLeafLow(pParent, key);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::iterator
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoInsert(
    node_base_type* pParent, node_base_type* x, const value_type& value) {
  bool insert_left = (x != 0 || pParent == mEnd() ||
                      mImpl.KeyCompare(KeyOfT()(value), sKey(pParent)));

  node_type* pNew = DoCreateNode(value);

  RBTreeInsert(pNew, pParent, &mImpl.mHeader, insert_left);

  ++mImpl.mSize;
  return iterator(pNew);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoAllocateNode() {
  return static_cast<node_type*>(mImpl.mAllocator.allocate(sizeof(node_type)));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoFreeNode(
    node_type* pNode) {
  mImpl.mAllocator.deallocate(static_cast<void *>(pNode), sizeof(node_type));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoCreateNode(
    const value_type& value) {
  node_type* const pNew = DoAllocateNode();

#if JNSTL_EXCEPTIONS_ENABLED
  try {
#endif
    ::new(static_cast<void*>(&pNew->mValue)) value_type(value);

#if JNSTL_EXCEPTIONS_ENABLED
  } catch (...) {
    DoFreeNode(pNew);
    throw;
  }
#endif

  return pNew;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoCreateNode(
    const node_type* pSource, node_type* pParent) {
  node_type* const pNew = DoCreateNode(pSource->mValue);

  pNew->mRight  = nullptr;
  pNew->mLeft   = nullptr;
  pNew->mParent = pParent;
  pNew->mColor  = pSource->mColor;

  return pNew;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
    rbtree<Key, T, KeyOfT, Compare, Allocator>::DoDestroyNode(
    node_type* pNode) {
  pNode->~node_type();
  mImpl.mAllocator.deallocate(static_cast<void *>(pNode), sizeof(node_type));
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
typename rbtree<Key, T, KeyOfT, Compare, Allocator>::node_type*
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoCreateTree(
    const node_type* pSource, node_type* pParent) {
  node_type* const pTop = DoCreateNode(pSource, pParent);

#if JNSTL_EXCEPTIONS_ENABLED
  try {
#endif
  if(pSource->mRight)
    pTop->mRight = DoCreateTree(sRight(pSource), pTop);

  pParent = pTop;
  pSource = sLeft(pSource);

  while (pSource != nullptr) {
    node_type* y = DoCreateNode(pSource, pParent);

    pParent->mLeft = y;

    if (pSource->mRight)
      y->mRight = DoCreateTree(sRight(pSource), y);

    pParent = y;
    pSource = sLeft(pSource);
  }
#if JNSTL_EXCEPTIONS_ENABLED
  } catch (...) {
    DoDestroyTree(pTop);
    throw;
  }
#endif

  return pTop;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoDestroyTree(
    node_type* pTop) {
  while (pTop != nullptr) {
    DoDestroyTree(sRight(pTop));

    node_type* const pLeft = sLeft(pTop);
    DoDestroyNode(pTop);
    pTop = pLeft;
  }
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
void
rbtree<Key, T, KeyOfT, Compare, Allocator>::DoSwap(
    this_type& x) {
  LIB::swap(mImpl.mSize, x.mImpl.mSize);
  LIB::swap(mImpl.KeyCompare, x.mImpl.KeyCompare);

  if (mImpl.mHeader.mParent != nullptr &&
      x.mImpl.mHeader.mParent != nullptr) {
    LIB::swap(mImpl.mHeader.mRight,  x.mImpl.mHeader.mRight);
    LIB::swap(mImpl.mHeader.mLeft,   x.mImpl.mHeader.mLeft);
    LIB::swap(mImpl.mHeader.mParent, x.mImpl.mHeader.mParent);

      mImpl.mHeader.mParent =   &mImpl.mHeader;
    x.mImpl.mHeader.mParent = &x.mImpl.mHeader;
  } else if (mImpl.mHeader.mParent != nullptr) {
    x.mImpl.mHeader.mRight  = mImpl.mHeader.mRight;
    x.mImpl.mHeader.mLeft   = mImpl.mHeader.mLeft;
    x.mImpl.mHeader.mParent = mImpl.mHeader.mParent;
    x.mImpl.mHeader.mParent = &x.mImpl.mHeader;

    mImpl.mHeader.mRight  = &mImpl.mHeader;
    mImpl.mHeader.mLeft   = &mImpl.mHeader;
    mImpl.mHeader.mParent = nullptr;
  } else if (x.mImpl.mHeader.mParent != nullptr) {
    mImpl.mHeader.mRight  = x.mImpl.mHeader.mRight;
    mImpl.mHeader.mLeft   = x.mImpl.mHeader.mLeft;
    mImpl.mHeader.mParent = x.mImpl.mHeader.mParent;
    mImpl.mHeader.mParent = &mImpl.mHeader;

    x.mImpl.mHeader.mRight  = &x.mImpl.mHeader;
    x.mImpl.mHeader.mLeft   = &x.mImpl.mHeader;
    x.mImpl.mHeader.mParent = nullptr;
  }
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator==(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
           const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return a.size() == b.size() && equal(a.begin(), a.end(), b.begin());
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator<(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
          const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return std::lexicographical_compare(a.begin(), a.end(),
                                      b.begin(), b.end());
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator!=(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
           const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return !(a == b);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator>(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
          const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return b < a;
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator<=(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
          const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return !(b < a);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline bool
operator>=(const rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
          const rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  return !(a < b);
}

template <typename Key, typename T, typename KeyOfT,
          typename Compare, typename Allocator>
inline void
swap(rbtree<Key, T, KeyOfT, Compare, Allocator>& a,
     rbtree<Key, T, KeyOfT, Compare, Allocator>& b) {
  a.swap(b);
}

}  // namespace jnstl
#endif  // JNSTL_RBTREE_H //
