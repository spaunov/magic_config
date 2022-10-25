
#include "../Traits.hpp"
#include "../Converters.hpp"

#include "../../magic/detail/type2str.hpp"

#include <functional>
#include <bitset>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <gtest/gtest.h>
#include <stdint.h>

using Traits = magic_config::dynamic_json::JsonTraits;
using Config = typename Traits::Config;

template <typename Target>
using Converter = Traits::Converter<Target>;

template <typename TargetType>
class JsonConverter : public ::testing::Test {
 public:
   using Target = TargetType;

   std::function<void(const Config& json)>      check_json;
   std::function<void(const Target& extracted)> check_result;

   // Convert json into target type and invoke check_result()
   void convertAndCheck(const std::string& jsonDoc);

   // Convert json into target type and confirm the result
   void convertAndCheck(const std::string& jsonDoc, Target expected)
   {
      check_result = [&](const Target& extracted) {
         EXPECT_EQ(extracted, expected);
      };

      convertAndCheck(jsonDoc);
   }

   // Try converting json into target type, but expect an exception thrown
   void confirmError(const std::string& jsonDoc);
};


template <typename TargetType>
void JsonConverter<TargetType>::convertAndCheck(const std::string& jsonDoc)
{
   Config json = folly::parseJson(jsonDoc);

   std::cout << "Testing " << magic_config::detail::type_to_str<TargetType>() << "\n";

   auto const& item = json["item"];

   if (check_json) {
      check_json(item);
   }

   auto extracted = Converter<TargetType>::convert(item);

   ASSERT_TRUE(check_result);
   check_result(extracted);
}

template <typename TargetType>
void JsonConverter<TargetType>::confirmError(const std::string& jsonDoc)
{
   size_t numExceptions = 0;

   check_result = [&](const Target&) {
      ASSERT_TRUE(false); // We should never get here
   };

   try {
      convertAndCheck(jsonDoc);
   } catch (const std::exception&) {
      ++numExceptions;
   }

   EXPECT_EQ(numExceptions, 1);
}


// ================
// TESTS:

using JsonBoolConverter = JsonConverter<bool>;

TEST_F(JsonBoolConverter, convert_bool)
{
   std::string jsonDoc = R"({"item" : true})";

   this->check_json = [](const Config& item) {
      EXPECT_TRUE(item.isBool());
      EXPECT_TRUE(item.asBool());
   };

   this->convertAndCheck(jsonDoc, true);
}


using JsonStringConverter = JsonConverter<std::string>;

TEST_F(JsonStringConverter, convert_string)
{
   std::string jsonDoc = R"({"item" : "myString"})";

   constexpr const char* EXPECTED = "myString";

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isString());
      EXPECT_EQ(item.asString(), EXPECTED);
   };

   this->convertAndCheck(jsonDoc, EXPECTED);
}


// ===============
// Test char types:

template <typename TargetType>
using JsonCharConverter = JsonConverter<TargetType>;

using CharTypes = testing::Types<char, unsigned char, int8_t, uint8_t>;

TYPED_TEST_CASE(JsonCharConverter, CharTypes);

TYPED_TEST(JsonCharConverter, convert_char)
{
   std::string jsonDoc = R"({"item" : "Z"})";

   constexpr const char EXPECTED = 'Z';

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isString());
      EXPECT_EQ(item.size(), 1);
      EXPECT_EQ(item.asString()[0], EXPECTED);
   };

   this->convertAndCheck(jsonDoc, EXPECTED);
}

// ==============
// Test int types:

template <typename TargetType>
using JsonIntConverter = JsonConverter<TargetType>;

using IntTypes = testing::Types<uint16_t, uint32_t, uint64_t,
                                 int16_t,  int32_t,  int64_t>;

TYPED_TEST_CASE(JsonIntConverter, IntTypes);

TYPED_TEST(JsonIntConverter, convert_ints)
{
   std::string jsonDoc = R"({"item" : 32156})";

   constexpr int EXPECTED = 32156;

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(item.asInt(), EXPECTED);
   };

   this->convertAndCheck(jsonDoc, EXPECTED);
}

// ===========================
// Test int overflow detection:

template <typename TargetType>
using JsonIntOverflowTest = JsonConverter<TargetType>;

using IntOverflowTestTypes = testing::Types<uint16_t, int16_t>;

TYPED_TEST_CASE(JsonIntOverflowTest, IntOverflowTestTypes);

