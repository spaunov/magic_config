#pragma once

#include "traits.hpp"

namespace magic_config { namespace magic { namespace detail {

// DispatchConverter Interface: Used to dispatch type converters. See specializations below!
template <typename Traits, typename Target, bool isComplexType>
struct DispatchConverter;

// Specialization for simple type
template <typename Traits, typename Target>
struct DispatchConverter<Traits, Target, false>
{
    static Target convert(const typename Traits::Config& cfg)
    {
        return Traits::template Converter<Target>::convert(cfg);
    }
};

// Specialization for complex type
template <typename Traits, typename ComplexType>
struct DispatchConverter<Traits, ComplexType, true>
{
    static ComplexType convert(const typename Traits::Config& cfg)
    {
        return ComplexType::load(cfg);
    }
};

// Converter dispatch method
template <typename Traits, typename Target>
Target convert(const typename Traits::Config& cfg)
{
    const bool isComplex = magic::detail::is_magic_config<Target>;

    return DispatchConverter<Traits, Target, isComplex>::convert(cfg);
}

}}} // magic_config::magic::detail
