#pragma once

#include "one_of.hpp"

#include <sys/types.h>
#include <stdint.h>

namespace magic_config::traits {

// -----------------------------------------------------------------
// Defining is_supported_char to explicitly list supported char types
// This is used to elimitate implicit conversions to char
template <typename T>
struct is_supported_char
   : std::integral_constant<bool,
                            is_one_of_v<std::decay_t<T>,
                                        char, unsigned char, int8_t, uint8_t
                                      > > {
};

template <typename T>
inline constexpr bool is_supported_char_v = is_supported_char<T>::value;


// -----------------------------------------------------------------
// Defining is_supported_integer to explicitly list supported integer types
// NOTE: The alternative is to use std::is_integral, but then we will need
//       to provide overloads to all incompatible types e.g. char variants
template <typename T>
struct is_supported_integer
   : std::integral_constant<bool,
                            is_one_of_v<std::decay_t<T>,
                                        int16_t,  int32_t,  int64_t,
                                        uint16_t, uint32_t, uint64_t,
                                        ssize_t, size_t
                                       > > {
};

template <typename T>
inline constexpr bool is_supported_integer_v = is_supported_integer<T>::value;

}  //namespace magic_config::traits
