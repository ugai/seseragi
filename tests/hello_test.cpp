#include <gtest/gtest.h>

TEST(SeseragiTest, BasicAssertions) {
  EXPECT_STRNE("GNU", "GNU is Not Unix");
  EXPECT_EQ(6 + 6 + 6 + 6 * 0, 18);
}
