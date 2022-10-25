
// NOTE: We use the following Setup.hpp header to enable compilation of the examples
//       for both YamlConfig and JsonConfig. The default is YamlConfig.
//       The alias MagicConfig is used in the examples to refer to the configured type.
#include "Setup.hpp"

#include <string>
#include <boost/container/small_vector.hpp>
#include <gtest/gtest.h>

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

// NOTE: In this test we will use a boost::small_vector to store a set of employees
//       The test aims to show that this third-party class which conforms to the STL
//       interface is supported out-of-the-box without any extra work.
//
// NOTE: The framework requires only a limited STL-like interface to figure out what
//       to do. Loosely speaking the container should provide the following at a minimum:
//          - trait ::value_type
//          - trait ::iterator
//          - push_back or insert method
//          - be a sequence or a map container
//
// See container_traits.hpp in magic_config/magic/detail/traits/ and the respective
// Converter specializations for each magic config type for more details!

struct Employees : MagicConfig<Employees>  // Derive from MagicConfig
{
    using Data = boost::container::small_vector<Employee, 32>;

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

        // NOTE: It just works out of the box! Nothing for you to do, but enjoy :)
        EXPECT_EQ(employees.data[1].name, "Chuck Norris");
        EXPECT_EQ(employees.data[3].name, "Jason Statham");

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
}
