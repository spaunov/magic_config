#pragma once

#include <type_traits>

namespace magic_config::traits {

// Class is_one_of checks if T is in a list of types (Us)
template <typename T, typename... Us>
struct is_one_of : std::false_type { };

// Specialization to check first type and recurse
template <typename T, typename U, typename... Us>
struct is_one_of<T, U, Us...>  : std::integral_constant<bool,
    std::is_same<T, U>::value || is_one_of<T, Us...>::value> {
};

template <typename T, typename... Us>
inline constexpr bool is_one_of_v = is_one_of<T, Us...>::value;

}  //namespace magic_config::traits
