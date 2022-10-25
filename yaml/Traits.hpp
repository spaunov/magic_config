#pragma once

#include "import_yaml.hpp"
#include "Converters.hpp"

#include <string>

namespace magic_config { namespace yaml {

struct YamlTraits
{
   using Name   = std::string;
   using Config = YAML::Node;

   template <typename Target>
   using Converter = magic_config::yaml::Converter<Target>;

   static bool isValid(const Config& node)   { return !node.IsNull() && !node.IsDefined(); }
   static bool isScalar(const Config& node)  { return node.IsScalar(); }
   static bool isArray(const Config& node)   { return node.IsSequence(); }
   static bool isComplex(const Config& node) { return node.IsSequence() || node.IsMap(); }

   static bool isPresent(const Config& node, const Name& name)
   {
      try
      {
         if(node[name]) return true;
      }
      catch(YAML::InvalidNode& ex) { /* fall through */ }

      return false;
   }

   static Config get(const Config& node, const Name& name)
   {
      return node[name];
   }

   static Config parse(const std::string& yamlString)
   {
      return YAML::Load(yamlString);
   }
};

}} // magic_config::yaml
