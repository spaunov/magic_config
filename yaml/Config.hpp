#pragma once

#include "Traits.hpp"
#include "../magic/Loader.hpp"

#include <string>

namespace magic_config { namespace yaml {

template <typename Derived>
struct YamlConfig : magic::Loader<Derived, YamlTraits> {};

}} // magic_config::yaml
