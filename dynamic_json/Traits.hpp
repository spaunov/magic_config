#pragma once

#include "import_json.hpp"
#include "Converters.hpp"

#include <string>

namespace magic_config { namespace dynamic_json {

struct JsonTraits
{
   using Name   = std::string;
   using Config = folly::dynamic;

   template <typename Target>
   using Converter = magic_config::dynamic_json::Converter<Target>;

   static bool isValid(const Config& json)   { return !json.isNull();   }
   static bool isScalar(const Config& json)  { return !json.isObject(); }
   static bool isArray(const Config& json)   { return  json.isArray(); }
   static bool isComplex(const Config& json) { return  json.isArray() || json.isObject(); }

   static bool isPresent(const Config& json, const Name& name)
   {
      return json.count(name);
   }

   static const Config& get(const Config& json, const Name& name)
   {
      return json[name];
   }

   static Config parse(const std::string& jsonString)
   {
      return folly::parseJson(jsonString);
   }
};

}} // magic_config::json
