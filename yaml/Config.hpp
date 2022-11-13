#pragma once

#include "Traits.hpp"
#include "../magic/Loader.hpp"

#include <string>

namespace magic_config { namespace yaml {

// Description of arguments:
//   Derived: The top level config class which inherits this functionality
//     Bases: A list of base classes to be inherited into Derived, which are
//            each also magic configs themselves.
template <typename Derived, typename... Bases>
struct YamlConfig : magic::Loader<YamlTraits, Derived, Bases...> {};

}} // magic_config::yaml
