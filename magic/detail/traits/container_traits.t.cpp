#include "container_traits.hpp"

#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include <gtest/gtest.h>

using namespace magic_config::traits;

TEST(traits, vector) {
   using test_type = std::vector<int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_FALSE((is_associative<test_type>::value));
   EXPECT_FALSE((is_map<test_type>::value));
}

TEST(traits, list) {
   using test_type = std::list<int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_FALSE((is_associative<test_type>::value));
   EXPECT_FALSE((is_map<test_type>::value));
}

TEST(traits, set) {
   using test_type = std::set<int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_TRUE((is_associative<test_type>::value));
   EXPECT_FALSE((is_map<test_type>::value));
}

TEST(traits, unordered_set) {
   using test_type = std::unordered_set<int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_TRUE((is_associative<test_type>::value));
   EXPECT_FALSE((is_map<test_type>::value));
}

TEST(traits, map) {
   using test_type = std::map<int, int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_TRUE((is_associative<test_type>::value));
   EXPECT_TRUE((is_map<test_type>::value));
}


TEST(traits, unordered_map) {
   using test_type = std::unordered_map<int, int>;

   EXPECT_TRUE((is_container<test_type>::value));
   EXPECT_TRUE((is_range<test_type>::value));
   EXPECT_TRUE((is_associative<test_type>::value));
   EXPECT_TRUE((is_map<test_type>::value));
}
