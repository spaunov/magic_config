#pragma once

#include "import_yaml.hpp"

namespace magic_config { namespace yaml {

// -----------------------------------------------------------------------
// CONVERTER FORWARD DECLARATION:
//
// Defines interface for Converter<Target>
//
// Requires:
//    static Target Converter<Target>::convert(const YAML::Node& node)

template <typename Target, typename Enable = void>
struct Converter;

}}  // namespace magic_config::yaml
