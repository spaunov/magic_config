#pragma once

#include "traits/define_trait.hpp"
#include "traits/supported_types.hpp"
#include "traits/optional.hpp"
#include "traits/features.hpp"
#include "traits/container_traits.hpp"

namespace magic_config { namespace magic { namespace detail {

struct AggregateConfigTag {
    // Tag used to distinguish complex (aggregate) from simple types
    // NOTE: config::magic::Loader inherits this tag which enables the framework to
    //       correctly determine which conversion/loader logic to use on a given type.
};

// Trait to check if a given T is an instance of a magic config
template <typename T>
inline constexpr bool is_magic_config = std::is_base_of<AggregateConfigTag, T>::value;

}}}  // namespace magic_config::magic::detail
