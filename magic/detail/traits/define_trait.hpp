#pragma once

#include <type_traits>
#include <utility>

#define DEFINE_HAS_TRAIT(X)                                 \
template <typename, typename = std::void_t<> >              \
struct has_trait_ ## X : std::false_type {};                \
                                                            \
template <typename T>                                       \
struct has_trait_ ## X<T, std::void_t<typename T::X> >      \
   : std::true_type {}


#define DEFINE_HAS_MEMBER(X)                                \
template <typename, typename = std::void_t<> >              \
struct has_member_ ## X : std::false_type {};               \
                                                            \
template <typename T>                                       \
struct has_member_ ## X<T, std::void_t<decltype(&T::X) > >  \
   : std::true_type {}
