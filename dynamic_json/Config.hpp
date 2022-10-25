#pragma once

#include "Traits.hpp"
#include "../magic/Loader.hpp"

#include <string>

namespace magic_config { namespace dynamic_json {

template <typename Derived>
struct JsonConfig : magic::Loader<Derived, JsonTraits> {};

}} // magic_config::json
