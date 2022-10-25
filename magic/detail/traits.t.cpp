#include "traits.hpp"

#include <gtest/gtest.h>

#include <vector>
#include <set>

using namespace magic_config::traits;


TEST(interface_check, push_back) {
   EXPECT_TRUE(has_push_back_method<std::vector<int> >::value);
   EXPECT_FALSE(has_push_back_method<std::set<int> >::value);
}

TEST(interface_check, insert) {
   EXPECT_FALSE(has_insert_method<std::vector<int> >::value);
   EXPECT_TRUE(has_insert_method<std::set<int> >::value);
}

struct NullClass {};
struct Config {};

struct TestVerify {
   bool verify_config(const Config&) { return true; }
};

TEST(interface_check, verify_config) {
   EXPECT_FALSE((has_verify_config<NullClass, Config>::value));
   EXPECT_TRUE((has_verify_config<TestVerify, Config>::value));
}
