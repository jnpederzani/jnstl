#ifndef JNSTL_STACK_H_
#define JNSTL_STACK_H_

#include "JNSTL/bits/config.h"

namespace jnstl {
/**
 * @brief A container adapter givig FILO behavior.
 *
 * @tparam T Type of the stored elements.
 * @tparam Container The type of the underlyng container used to store the
 * elements.
 *
 * The stack class is a container adapter that gives the programmer the
 * functionality of a stack - specifically, a FILO (first-in, last-out) data
 * structure.
 * The class template acts as a wrapper to the underlying container - only a
 * specific set of functions is provided. The stack pushes and pops the element
 * from the back of the underlying container, known as the top of the stack.
 * The type of the underlying container to use to store the elements must
 * satisfy the requirements of SequenceContainer. Additionally,
 * it must provide the following functions with the usual semantics:
 * back()
 * push_back()
 * pop_back()
 * The containers vector, deque and list satisfy these requirements.
 */
template <typename T, typename Container = jnstl::vector<T>>
class stack {
  template <typename T1, typename C1>
  friend bool
  operator==(const stack<T1, C1>&, const stack<T1, C1>&);

  template <typename T1, typename C1>
  friend bool
  operator<(const stack<T1, C1>&, const stack<T1, C1>&);

 public:
  typedef stack<T, Container>                    this_type;
  typedef          Container                     container_type;
  typedef typename Container::value_type         value_type;
  typedef typename Container::reference          reference;
  typedef typename Container::const_reference    const_reference;
  typedef typename Container::size_type          size_type;

 protected:
  /* Standerd requirement */
  Container c;

 public:
  explicit
  stack(const container_type& cont)
      : c{cont} {}

  explicit
  stack(container_type&& cont = container_type())
      : c{LIB::move(cont)} {}

  /**
   *  Checks if the underlying container has no elements.
   */
  bool
  empty() const {
    return c.empty();
  }

  /**
   * Returns the number of elements in the container.
   */
  size_type
  size() const {
    return c.size();
  }

  /**
   *  Returns a read/write reference to the top element of the stack.
   *  This element is removed with a call to pop().
   */
  reference
  top() {
    return c.back();
  }

  /**
   *  Returns a read-only (constant)  reference to the top element of the stack.
   *  This element is removed with a call to pop().
   */
  const_reference
  top() const {
    return c.back();
  }

  /**
   *   Pushes the given element to the top of the stack.
   */
  void
  push(const value_type& value) {
    c.push_back(value);
  }

  void
  push(value_type&& value) {
    c.push_back(LIB::move(value));
  }

  /**
   *  @brief Removes the top element from the stack.
   *
   *  Note that no data is returned, and if the top element's
   *  data is needed, it should be retrieved before pop() is
   *  called.
   */
  void
  pop() {
    c.pop_back();
  }

  /**
   * Exchanges the contents of the container adaptor with those of other.
   */
  void
  swap(stack& other) {
    using std::swap;
    swap(c, other.c);
  }

  bool
  validate() const {
    return c.validate();
  }
};  // stack

template <typename T, typename Container>
inline bool
operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return (lhs.c == rhs.c);
}

template <typename T, typename Container>
inline bool
operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return (lhs.c < rhs.c);
}

template <typename T, typename Container>
inline bool
operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
inline bool
operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return (rhs < lhs);
}

template <typename T, typename Container>
inline bool
operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Container>
inline bool
operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Container>
inline void
swap(stack<T, Container>& lhs, stack<T, Container>& rhs) {
  lhs.swap(rhs);
}

}  // namespace jnstl

#endif /* JNSTL_STACK_H_ */
