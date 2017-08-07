#ifndef JNSTL_MULTISET_H_
#define JNSTL_MULTISET_H_

#include "JNSTL/bits/config.h"

#include "JNSTL/utility.h"
#include "JNSTL/red_black_tree.h"

namespace jnstl {
template<typename Key, typename Compare = std::less<Key>,
         typename Allocator = jnstl::allocator>
class multiset {
 public:
  typedef Key         key_type;
  typedef Key         value_type;
  typedef Compare     key_compare;
  typedef Compare     value_compare;
  typedef Allocator   allocator_type;

 private:
  typedef rbtree<key_type, value_type, jnstl::select_self<value_type>,
                 key_compare, allocator_type> rep_type;
  rep_type mT;  // Red-black tree representing multiset.

 public:
  // avoids modifications of keys
  typedef typename rep_type::const_iterator                  iterator;
  typedef typename rep_type::const_iterator            const_iterator;
  typedef typename rep_type::size_type                 size_type;
  typedef typename rep_type::difference_type           difference_type;
  typedef pair<iterator, bool>                         insert_return_type;

  multiset()
      : mT() {}

  explicit
  multiset(const Compare& compare,
           const allocator_type& a = allocator_type())
      : mT(compare, a) {}

  template<typename InputIterator>
  multiset(InputIterator first, InputIterator last)
      : mT() {
    mT.DoInsertMulti(first, last);
  }

  template<typename InputIterator>
  multiset(InputIterator first, InputIterator last,
           const Compare& compare,
           const allocator_type& a = allocator_type())
      : mT(compare, a) {
    mT.DoInsertMulti(first, last);
  }

  multiset(const multiset& x)
      : mT(x.mT) {}

  multiset(multiset&& x)
      : mT(LIB::move(x.mT)) {}

  multiset(std::initializer_list<value_type> ilist,
           const Compare& compare = Compare(),
           const allocator_type& a = allocator_type())
      : mT(compare, a) {
    mT.DoInsertMulti(ilist.begin(), ilist.end());
  }

  explicit
  multiset(const allocator_type& a)
      : mT(Compare(), a) {}

  multiset&
  operator=(const multiset& x) {
    mT = x.mT;
    return *this;
  }

  multiset&
  operator=(std::initializer_list<value_type> ilist) {
    mT.DoAssignMulti(ilist.begin(), ilist.end());
    return *this;
  }

  key_compare
  key_comp() const {
    return mT.key_comp();
  }

  value_compare
  value_comp() const {
    return mT.key_comp();
  }

  iterator
  begin() {
    return mT.begin();
  }

  const_iterator
  begin() const {
    return mT.begin();
  }

  iterator
  end() {
    return mT.end();
  }

  const_iterator
  end() const {
    return mT.end();
  }

  bool
  empty() const {
    return mT.empty();
  }

  size_type
  size() const {
    return mT.size();
  }

  void
  swap(multiset& x) {
    mT.swap(x.mT);
  }

  iterator
  insert(const value_type& x) {
    return mT.DoInsertMulti(x);
  }

  iterator
  insert(const value_type&& x) {
    return mT.DoInsertMulti(LIB::move(x));
  }

  iterator
  insert(const_iterator position, const value_type& x) {
    return mT.DoInsertMulti(position, x);
  }

  iterator
  insert(const_iterator position, const value_type&& x) {
    return mT.DoInsertMulti(position, LIB::move(x));
  }

  template<typename InputIterator>
  void
  insert(InputIterator first, InputIterator last) {
    mT.DoInsertMulti(first, last);
  }

  void
  insert(std::initializer_list<value_type> ilist) {
    this->insert(ilist.begin(), ilist.end());
  }

  iterator
  erase(const_iterator position) {
    return mT.erase(position);
  }

  size_type
  erase(const key_type& x) {
    return mT.erase(x);
  }

  iterator
  erase(const_iterator first, const_iterator last) {
    return mT.erase(first, last);
  }

  void
  clear() {
    mT.clear();
  }

  size_type
  count(const key_type& x) const {
    return mT.find(x) == mT.end() ? 0 : 1;
  }

  iterator
  find(const key_type& x) {
    return mT.find(x);
  }

  const_iterator
  find(const key_type& x) const {
    return mT.find(x);
  }

  iterator
  lower_bound(const key_type& x) {
    return mT.lower_bound(x);
  }

  const_iterator
  lower_bound(const key_type& x) const {
    return mT.lower_bound(x);
  }

  iterator
  upper_bound(const key_type& x) {
    return mT.upper_bound(x);
  }

  const_iterator
  upper_bound(const key_type& x) const {
    return mT.upper_bound(x);
  }

  jnstl::pair<iterator, iterator>
  equal_range(const key_type& x) {
    return mT.equal_range(x);
  }

  jnstl::pair<const_iterator, const_iterator>
  equal_range(const key_type& x) const {
    return mT.equal_range(x);
  }

  bool
  validate() const {
    return mT.validate();
  }

  bool
  validate_iterator(const_iterator i) const {
    return mT.validate_iterator(i);
  }

  template<typename _K1, typename _C1, typename _A1>
  friend bool
  operator==(const multiset<_K1, _C1, _A1>&, const multiset<_K1, _C1, _A1>&);

  template<typename _K1, typename _C1, typename _A1>
  friend bool
  operator<(const multiset<_K1, _C1, _A1>&, const multiset<_K1, _C1, _A1>&);
};

template<typename Key, typename Compare, typename Allocator>
inline bool
operator==(const multiset<Key, Compare, Allocator>& x,
           const multiset<Key, Compare, Allocator>& y) {
  return x.mT == y.mT;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator<(const multiset<Key, Compare, Allocator>& x,
          const multiset<Key, Compare, Allocator>& y) {
  return x.mT < y.mT;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator!=(const multiset<Key, Compare, Allocator>& x,
           const multiset<Key, Compare, Allocator>& y) {
  return !(x == y);
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator>(const multiset<Key, Compare, Allocator>& x,
          const multiset<Key, Compare, Allocator>& y) {
  return y < x;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator<=(const multiset<Key, Compare, Allocator>& x,
           const multiset<Key, Compare, Allocator>& y) {
  return !(y < x);
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator>=(const multiset<Key, Compare, Allocator>& x,
           const multiset<Key, Compare, Allocator>& y) {
  return !(x < y);
}

template<typename Key, typename Compare, typename Allocator>
inline void
swap(multiset<Key, Compare, Allocator>& x,
     multiset<Key, Compare, Allocator>& y) {
  x.swap(y);
}

}  // namespace jnstl
#endif  // JNSTL_MULTISET_H_ //
