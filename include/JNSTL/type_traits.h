#ifndef JNSTL_TYPE_TRAITS_H_
#define JNSTL_TYPE_TRAITS_H_

#include "JNSTL/bits/config.h"

namespace jnstl {

template <typename T>
struct is_normal_iterator
    : public std::false_type {};

template <typename Iterator, typename Container>
class normal_iterator;

template <typename Iterator, typename Container>
struct is_normal_iterator<normal_iterator<Iterator, Container>>
    : public std::true_type {};


template <typename T>
struct is_move_iterator
    : public std::false_type {};

template <typename Iterator>
class move_iterator;

template <typename Iterator>
struct is_move_iterator<move_iterator<Iterator>>
    : public std::true_type {};

}  // namespace jnstl

#endif /* JNSTL_TYPE_TRAITS_H_ */
