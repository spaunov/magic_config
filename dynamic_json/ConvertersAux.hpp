#pragma once

#include "Converters.h"

#include "../magic/detail/error.hpp"
#include "../magic/detail/traits.hpp"

#include <boost/lexical_cast.hpp>

namespace magic_config { namespace dynamic_json {

// Adapter verify method:
template <typename... Args>
inline void VERIFY_JSON(Args&&... args)
{
    struct Traits {
        using Config = folly::dynamic;
    };

    VERIFY_CONFIG<Traits>(std::forward<Args>(args)...);
}

namespace detail {

// ==========================================================================
// HELPER METHODS:

template <typename Target,
          std::enable_if_t<std::is_same_v<Target, std::string> >* = nullptr>
Target extractFromString(const folly::dynamic& json)
{
    return std::string(json.c_str());
}

template <typename Target,
          std::enable_if_t<!std::is_same_v<Target, std::string> >* = nullptr>
Target extractFromString(const folly::dynamic& json)
{
    return boost::lexical_cast<Target>(json.c_str());
}

template <typename Target>
Target extractPair(const folly::dynamic& json)
{
    using Key   = std::remove_cv_t<typename Target::first_type>;
    using Value = std::remove_cv_t<typename Target::second_type>;

    VERIFY_JSON(json.isObject(), "Type mismatch: pair is represented as object!", &json);

    VERIFY_JSON(json.size() != 2, "Type mismatch: only two elements needed", &json);

    auto const& pair = *json.items().begin();

    return Target(// Converter<Key>::convert(pair.first),
        extractFromString<Key>(pair.first),
        Converter<Value>::convert(pair.second));
}


template <typename Target>
Target extractArray(const folly::dynamic& json)
{
    using value_type = typename Target::value_type;

    VERIFY_JSON(json.isArray(), "Type mismatch: array or list expected!", &json);

    Target target;

    VERIFY_JSON(json.size() == target.max_size(), "Array size mismatch!", &json);

    size_t idx = 0; // start index

    for (auto const& item : json) {
        target[idx++] = Converter<value_type>::convert(item);
    }

    return target;
}


// Inserter functions:
// NOTE: Taking value_type as a forwarding ref to make sure code is optimal

template <typename Container, typename value_type>
std::enable_if_t<magic_config::traits::has_push_back_method<Container>::value>
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
Target extractSequence(const folly::dynamic& json)
{
    using value_type = typename Target::value_type;

    VERIFY_JSON(json.isArray(), "Type mismatch: array or list expected!", &json);

    Target result;

    for (auto const& item : json) {
        insert_helper(result, Converter<value_type>::convert(item));
    }

    return result;
}

template <typename Target>
Target extractMapContainter(const folly::dynamic& json)
{
    using value_type = typename Target::value_type;
    using Key        = std::remove_cv_t<typename value_type::first_type>;
    using Value      = std::remove_cv_t<typename value_type::second_type>;

    Target target;

    VERIFY_JSON(json.isObject(), "Type mismatch: object expected!", &json);

    for (auto const& pair : json.items()) {
        auto& key = pair.first;
        VERIFY_JSON(key.isString(),
                    "Type mismatch: expected a string element!", &key);

        auto item = std::make_pair(// Converter<Key>::convert(pair.first),
            extractFromString<Key>(pair.first),
            Converter<Value>::convert(pair.second));

        auto result = target.insert(std::move(item));

        // auto result = target.insert(std::move(extractPair<value_type>(pair)));

        VERIFY_JSON(result.second, "Unable to insert element");
    }

    return target;
}

}  // namespace detail

}}  // namespace magic_config::json
