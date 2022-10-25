#pragma once

#include "Converters.h"

#include "../magic/detail/error.hpp"
#include "../magic/detail/traits.hpp"

#include <boost/lexical_cast.hpp>

namespace magic_config { namespace yaml {

// Adapter verify method:
template <typename... Args>
inline void VERIFY_YAML(Args&&... args)
{
    struct Traits {
        using Config = YAML::Node;
    };

    VERIFY_CONFIG<Traits>(std::forward<Args>(args)...);
}

namespace detail {

// ==========================================================================
// HELPER METHODS:

template <typename Target,
          std::enable_if_t<std::is_same_v<Target, std::string> >* = nullptr>
Target extractFromString(const YAML::Node& node)
{
    return node.as<std::string>();
}

template <typename Target,
          std::enable_if_t<!std::is_same_v<Target, std::string> >* = nullptr>
Target extractFromString(const YAML::Node& node)
{
    auto asString = node.as<std::string>();
    return boost::lexical_cast<Target>(asString.c_str());
}

template <typename Target>
Target extractPair(const YAML::Node& node)
{
    using Key   = std::remove_cv_t<typename Target::first_type>;
    using Value = std::remove_cv_t<typename Target::second_type>;

    VERIFY_YAML(node.IsSequence(), "Type mismatch: pair is represented as a sequence!", &node);

    VERIFY_YAML(node.size() != 2, "Type mismatch: only two elements needed", &node);

    // TODO: Does YAML, like JSON, require that key is string?!
    return Target(extractFromString<Key>(node[0]),
                  Converter<Value>::convert(node[1]));
}

template <typename Target>
Target extractArray(const YAML::Node& node)
{
    using value_type = typename Target::value_type;

    VERIFY_YAML(node.IsSequence(), "Type mismatch: array or list expected!", &node);

    Target target;

    VERIFY_YAML(node.size() == target.max_size(), "Array size mismatch!", &node);

    size_t idx = 0; // start index

    for (auto const& item : node) {
        target[idx++] = Converter<value_type>::convert(item);
    }

    return target;
}

// Inserter functions:
// NOTE: Taking value_type as a forwarding ref to make sure code is optimal

template <typename Container, typename value_type>
std::enable_if_t<magic_config::traits::has_push_back_method_v<Container> >
insert_helper(Container& c, value_type&& value) {
    // static_assert(std::is_same_v<value_type, typename Containter::value_type>, "");
    c.push_back(std::forward<value_type>(value));
}

template <typename Container, typename value_type>
std::enable_if_t<magic_config::traits::has_insert_method<Container>::value>
insert_helper(Container& c, value_type&& value) {
    // static_assert(std::is_same_v<value_type, typename Containter::value_type>, "");
    c.insert(std::forward<value_type>(value));
}


template <typename Target>
Target extractSequence(const YAML::Node& node)
{
    using value_type = typename Target::value_type;

    VERIFY_YAML(node.IsSequence(), "Type mismatch: array or list expected!", &node);

    Target result;

    for (auto const& item : node) {
        insert_helper(result, Converter<value_type>::convert(item));
    }

    return result;
}

template <typename Target>
Target extractMapContainter(const YAML::Node& node)
{
    using value_type = typename Target::value_type;
    using Key        = std::remove_cv_t<typename value_type::first_type>;
    using Value      = std::remove_cv_t<typename value_type::second_type>;

    Target target;

    VERIFY_YAML(node.IsMap(), "Type mismatch: object expected!", &node);

    for (auto const& pair : node) {
        auto& key   = pair.first;
        auto& value = pair.second;

        // TODO: Does YAML, like JSON, require that key is string?!
        VERIFY_YAML(key.IsScalar(),
                    "Type mismatch: expected a string element!", &key);

        auto item = std::make_pair(extractFromString<Key>(key),
                                   Converter<Value>::convert(value));

        auto result = target.insert(std::move(item));

        VERIFY_YAML(result.second, "Unable to insert element");
    }

    return target;
}

}  // namespace detail

}}  // namespace magic_config::yaml
