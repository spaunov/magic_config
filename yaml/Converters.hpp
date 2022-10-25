#pragma once

#include "ConvertersAux.hpp"

#include <array>
#include <limits>
#include <utility>
#include <type_traits>

namespace magic_config { namespace yaml {

// -----------------------------------------------------------------------
// CONVERTER SPECIALIZATIONS:

template <>
struct Converter<bool>
{
    static bool convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsScalar(), "Incompatible boolean type!", &node);
        return node.as<bool>();
    }
};

template <>
struct Converter<std::string>
{
    static std::string convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsScalar(), "Incompatible string type!", &node);
        return node.as<std::string>();
    }
};


template <typename Target>
struct Converter<Target, std::enable_if_t<magic_config::traits::is_supported_char_v<Target> > >
{
    static Target convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsScalar(), "Incompatible char type!", &node);
        return node.as<Target>();
    }
};


template <typename Target>
struct Converter<Target, std::enable_if_t<std::is_enum_v<Target> > >
{
    using target_type = std::underlying_type_t<Target>;

    static target_type convert(const YAML::Node& node) {
        return static_cast<target_type>(Converter<target_type>::convert(node));
    }
};

template <typename Target>
struct Converter<Target, std::enable_if_t<magic_config::traits::is_supported_integer_v<Target> > >
{
    static Target convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsScalar(), "Incompatible integral type!", &node);

        return node.as<Target>();
    }
};

template <typename Target>
struct Converter<Target,
                 std::enable_if_t<std::is_floating_point_v<Target>> >
{
    static Target convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsScalar(),
                    "Incompatible floating point type!", &node);

        return node.as<Target>();
    }
};

template <typename Key, typename Value>
struct Converter< std::pair<Key, Value> >
{
    using target_type = std::pair<Key, Value>;

    static target_type convert(const YAML::Node& node)
    {
        return detail::extractPair<target_type>(node);
    }
};

template <typename Target, size_t SIZE>
struct Converter< std::array<Target, SIZE> >
{
    using target_type = std::array<Target, SIZE>;

    static target_type convert(const YAML::Node& node)
    {
        return detail::extractArray<target_type>(node);
    }
};

template <typename Container>
struct Converter<Container,
                 std::enable_if_t<magic_config::traits::is_container<Container>::value and
                                  not magic_config::traits::is_map<Container>::value> >
{
    static Container convert(const YAML::Node& node)
    {
        return detail::extractSequence<Container>(node);
    }
};

template <typename Container>
struct Converter<Container,
                 std::enable_if_t<magic_config::traits::is_container<Container>::value and
                                  magic_config::traits::is_map<Container>::value> >
{
    static Container convert(const YAML::Node& node)
    {
        return detail::extractMapContainter<Container>(node);
    }
};


template <typename Container>
struct Converter<Container, std::enable_if_t<magic_config::magic::detail::is_magic_config<Container> > >
{
    static Container convert(const YAML::Node& node)
    {
        return Container::load(node);
    }
};


}} // magic_config::yaml
