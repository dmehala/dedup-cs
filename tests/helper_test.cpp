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

TEST(Helper, splitTestWrongDelimiter)
{
    EXPECT_EQ(ExpectedSplit({"foo:bar::foobar"}), helper::split("foo:bar::foobar", ','));
}

TEST(Helper, splitTest)
{
    const ExpectedSplit expected ({"id", "year", "length", "genre", "directors", "actors"});
    EXPECT_EQ(expected, helper::split("id\tyear\tlength\tgenre\tdirectors\tactors", '\t'));
}

TEST(Helper, lcsTestSame)
{
    EXPECT_EQ(0, helper::lcs("", ""));
    EXPECT_EQ(6, helper::lcs("foobar", "foobar"));
}

TEST(Helper, lcsTest)
{
    EXPECT_EQ(3, helper::lcs("stone", "longest"));
}

TEST(Helper, jaccardSimilarityTestEmptyInput)
{
    const std::vector<int> s1 {};
    const std::vector<int> s2 { 5, 6, 7, 8 };
    EXPECT_EQ(0.f, helper::jaccard_similarity(s1, s2));
}

TEST(Helper, jaccardSimilarityTestDisjoint)
{
    const std::vector<int> s1 { 1, 2, 3, 4 };
    const std::vector<int> s2 { 5, 6, 7, 8 };
    EXPECT_EQ(0.f, helper::jaccard_similarity(s1, s2));
}

TEST(Helper, jaccardSimilarityTest)
{
    const std::vector<int> s1 { 1, 2, 3, 4 };
    const std::vector<int> s2 { 3, 4 };
    EXPECT_EQ(0.5, helper::jaccard_similarity(s1, s2));
}