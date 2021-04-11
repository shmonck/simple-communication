#include <gtest/gtest.h>

#include <comm/lib.h>

TEST(SimpleTest, BasicAssertions)
{
    ASSERT_EQ(lib::foo(), 42);
}
