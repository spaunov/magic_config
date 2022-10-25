#pragma once

#include <type_traits>

namespace magic_config::traits {

// -----------------------------------------------------------------
// Extract ::value_type from provided type T
template <typename T>
using get_vt_t = typename T::value_type;

// -----------------------------------------------------------------
// Interface checker: Check if T has a push_back() method
template <typename, typename = std::void_t<> >
struct has_push_back_method : std::false_type {};

template <typename T>
struct has_push_back_method<T,
           std::void_t<decltype(std::declval<T>().push_back(get_vt_t<T>{}))> >
   : std::true_type {};


template <typename T>
inline constexpr bool has_push_back_method_v = has_push_back_method<T>::value;

// -----------------------------------------------------------------
// Interface checker: Check if T has a insert() method
template <typename, typename = std::void_t<> >
struct has_insert_method : std::false_type {};

template <typename T>
struct has_insert_method<T,
           std::void_t<decltype(std::declval<T>().insert(get_vt_t<T>{}))> >
   : std::true_type {};

template <typename T>
using has_insert_method_v = typename has_insert_method<T>::value;


// Interface checker: Check if T has a push_back() method
template <typename, typename, typename = std::void_t<> >
struct has_verify_config : std::false_type {};

template <typename T, typename Config>
struct has_verify_config<T, Config, std::void_t<decltype(
           std::declval<T>().verify_config(std::declval<Config>()))> >
   : std::true_type {};


// Interface checker: Check if T has a static defineConfigMapping() method
template <typename, typename = std::void_t<> >
struct has_defineConfigMapping : std::false_type {};

template <typename T>
struct has_defineConfigMapping<T, std::void_t<decltype(
           std::declval<T>().defineConfigMapping())> >
   : std::true_type {};

}  //namespace magic_config::traits
