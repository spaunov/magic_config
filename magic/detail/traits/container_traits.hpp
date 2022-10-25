#pragma once

#include "define_trait.hpp"
#include "logical.hpp"

#include <iterator>
#include <type_traits>

namespace magic_config::traits {

DEFINE_HAS_TRAIT(value_type);
DEFINE_HAS_TRAIT(iterator);
DEFINE_HAS_TRAIT(key_type);
DEFINE_HAS_TRAIT(mapped_type);

// NOTE: Lifted this from Facebook/folly
template <typename T> struct iterator_class_is_container {
  typedef std::reverse_iterator<typename T::iterator> some_iterator;
  enum {
     value = has_trait_value_type<T>::value
          && std::is_constructible<T, some_iterator, some_iterator>::value
  };
};

template <typename T>
using class_is_container =
    Conjunction<has_trait_iterator<T>, iterator_class_is_container<T>>;

template <typename T>
using is_range = StrictConjunction<has_trait_value_type<T>, has_trait_iterator<T>>;

template <typename T>
using is_container = StrictConjunction<std::is_class<T>, class_is_container<T>>;

template <typename T>
using is_map = StrictConjunction<is_range<T>, has_trait_mapped_type<T>>;

template <typename T>
using is_associative = StrictConjunction<is_range<T>, has_trait_key_type<T>>;

}  //namespace magic_config::traits
