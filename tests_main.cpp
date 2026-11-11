#include <gtest/gtest.h>

class FooTest : public testing::Test {};

TEST(FooTest, SameTestMethod) { EXPECT_EQ(1, 2); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
