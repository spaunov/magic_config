#include "define_trait.hpp"

#include <gtest/gtest.h>

struct Tester {
   using type = int;

   type getType() { return type(); }
};

namespace detail {
DEFINE_HAS_TRAIT(type);
DEFINE_HAS_TRAIT(goo);

DEFINE_HAS_MEMBER(getType);
DEFINE_HAS_MEMBER(zzz);

} // end of namespace detail

TEST(traits, has_trait) {
   EXPECT_TRUE((std::is_same<int, typename Tester::type>::value));
   EXPECT_TRUE(detail::has_trait_type<Tester>::value);
   EXPECT_FALSE(detail::has_trait_goo<Tester>::value);

   EXPECT_TRUE(detail::has_member_getType<Tester>::value);
   EXPECT_FALSE(detail::has_member_zzz<Tester>::value);
}
