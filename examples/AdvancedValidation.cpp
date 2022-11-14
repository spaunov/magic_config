
// NOTE: We use the following Setup.hpp header to enable compilation of the examples
//       for both YamlConfig and JsonConfig. The default is YamlConfig.
//       The alias MagicConfig is used in the examples to refer to the configured type.
#include "Setup.hpp"

// Support for std::optional
#include "../magic/detail/traits/optional.hpp"


#include <string>
#include <gtest/gtest.h>

namespace magic_config { namespace examples {

// Forward declaration of our custom property check
// WARNING: The actual implementation is not for the faint of heart and
//          requires pretty strong understanding of templates and this framework!
template <typename Class, typename Member>
struct VotingAgeCheck;

// NOTE: User-defined property checks and property lambda checks are attached in
//       static member function context so they cannot capture local state!

// Some horrendous globals for test purposes:
bool g_checked_voting_age     = false;
bool g_checked_phone_format   = false;


// Employee definition
struct Employee : MagicConfig<Employee>  // Derive from MagicConfig
{
    static constexpr const size_t MinimumAge          = 18;
    static constexpr const size_t ForcedRetirementAge = 75;
    static constexpr const size_t VotingAge           = 21;

    std::string                name;
    size_t                     age;
    std::optional<std::string> phone;
    bool                       passed_post_validation = false;

    // Define a config mapping for the Employee class. See below!
    static void defineConfigMapping();

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

// This function got a bit lengthy so let's define it out of line
void Employee::defineConfigMapping()
{
    // NOTE: We have now declared that all members are required!
    Employee::assign("name", &Employee::name).required();

    // NOTE: Now let's add our user-defined VotingAgeCheck
    // NOTE: Please read the WARNINGs above.
    Employee::assign("age", &Employee::age)
        .required()
        .range(MinimumAge, ForcedRetirementAge)
        .attach<VotingAgeCheck>(VotingAge);

    // NOTE: Now let's attach a custom lambda function to check, well, anything
    // we want!!!
    Employee::assign("phone", &Employee::phone)
        .attach([](auto const& phone) {
            g_checked_phone_format = true;

            if (!phone) return; // nothing to do

            if (phone->find('-') == std::string::npos) {
                throw std::logic_error("Phone has no dashes!");
            }
        });
}

// WARNING: This should be done only by advanced users. Basic users should opt for
//          attaching a lamba function instead! See below.
//
// WARNING: Defining custom property checks via the magic_config::IPropertyCheck
//          requires more than cursory understanding of templates and this framework.
//          It is also likely not necessary as you can simply attach a lambda.
//
// NOTE: If you have gotten past the warnings above unshaken, then here is an example
//       of how to define a custom property check.
//       See magic_config/magic/PropertyChecks.h for examples.
template <typename Class, typename Member>
struct VotingAgeCheck : magic_config::IPropertyCheck<Class>
{
    VotingAgeCheck(Member Class::*member, size_t votingAge)
        : m_member(member)
        , m_votingAge(votingAge) {
    }

    // Check if the member value passes our voting age check
    void check(Class& obj) override {
        // NOTE: Use magic_config::has_value & magic_config::get to
        //       ensure that the code works with std::optional

        if (!magic_config::has_value(obj.*m_member)) return;

        auto const& value = magic_config::get(obj.*m_member);

        g_checked_voting_age = true;

        if (value < m_votingAge) {
            throw std::logic_error("Voting age requirement failed!");
        }
    }

private:
    Member Class::*m_member;
    size_t         m_votingAge;
};


TEST(MagicConfigExamples, advanced_validation_passed)
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
        EXPECT_TRUE(employee.phone.has_value());
        EXPECT_TRUE(employee.passed_post_validation);

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
    EXPECT_TRUE(g_checked_voting_age);
    EXPECT_TRUE(g_checked_phone_format);
}

}}  // namespace magic_config::examples
