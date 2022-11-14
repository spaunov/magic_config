#pragma once

#include <optional>
#include <stdexcept>

#include <type_traits>

namespace magic_config {
namespace traits {

// Class is_optional checks if type T is a std::optional
// Default template
template <typename T>
struct is_optional : std::false_type {};

// Specialization for optional
template <typename T>
struct is_optional<std::optional<T> >  : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;


}  // namespace traits

template <typename T>
bool has_value(const T&) { return true; }

template <typename T>
bool has_value(const std::optional<T>& opt) { return opt.has_value(); }

template <typename T>
decltype(auto) get(const T& value) { return value; }

template <typename T>
decltype(auto) get(const std::optional<T>& opt)
{
    if (!opt) {
        throw std::logic_error("Attempt to dereference uninitialized optional");
    }

    return *opt;
}

}  // namespace magic_config
