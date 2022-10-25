#pragma once

#include "ConvertersAux.hpp"

#include <array>
#include <limits>
#include <utility>
#include <type_traits>

namespace magic_config { namespace dynamic_json {

// -----------------------------------------------------------------------
// CONVERTER SPECIALIZATIONS:

template <>
struct Converter<bool>
{
    static bool convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isBool(), "Incompatible boolean type!", &json);
        return json.asBool();
    }
};

template <>
struct Converter<std::string>
{
    static std::string convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isString(), "Incompatible string type!", &json);
        return std::string(json.c_str());
    }
};


template <typename Target>
struct Converter<Target, std::enable_if_t<magic_config::traits::is_supported_char_v<Target>> >
{
    static Target convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isString(), "Incompatible string type!", &json);
        VERIFY_JSON(json.size() == 1, "Incompatible string len!", &json);

        return Target(json.c_str()[0]);
    }
};


template <typename Target>
struct Converter<Target, std::enable_if_t<std::is_enum_v<Target> > >
{
    using target_type = std::underlying_type_t<Target>;

    static target_type convert(const folly::dynamic& json) {
        return static_cast<target_type>(Converter<target_type>::convert(json));
    }
};

template <typename Target>
struct Converter<Target, std::enable_if_t<magic_config::traits::is_supported_integer_v<Target> > >
{
    static Target convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isInt(), "Incompatible integral type!", &json);

        auto value = json.asInt();

        VERIFY_JSON(value <= std::numeric_limits<Target>::max(),
                    "Not enough precision to store value", &json);

        return static_cast<Target>(value);
    }
};

template <typename Target>
struct Converter<Target, std::enable_if_t<std::is_floating_point_v<Target> > >
{
    static Target convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isDouble(),
                    "Incompatible floating point type!", &json);

        auto value = json.asDouble();

        VERIFY_JSON(value <= std::numeric_limits<Target>::max(),
                    "Not enough precision to store value", &json);

        return static_cast<Target>(value);
    }
};

template <typename Key, typename Value>
struct Converter< std::pair<Key, Value> >
{
    using target_type = std::pair<Key, Value>;

    static target_type convert(const folly::dynamic& json)
    {
        return detail::extractPair<target_type>(json);
    }
};

template <typename Target, size_t SIZE>
struct Converter< std::array<Target, SIZE> >
{
    using target_type = std::array<Target, SIZE>;

    static target_type convert(const folly::dynamic& json)
    {
        return detail::extractArray<target_type>(json);
    }
};

template <typename Container>
struct Converter<Container,
                 std::enable_if_t<magic_config::traits::is_container<Container>::value and
                                  not magic_config::traits::is_map<Container>::value> >
{
    static Container convert(const folly::dynamic& json)
    {
        return detail::extractSequence<Container>(json);
    }
};

template <typename Container>
struct Converter<Container,
                 std::enable_if_t<magic_config::traits::is_container<Container>::value and
                                  magic_config::traits::is_map<Container>::value> >
{
    static Container convert(const folly::dynamic& json)
    {
        return detail::extractMapContainter<Container>(json);
    }
};


template <typename Container>
struct Converter<Container, std::enable_if_t<magic_config::magic::detail::is_magic_config<Container> > >
{
    static Container convert(const folly::dynamic& json)
    {
        return Container::load(json);
    }
};


}} // magic_config::json
