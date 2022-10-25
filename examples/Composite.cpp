
// NOTE: We use the following Setup.hpp header to enable compilation of the examples
//       for both YamlConfig and JsonConfig. The default is YamlConfig.
//       The alias MagicConfig is used in the examples to refer to the configured type.
#include "Setup.hpp"

#include <string>
#include <set>
#include <vector>

#include <gtest/gtest.h>

namespace magic_config { namespace examples {

struct Employee : MagicConfig<Employee>  // Derive from MagicConfig
{
    static constexpr const size_t MinimumAge          = 18;
    static constexpr const size_t ForcedRetirementAge = 75;

    std::string name;
    size_t      age;
    std::string phone;

    // Define a config mapping for the Employee class
    static void defineConfigMapping() {
        Employee::assign("name",  &Employee::name).required();

        Employee::assign("age", &Employee::age)
            .required()
            .range(MinimumAge, ForcedRetirementAge);

        Employee::assign("phone", &Employee::phone);
    }
};

struct Department : MagicConfig<Department> {
    std::string name;
    size_t      budget = 0;

    static void defineConfigMapping() {
        Department::assign("name",   &Department::name).required();
        Department::assign("budget", &Department::budget);
    }
};

struct Company : MagicConfig<Company> {
    using Employees   = std::set<Employee, std::less<>>;
    using Departments = std::vector<Department>;

    std::string name;
    Employees   employees;
    Departments departments;

    static void defineConfigMapping() {
        Company::assign("name",        &Company::name).required();
        Company::assign("employees",   &Company::employees).required();
        Company::assign("departments", &Company::departments).required();
    }
};

// Operators to aid seamless search in std::set
bool operator<(const Employee& e1, const Employee& e2)
{
    return e1.name < e2.name;
}

bool operator<(const Employee& e1, const std::string& name)
{
    return e1.name < name;
}

bool operator<(const std::string& name, const Employee& e1)
{
    return name < e1.name;
}

}}  // namespace magic_config::examples


TEST(MagicConfigExamples, composite)
{
    std::string jsonDoc =
        R"({
            "name"        : "Magic Enterprises",
            "departments" : [{
                               "name"   : "Accounting"
                             },
                             {
                               "name"   : "Finance",
                               "budget" : 123456789
                             },
                             {
                               "name"   : "IT"
                             }
                            ],
            "employees": [{
                           "name"  : "John Smith",
                           "age"   : 35
                          },
                          {
                           "name"  : "Some Dude",
                           "age"   : 25
                          }
                         ]
          })";

    bool did_not_throw = false;

    try {
        auto config  = magic_config::examples::Traits::parse(jsonDoc);
        auto company = magic_config::examples::Company::load(config);

        EXPECT_EQ(company.name,               "Magic Enterprises");
        EXPECT_EQ(company.employees.size(),   2);
        EXPECT_EQ(company.departments.size(), 3);

        std::string John = "John Smith";
        auto        it   = company.employees.find(John);

        EXPECT_EQ(company.employees.count(John), 1);
        EXPECT_TRUE(it != company.employees.end());
        EXPECT_EQ(it->name, "John Smith");
        EXPECT_EQ(it->age,  35);

        EXPECT_EQ(company.departments[2].name,   "IT");
        EXPECT_EQ(company.departments[1].budget, 123456789);

        did_not_throw = true;

    } catch (std::exception& ex) {
        std::cout << "Exception caught: " << ex.what();
    }

    EXPECT_TRUE(did_not_throw);
}
