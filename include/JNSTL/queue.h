#ifndef JNSTL_QUEUE_H_
#define JNSTL_QUEUE_H_

#include "JNSTL/bits/config.h"

namespace jnstl {
/**
 * @brief A container adapter givig FIFO behavior.
 *
 * @tparam T Type of the stored elements.
 * @tparam Container The type of the underlyng container used to store the
 * elements.
 *
 * The queue class is a container adapter that gives the programmer the
 * functionality of a queue - specifically, a FIFO (first-in, first-out) data
 * structure.
 * The class template acts as a wrapper to the underlying container - only a
 * specific set of functions is provided. The queue pushes the element on the
 * back of the underlying container and pops them from the front.
 * The type of the underlying container to use to store the elements must
 * satisfy the requirements of SequenceContainer. Additionally, it must provide
 * the following functions with the usual semantics:
 * back()
 * front()
 * push_back()
 * pop_front()
 * The containers deque and list satisfy these requirements.
 */
template <typename T, typename Container = jnstl::list<T>>
class queue {
  template <typename T1, typename C1>
  friend bool
  operator==(const queue<T1, C1>&, const queue<T1, C1>&);

  template <typename T1, typename C1>
  friend bool
  operator<(const queue<T1, C1>&, const queue<T1, C1>&);

 public:
  typedef queue<T, Container>                    this_type;
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
  queue(const container_type& cont)
      : c{cont} {}

  explicit
  queue(container_type&& cont = container_type())
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
   * Returns a read/write reference to the first element of the queue.
   */
  reference
  front() {
    return c.front();
  }

  /**
   * Returns a read-only (constant)  reference to the first element of the
   * queue.
   */
  const_reference
  front() const {
    return c.front();
  }

  /**
   * Returns a read/write reference to the last element of the queue.
   */
  reference
  back() {
    return c.back();
  }

  /**
   * Returns a read-only (constant)  reference to the last element of the
   * queue.
   */
  const_reference
  back() const {
    return c.back();
  }

  /**
   *   Pushes the given element to the back of the queue.
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
   *  @brief Removes the first element from the queue.
   *
   *  Note that no data is returned, and if the first element's
   *  data is needed, it should be retrieved before pop() is
   *  called.
   */
  void
  pop() {
    c.pop_front();
  }

  /**
   * Exchanges the contents of the container adaptor with those of other.
   */
  void
  swap(queue& other) {
    using std::swap;
    swap(c, other.c);
  }

  bool
  validate() const {
    return c.validate();
  }
}; // queue

template <typename T, typename Container>
inline bool
operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return (lhs.c == rhs.c);
}

template <typename T, typename Container>
inline bool
operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return (lhs.c < rhs.c);
}

template <typename T, typename Container>
inline bool
operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
inline bool
operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return (rhs < lhs);
}

template <typename T, typename Container>
inline bool
operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Container>
inline bool
operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Container>
inline void
swap(queue<T, Container>& lhs, queue<T, Container>& rhs) {
  lhs.swap(rhs);
}

/**
 * @brief A container adapter that provides constant time lookup of the largest
 * (by default) element, at the expense of logarithmic insertion and extraction.
 *
 * @tparam T Type of the stored elements.
 * @tparam Container The type of the underlyng container used to store the
 * elements.
 * @tparam Compare A compare function providing a strict weak ordering.
 *
 * The type of the underlying container to use to store the elements must
 * satisfy the requirements of SequenceContainer and its iterators must satisfy
 * the requirments of RandomAccessIterator. Additionally, it must provide the
 * following functions with the usual semantics:
 * front()
 * push_back()
 * pop_back()
 * The containers vector and deque satisfy these requirements.
 */
template <typename T, typename Container = jnstl::vector<T>,
          typename Compare = std::less<typename Container::value_type>>
class priority_queue {
 public:
  typedef priority_queue<T, Container>           this_type;
  typedef Container                              container_type;
  typedef Compare                                compare_type;
  typedef typename Container::value_type         value_type;
  typedef typename Container::reference          reference;
  typedef typename Container::const_reference    const_reference;
  typedef typename Container::size_type          size_type;

 protected:
  /* Standerd requirement */
  Container c;
  Compare   comp;

 public:
  explicit
  priority_queue(const compare_type& compare, const container_type& cont)
      : c{cont}, comp{compare} {
    jnstl::make_heap(c.begin(), c.end(), comp);
  }

  explicit
  priority_queue(const compare_type& compare = Compare(),
                 container_type&& cont = container_type())
      : c{LIB::move(cont)}, comp{compare} {
    jnstl::make_heap(c.begin(), c.end(), comp);
  }

  template<typename InputIt>
  priority_queue(InputIt first, InputIt last, const Compare& compare,
                 const Container& cont)
      : c{cont}, comp{compare} {
    c.insert(c.end(),first, last);
    jnstl::make_heap(c.begin(), c.end(), comp);
  }

  template<typename InputIt>
  priority_queue(InputIt first, InputIt last,
                 const Compare& compare = Compare(),
                 Container&& cont = Container())
      : c{LIB::move(cont)}, comp{compare} {
    c.insert(c.end(),first, last);
    jnstl::make_heap(c.begin(), c.end(), comp);
  }

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
   * Returns a read/write reference to the first element of the priority_queue.
   * If default comparison function is used, the returned element is the
   * greatest in the queue.
   */
  const_reference
  top() const {
    return c.front();
  }

  /**
   *   Pushes the given element to the the priority_queue.
   */
  void
  push(const value_type& value) {
    c.push_back(value);
    jnstl::push_heap(c.begin(), c.end(), comp);
  }

  void
  push(value_type&& value) {
    c.push_back(LIB::move(value));
    jnstl::push_heap(c.begin(), c.end(), comp);
  }

  /**
   *  @brief Removes the top element from the priority_queue.
   *
   *  Note that no data is returned, and if the first element's
   *  data is needed, it should be retrieved before pop() is
   *  called.
   */
  void
  pop() {
    std::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
  }

  /**
   * Exchanges the contents of the container adaptor with those of other.
   */
  void
  swap(priority_queue& other) {
    using std::swap;
    swap(c, other.c);
    swap(comp, other.comp);
  }

  container_type&
  get_container()  {
    return c;
  }

  bool
  validate() const {
    return c.validate() && jnstl::is_heap(c.begin(), c.end(), comp);
  }

}; // priority_queue

template <typename T, typename Container, typename Compare>
inline void
swap(priority_queue<T, Container, Compare>& lhs,
     priority_queue<T, Container, Compare>& rhs) {
  lhs.swap(rhs);
}

}  // namespace jnstl

#endif /* JNSTL_QUEUE_H_ */
