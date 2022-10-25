
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
    static constexpr const size_t MinimumAge          = 18;
    static constexpr const size_t ForcedRetirementAge = 75;

    std::string phone;
    bool        passed_post_validation = false;

    // Define a config mapping for the Employee class
    //
    // NOTE that magic_config supports defining a mapping which includes base class members!
    static void defineConfigMapping() {
        // NOTE: We have now declared that all members are required!
        Employee::assign("name",  &Employee::name).required();

        // NOTE: We now enforce that the employee age is between 18 and 80
        Employee::assign("age", &Employee::age)
            .required()
            .range(MinimumAge, ForcedRetirementAge);
        // NOTE: range is inclusive by default, but can be made exclusive via a third parameter!

        // NOTE: Mandatory phone requirement will cause the first test to fail!
        Employee::assign("phone", &Employee::phone).required();
    }

    // NOTE: If the class defines the following verify_config method,
    //       magic config will detect that and will call this function for
    //       post-validation once the config has been loaded into the object!
    bool verify_config(const typename Employee::Traits::Config& config) {
        // NOTE: Write any complex validation code which might involve
        //       relationships among multiple data members here!
        passed_post_validation = true;

        return true; // Inform the framework that post validation passed
    }
};


TEST(MagicConfigExamples, basic_validation_failed)
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
    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    // NOTE: Phone is now a required field, so load will not succeed
    EXPECT_FALSE(did_not_throw);
}


TEST(MagicConfigExamples, basic_validation_passed)
{
    std::string jsonDoc =
        R"({"employee": {
               "name"  : "John Smith",
               "age"   : 35,
               "phone" : "555-1234"
             }
         })";

    bool did_not_throw = false;

    try {
        auto config   = magic_config::examples::Traits::parse(jsonDoc);
        auto employee = Employee::load(config["employee"]);

        did_not_throw = true;

        EXPECT_EQ(employee.name, "John Smith");
        EXPECT_EQ(employee.age, 35);
        EXPECT_FALSE(employee.phone.empty());
        EXPECT_TRUE(employee.passed_post_validation);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
}

}}  // namespace magic_config::examples
