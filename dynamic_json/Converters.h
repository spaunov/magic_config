#pragma once

#include "import_json.hpp"

namespace magic_config { namespace dynamic_json {

// -----------------------------------------------------------------------
// CONVERTER FORWARD DECLARATION:
//
// Defines interface for Converter<Target>
//
// Requires:
//    static Target Converter<Target>::convert(const folly::dynamic& json)

template <typename Target, typename Enable = void>
struct Converter;

}}  // namespace magic_config::json