TYPED_TEST(JsonIntOverflowTest, convert_ints)
{
   std::string jsonDoc = R"({"item" : 654321})";

   // json value does not match precision
   this->confirmError(jsonDoc);
}

// ================
// Test float types:

template <typename TargetType>
using JsonFloatConverter = JsonConverter<TargetType>;

using FloatTypes = testing::Types<float, double>;

TYPED_TEST_CASE(JsonFloatConverter, FloatTypes);

TYPED_TEST(JsonFloatConverter, convert_floating_points)
{
   std::string jsonDoc = R"({"item" : 123.456})";

   constexpr double EXPECTED = 123.456;

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isDouble());
      EXPECT_EQ(item.asDouble(), EXPECTED);
   };

   this->convertAndCheck(jsonDoc, EXPECTED);
}


// ===============================
// Test sequential container types:

template <typename TargetType>
using JsonSequenceConverter = JsonConverter<TargetType>;

using SequenceTypes = testing::Types<std::array<int, 4>,
                                     std::list<int>,
                                     std::vector<int>,
                                     std::set<int> >;

TYPED_TEST_CASE(JsonSequenceConverter, SequenceTypes);

TYPED_TEST(JsonSequenceConverter, convert_sequence)
{
   using Self = std::remove_pointer_t<decltype(this)>;

   std::string jsonDoc = R"({"item" : [1, 2, 3, 4]})";

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isArray());
   };

   this->check_result = [&](const typename Self::Target& extracted) {
      EXPECT_EQ(4, extracted.size());

      size_t expected = 1;
      for (auto& item : extracted) {
         EXPECT_EQ(item, expected++);
      }
   };

   this->convertAndCheck(jsonDoc);
}

// ================================
// Test associative container types:

template <typename TargetType>
using JsonAssociativeConverter = JsonConverter<TargetType>;

using AssociativeTypes = testing::Types<std::set<int>,
                                        std::unordered_set<int> >;

TYPED_TEST_CASE(JsonAssociativeConverter, AssociativeTypes);

TYPED_TEST(JsonAssociativeConverter, convert_associative_containers)
{
   using Self = std::remove_pointer_t<decltype(this)>;

   std::string jsonDoc = R"({"item" : [1, 2, 3, 4]})";

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isArray());
   };

   this->check_result = [&](const typename Self::Target& extracted) {
      EXPECT_EQ(4, extracted.size());

      for (size_t i = 1; i <= extracted.size(); ++i)
      {
         EXPECT_TRUE(extracted.find(i) != extracted.end());
      }
   };

   this->convertAndCheck(jsonDoc);
}

// ================================
// Test map container types:

template <typename TargetType>
using JsonMapConverter = JsonConverter<TargetType>;

using MapTypes = testing::Types<std::map<std::string, int>,
                                std::unordered_map<std::string, int> >;

TYPED_TEST_CASE(JsonMapConverter, MapTypes);

TYPED_TEST(JsonMapConverter, convert_map_containers)
{
   using Self = std::remove_pointer_t<decltype(this)>;

   std::string jsonDoc = R"({"item" : { "1" : 1, "2" : 2, "3" : 3, "4" : 4 } })";

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isObject());
   };

   std::bitset<5> seen;

   this->check_result = [&](const typename Self::Target& extracted) {
      EXPECT_EQ(extracted.size(),  4);

      for (auto& item : extracted) {
         seen.set(item.second);

         char key[128];
         snprintf(key, sizeof(key), "%u", item.second);
         EXPECT_EQ(key,      item.first);
      }
   };

   this->convertAndCheck(jsonDoc);

   EXPECT_EQ(seen.count(),  4);
}



using JsonNonStringMapConverter = JsonConverter<std::map<int, int> >;

TEST_F(JsonNonStringMapConverter, convert_map)
{
   using Self = std::remove_pointer_t<decltype(this)>;

   std::string jsonDoc =
      R"({"item" : { "1" : 1, "2" : 2, "3" : 3, "4" : 4 } })";

   this->check_json = [&](const Config& item) {
      EXPECT_TRUE(item.isObject());
   };

   this->check_result = [&](const typename Self::Target& extracted) {
      size_t expected = 1;
      for (auto& item : extracted) {
         EXPECT_EQ(item.first,  expected);
         EXPECT_EQ(item.second, expected);
         ++expected;
      }
   };

   this->convertAndCheck(jsonDoc);
}
