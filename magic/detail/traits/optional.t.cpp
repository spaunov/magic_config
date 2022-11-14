#include "optional.hpp"

#include <gtest/gtest.h>

using namespace magic_config;

TEST(traits, optional) {
    EXPECT_TRUE(( traits::is_optional_v<std::optional<int>>));
    EXPECT_FALSE((traits::is_optional_v<int>));
}


TEST(traits, optional_has_value) {
    int                x = 5;
    std::optional<int> y = x;
    std::optional<int> nv;

    EXPECT_TRUE(has_value(x));
    EXPECT_TRUE(has_value(y));
    EXPECT_FALSE(has_value(nv));
}


TEST(traits, optional_get) {
    int                x = 5;
    std::optional<int> y = x;
    std::optional<int> nv;

    EXPECT_EQ(get(x), x);
    EXPECT_EQ(get(y), x);
    EXPECT_THROW(get(nv), std::logic_error);
}
