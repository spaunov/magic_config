#pragma once

#include <type_traits>

namespace magic_config::traits {

// NOTE: Lifted these from somewhere inside Folly

template <typename...>
struct Conjunction : std::true_type {};
template <typename T>
struct Conjunction<T> : T {};
template <typename T, typename... TList>
struct Conjunction<T, TList...>
    : std::conditional<T::value, Conjunction<TList...>, T>::type {};

template <typename...>
struct Disjunction : std::false_type {};
template <typename T>
struct Disjunction<T> : T {};
template <typename T, typename... TList>
struct Disjunction<T, TList...>
    : std::conditional<T::value, T, Disjunction<TList...>>::type {};

template <typename T>
struct Negation : std::integral_constant<bool, !T::value> {};

template <bool... Bs>
struct Bools {
  using valid_type = bool;
  static constexpr std::size_t size() {
    return sizeof...(Bs);
  }
};

// Lighter-weight than Conjunction, but evaluates all sub-conditions eagerly.
template <class... Ts>
struct StrictConjunction
    : std::is_same<Bools<Ts::value...>, Bools<(Ts::value || true)...>> {};

template <class... Ts>
struct StrictDisjunction
  : Negation<
      std::is_same<Bools<Ts::value...>, Bools<(Ts::value && false)...>>
    > {};


template<bool... Bs>
using AllTrue = std::is_same<Bools<Bs..., true>, Bools<true, Bs...> >;

template<bool... Bs>
inline constexpr bool all_true_v = AllTrue<Bs...>::value;

}  //namespace magic_config::traits
