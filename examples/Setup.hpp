
#ifndef MAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON
#include "../yaml/Config.hpp"
#else
#include "../dynamic_json/Config.hpp"
#endif

namespace magic_config { namespace examples {

// NOTE: Use the preprocessor define below to compile examples for JsonConfig
#ifndef MAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON

// By default examples use YamlConfig:
template <typename Derived, typename... Bases>
using MagicConfig = magic_config::yaml::YamlConfig<Derived, Bases...>;
using Traits      = magic_config::yaml::YamlTraits;

#else // Use JsonConfig instead

template <typename Derived, typename... Bases>
using MagicConfig = magic_config::dynamic_json::JsonConfig<Derived, Bases...>;
using Traits      = magic_config::dynamic_json::JsonTraits;

#endif

}}  // namespace magic_config::examples
