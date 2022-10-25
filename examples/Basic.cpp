
// NOTE: We use the following Setup.hpp header to enable compilation of the examples
//       for both YamlConfig and JsonConfig. The default is YamlConfig.
//       The alias MagicConfig is used in the examples to refer to the configured type.
#include "Setup.hpp"

#include <string>
#include <gtest/gtest.h>

namespace magic_config { namespace examples {

// Base Employee class
struct EmployeeBase
{
    std::string name;
    size_t      age;
};

// Extended Employee class with extra info
struct Employee : EmployeeBase
                , MagicConfig<Employee>  // Derive from MagicConfig
{
    std::string phone;

    // Define a config mapping for the Employee class
    //
    // NOTE that magic_config supports defining a mapping which includes base class members!
    static void defineConfigMapping() {
        Employee::assign("name",  &Employee::name);
        Employee::assign("age",   &Employee::age);
        Employee::assign("phone", &Employee::phone);
    }
};


TEST(MagicConfigExamples, basic)
{
    std::string jsonDoc =
        R"({"employee": {
               "name" : "John Smith",
               "age"  : 35
             }
         })";

    bool did_not_throw = false;

    try {
        auto config   = magic_config::examples::Traits::parse(jsonDoc);
        auto employee = Employee::load(config["employee"]);

        did_not_throw = true;

        EXPECT_EQ(employee.name, "John Smith");
        EXPECT_EQ(employee.age, 35);
        EXPECT_TRUE(employee.phone.empty());

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
}

}}  // namespace magic_config::examples
