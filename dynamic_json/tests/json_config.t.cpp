
#include "../Config.hpp"

#include <string>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <functional>

#include <gtest/gtest.h>

#include <stdio.h>

namespace detail {

template <typename Container>
using Check = std::function<bool (const Container&)>;

template <typename Target>
Target convert(const folly::dynamic& json)
{
   using Traits = magic_config::dynamic_json::JsonTraits;
   return magic_config::magic::detail::convert<Traits, Target>(json);
}

} // end of namespace detail

class JsonConfigTestFixture : public ::testing::Test {
 public:

    void SetUp() {
        std::ifstream t("test_script.json");
        std::string jsonStr((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());

        m_config = folly::parseJson(jsonStr);
    }

   folly::dynamic m_config;
};

namespace
{

template <typename First, typename Second>
std::ostream& operator<<(std::ostream& os, const std::pair<First,Second>& pair)
{
    std::cout << pair.first << " --> " << pair.second;
    return os;
}

template <typename Container>
void print(const Container& container, std::ostream& os=std::cout)
{
    typedef typename Container::value_type value_type;

    for(auto const& item : container)
    {
        os << item << ", ";
    }
    os << std::endl;
}


template <typename Container>
void testConvert(folly::dynamic& global, const char* objName,
                 detail::Check<Container> check = [](auto const&) { return true; })
{
    try {
        auto const& json = global[objName];
        Container container = detail::convert<Container>(json);
        print(container);
        check(container);
    } catch (const std::exception& ex) {
        EXPECT_TRUE(false) << "Exception caught: " << ex.what() << " for "
                           << objName << std::endl;
    }
}

template <typename Target>
Target extract(const folly::dynamic& json)
{
    try {
        return detail::convert<Target>(json);
    } catch (const std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        throw; // rethrow
    }
}


struct ConfigBase
{
    std::map<std::string, int> dict;
};

struct ConfigTest : ConfigBase
                  , magic_config::dynamic_json::JsonConfig<ConfigTest>
{
    std::list<int>             list;

    static void defineConfigMapping() {
        ConfigTest ::assign("dict", &ConfigTest ::dict).required();
        ConfigTest ::assign("list", &ConfigTest ::list);
    }
};

struct ComplexConfig : public magic_config::dynamic_json::JsonConfig<ComplexConfig>
{
    ConfigTest  complex;

    bool verify_config(const folly::dynamic& json) {
        // TODO(spaunov): Need a proper test for this
        std::cout << "Called verify_config!\n";
        verifyConfigCalled = true;
        return true;
    }

    static void defineConfigMapping() {
        ComplexConfig::assign("complex", &ComplexConfig::complex);
    }

   bool verifyConfigCalled = false;
};

}

TEST(JsonConfig, breathing)
{
    std::ifstream t("test_script.json");
    std::string jsonStr((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

    folly::dynamic json = folly::parseJson(jsonStr);

    std::cout << json << std::endl;


    try
    {
        printf("number = %d\n", extract<int>(json["number"]));
        printf("animal = %s\n", extract<std::string>(json["animal"]).c_str());

        testConvert<std::list<int> >(json, "mylist");
        testConvert<std::vector<int> >(json, "mylist");
        testConvert<std::set<int> >(json, "mylist");

        testConvert<std::map<std::string, int> >(json, "mydict");

        testConvert<std::unordered_map<std::string, int> >(json, "mydict");

        std::cout << "Testing complex:\n";
        ComplexConfig test = ComplexConfig::load(json["conf"]);

        EXPECT_TRUE(test.verifyConfigCalled);

        print(test.complex.dict);
        print(test.complex.list);
    }
    catch(std::exception& ex)
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
    }
}

TEST(JsonConfig, missingConfig_required)
{
   std::string jsonDoc =
      R"({"conf": {
      "complex": {
      }
 }})";

   folly::dynamic json = folly::parseJson(jsonDoc);

   std::cout << json << std::endl;

    try
    {
       std::cout << "Testing complex:\n";
       ComplexConfig test = ComplexConfig::load(json["conf"]);
       EXPECT_TRUE(false); // We expected an exception to be thrown

    }
    catch(std::exception& ex)
    {
       // NOTE: We expect an exception due to missing 'dict' member, so no error
    }
}


TEST(JsonConfig, missingConfig_optional)
{
   std::string jsonDoc =
      R"({"conf": {
      "complex": {
         "dict": {
            "A": 1,
            "B": 2
         }
      }
   }})";

   const size_t expectedDictSize = 2;
   const size_t expectedListSize = 0;


   folly::dynamic json = folly::parseJson(jsonDoc);

   std::cout << json << std::endl;

   std::cout << "Testing complex:\n";
   ComplexConfig test = ComplexConfig::load(json["conf"]);

   EXPECT_TRUE(test.verifyConfigCalled);

   print(test.complex.dict);
   EXPECT_EQ(expectedDictSize, test.complex.dict.size());
   print(test.complex.list);
   EXPECT_EQ(expectedListSize, test.complex.list.size());
}

bool g_properyActionInvoked = false;

TEST(JsonConfig, property_action)
{
   const size_t expectedDictSize = 2;
   g_properyActionInvoked = false;

   struct TestObj : public magic_config::dynamic_json::JsonConfig<TestObj> {
      std::map<std::string, int> dict;

      static void defineConfigMapping() {
         TestObj::assign("dict", &TestObj::dict)
            .required()
            .attach([](auto const& dict){
                  g_properyActionInvoked = true;
                  if (dict.size() != 2) throw std::logic_error("Found issue!");
               })
            .cardinality(expectedDictSize);
      }
   };

   std::string jsonDoc =
      R"({"conf": {
         "dict": {
            "A": 1,
            "B": 2
         }
        }})";

   folly::dynamic json = folly::parseJson(jsonDoc);

   std::cout << json << std::endl;

   std::cout << "Testing complex:\n";
   TestObj test = TestObj::load(json["conf"]);
   EXPECT_TRUE(g_properyActionInvoked);

   print(test.dict);
   EXPECT_EQ(expectedDictSize, test.dict.size());
}

TEST(JsonConfig, range_check)
{
   std::string jsonDoc =
      R"({"conf": {
            "power": 7
          }
         })";

   struct TestObj : public magic_config::dynamic_json::JsonConfig<TestObj> {
      size_t outputPower = 0;

      static void defineConfigMapping() {
         TestObj::assign("power", &TestObj::outputPower)
            .required()
            .range(1, 8);
      }
   };


   folly::dynamic json = folly::parseJson(jsonDoc);

   TestObj test = TestObj::load(json["conf"]);
   EXPECT_EQ(7, test.outputPower);
}
