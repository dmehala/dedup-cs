#include <gtest/gtest.h>
#include "dedup/helper.hpp"

using ExpectedSplit = std::vector<std::string>;

TEST(Helper, splitTestEmptyInput)
{
    EXPECT_EQ(ExpectedSplit({}), helper::split("", ','));
}

TEST(Helper, splitTestDelimitersInput)
{
    EXPECT_EQ(ExpectedSplit({"", ""}), helper::split(",", ','));
    EXPECT_EQ(ExpectedSplit({"", "", "", ""}), helper::split("...", '.'));
}

TEST(Helper, splitTestOneElement)
{
    EXPECT_EQ(ExpectedSplit({"foo"}), helper::split("foo", ':'));
    EXPECT_EQ(ExpectedSplit({"bar", ""}), helper::split("bar:", ':'));
    EXPECT_EQ(ExpectedSplit({"", "foobar"}), helper::split(":foobar", ':'));
}

TEST(Helper, splitWrongDelimiter)
{
    EXPECT_EQ(ExpectedSplit({"foo:bar::foobar"}), helper::split("foo:bar::foobar", ','));
}

TEST(Helper, split)
{
    const ExpectedSplit expected ({"id", "year", "length", "genre", "directors", "actors"});
    EXPECT_EQ(expected, helper::split("id\tyear\tlength\tgenre\tdirectors\tactors", '\t'));
}