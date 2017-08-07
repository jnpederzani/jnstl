#ifndef JNSTL_MAP_H_
#define JNSTL_MAP_H_

#include "JNSTL/bits/config.h"

#include "JNSTL/utility.h"
#include "JNSTL/red_black_tree.h"

namespace jnstl {
template<typename Key, typename T, typename Compare = std::less<Key>,
         typename Allocator = jnstl::allocator>
class map {
 public:
  typedef Key                               key_type;
  typedef T                                 mapped_type;
  typedef jnstl::pair<const Key, T>         value_type;
  typedef Compare                           key_compare;
  typedef Allocator                         allocator_type;

 public:
  class value_compare : public std::binary_function<value_type, value_type, bool> {
    friend class map<Key, T, Compare, Allocator>;
   protected:
    Compare comp;

    value_compare(Compare c)
        : comp(c) {}

   public:
    bool operator()(const value_type& x, const value_type& y) const {
      return comp(x.first, y.first);
    }
  };

 private:
  typedef rbtree<key_type, value_type, jnstl::select_first<value_type>,
                 key_compare, allocator_type> rep_type;
  rep_type mT;  // Red-black tree representing map.

 public:
  typedef typename rep_type::iterator                  iterator;
  typedef typename rep_type::const_iterator            const_iterator;
  typedef typename rep_type::size_type                 size_type;
  typedef typename rep_type::difference_type           difference_type;
  typedef pair<iterator, bool>                         insert_return_type;

  map()
      : mT() {}

  explicit
  map(const Compare& compare,
      const allocator_type& a = allocator_type())
      : mT(compare, a) {}

  template<typename InputIterator>
  map(InputIterator first, InputIterator last)
      : mT() {
    mT.DoInsertUnique(first, last);
  }

  template<typename InputIterator>
  map(InputIterator first, InputIterator last,
      const Compare& compare,
      const allocator_type& a = allocator_type())
      : mT(compare, a) {
    mT.DoInsertUnique(first, last);
  }

  map(const map& x)
      : mT(x.mT) {}

  map(map&& x)
      : mT(LIB::move(x.mT)) {}

  map(std::initializer_list<value_type> ilist,
      const Compare& compare = Compare(),
      const allocator_type& a = allocator_type())
      : mT(compare, a) {
    mT.DoInsertUnique(ilist.begin(), ilist.end());
  }

  explicit
  map(const allocator_type& a)
      : mT(Compare(), a) {}

  map&
  operator=(const map& x) {
    mT = x.mT;
    return *this;
  }

  map&
  operator=(std::initializer_list<value_type> ilist) {
    mT.DoAssignUnique(ilist.begin(), ilist.end());
    return *this;
  }

  key_compare
  key_comp() const {
    return mT.key_comp();
  }

  value_compare
  value_comp() const {
    return value_compare(mT.key_comp());
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
  swap(map& x) {
    mT.swap(x.mT);
  }

  mapped_type&
  operator[](const key_type& key) {
    iterator i = lower_bound(key);

    if (i == end() || key_comp()(key, (*i).first))
      i = insert(i, value_type(key, mapped_type()));

    return (*i).second;
  }

  mapped_type&
  operator[](key_type& key) {
    iterator i = lower_bound(key);

    if (i == end() || key_comp()(key, (*i).first))
      i = insert(i, value_type(LIB::move(key), mapped_type()));

    return (*i).second;
  }

  mapped_type&
  at(const key_type& key) {
    iterator i = lower_bound(key);
#if JNSTL_EXCEPTIONS_ENABLED
    if (i == end() || key_comp()(key, (*i).first))
      throw;
#endif
    return (*i).second;
  }

  const mapped_type&
  at(const key_type& key) const {
    const_iterator i = lower_bound(key);
#if JNSTL_EXCEPTIONS_ENABLED
    if (i == end() || key_comp()(key, (*i).first))
      throw;
#endif
    return (*i).second;
  }

  jnstl::pair<iterator, bool>
  insert(const value_type& x) {
    jnstl::pair<typename rep_type::iterator, bool> p =
        mT.DoInsertUnique(x);
    return jnstl::pair<iterator, bool>(p.first, p.second);
  }

  jnstl::pair<iterator, bool>
  insert(const value_type&& x) {
    jnstl::pair<typename rep_type::iterator, bool> p =
        mT.DoInsertUnique(LIB::move(x));
    return jnstl::pair<iterator, bool>(p.first, p.second);
  }

  iterator
  insert(const_iterator position, const value_type& x) {
    return mT.DoInsertUnique(position, x);
  }

  iterator
  insert(const_iterator position, const value_type&& x) {
    return mT.DoInsertUnique(position, LIB::move(x));
  }

  template<typename InputIterator>
  void
  insert(InputIterator first, InputIterator last) {
    mT.DoInsertUnique(first, last);
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
  operator==(const map<_K1, _C1, _A1>&, const map<_K1, _C1, _A1>&);

  template<typename _K1, typename _C1, typename _A1>
  friend bool
  operator<(const map<_K1, _C1, _A1>&, const map<_K1, _C1, _A1>&);
};

template<typename Key, typename Compare, typename Allocator>
inline bool
operator==(const map<Key, Compare, Allocator>& x,
           const map<Key, Compare, Allocator>& y) {
  return x.mT == y.mT;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator<(const map<Key, Compare, Allocator>& x,
          const map<Key, Compare, Allocator>& y) {
  return x.mT < y.mT;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator!=(const map<Key, Compare, Allocator>& x,
          const map<Key, Compare, Allocator>& y) {
  return !(x == y);
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator>(const map<Key, Compare, Allocator>& x,
          const map<Key, Compare, Allocator>& y) {
  return y < x;
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator<=(const map<Key, Compare, Allocator>& x,
           const map<Key, Compare, Allocator>& y) {
  return !(y < x);
}

template<typename Key, typename Compare, typename Allocator>
inline bool
operator>=(const map<Key, Compare, Allocator>& x,
           const map<Key, Compare, Allocator>& y) {
  return !(x < y);
}

template<typename Key, typename Compare, typename Allocator>
inline void
swap(map<Key, Compare, Allocator>& x, map<Key, Compare, Allocator>& y) {
  x.swap(y);
}

}  // namespace jnstl
#endif  // JNSTL_MAP_H_ //
