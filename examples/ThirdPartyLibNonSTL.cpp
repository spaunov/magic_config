
// NOTE: We use the following Setup.hpp header to enable compilation of the examples
//       for both YamlConfig and JsonConfig. The default is YamlConfig.
//       The alias MagicConfig is used in the examples to refer to the configured type.
#include "Setup.hpp"

#include <string>
#include <vector>
#include <gtest/gtest.h>

// =======================================================================================
// NOTE: Let's create a fake library with a Container type which simulates some sort of
//       third-party lib which does not support STL interfaces. As expected, the
//       Magic Config Framework will not be able to accommodate this non-standard type
//       so we will need to teach it.

namespace FakeLib {

template <typename T>
struct Container : protected std::vector<T>
{
    using Base = std::vector<T>;

    // NOTE: We are hiding std::vector functionality such as the push_back method,
    //       iterators, various traits that std::vector defines, etc.

    // Selectively expose some functionality
    using Base::Base;
    using Base::operator[];
    using Base::reserve;
    using Base::resize;

    // NOTE: Introducing custom naming for insert/push_back functionality
    void addItem(const T& item) { Base::push_back(item); }
};

}  // namespace FakeLib

// =======================================================================================
// TEACH MAGIC CONFIG ABOUT THE NEW TYPE

#ifndef MAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON

// NOTE: Let's teach YamlConfig about this new type

#include "../yaml/Converters.hpp"

namespace magic_config { namespace yaml {

template <typename T>
struct Converter<FakeLib::Container<T>, void>
{
    using Container = FakeLib::Container<T>;

    static Container convert(const YAML::Node& node)
    {
        VERIFY_YAML(node.IsSequence(), "Type mismatch: array or list expected!", &node);

        Container result;

        for (auto const& item : node) {
            result.addItem(Converter<T>::convert(item));
        }

        return result;
    }
};

}}  // namespace magic_config::yaml

#else

// NOTE: Let's also teach JsonConfig about this new type

#include "../dynamic_json/Converters.hpp"

namespace magic_config { namespace dynamic_json {

template <typename T>
struct Converter<FakeLib::Container<T>, void>
{
    using Container = FakeLib::Container<T>;

    static Container convert(const folly::dynamic& json)
    {
        VERIFY_JSON(json.isArray(), "Type mismatch: array or list expected!", &json);

        Container result;

        for (auto const& item : json) {
            result.addItem(Converter<T>::convert(item));
        }

        return result;
    }
};

}}  // namespace magic_config::dynamic_json

#endif


// =======================================================================================
// BACK TO OUR TEST

namespace magic_config { namespace examples {

struct Employee : MagicConfig<Employee>  // Derive from MagicConfig
{
    std::string name;
    size_t      age;
    std::string phone;

    // Define a config mapping for the Employee class
    static void defineConfigMapping() {
        Employee::assign("name",  &Employee::name).required();
        Employee::assign("age",   &Employee::age).required();
        Employee::assign("phone", &Employee::phone);
    }
};

struct Employees : MagicConfig<Employees>  // Derive from MagicConfig
{
    using Data = FakeLib::Container<Employee>;

    static void defineConfigMapping() {
        Employees::assign("employees", &Employees::data).required();
    }

    Data data;
};

}}  // namespace magic_config::examples


TEST(MagicConfigExamples, third_party_lib_boost_small_vector)
{
    std::string jsonDoc =
        R"({
            "employees": [{
                           "name"  : "John Smith",
                           "age"   : 35
                          },
                          {
                           "name"  : "Chuck Norris",
                           "age"   : 82
                          },
                          {
                           "name"  : "Jean-Claude Van Damme",
                           "age"   : 62
                          },
                          {
                           "name"  : "Jason Statham",
                           "age"   : 55
                          },
                          {
                           "name"  : "Steven Seagal",
                           "age"   : 70
                          }
                         ]
          })";

    bool did_not_throw = false;

    try {
        auto config    = magic_config::examples::Traits::parse(jsonDoc);
        auto employees = magic_config::examples::Employees::load(config);

        did_not_throw = true;

        EXPECT_EQ(employees.data[1].name, "Chuck Norris");
        EXPECT_EQ(employees.data[3].name, "Jason Statham");

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
}
