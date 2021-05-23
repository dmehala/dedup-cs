#include <gtest/gtest.h>
#include <strstream>
#include "dedup/csv.hpp"

TEST(Csv, empty)
{
    {   // Empty content
        std::stringstream ss;

        csv::Reader reader(ss);
        csv::Row row = reader.read_row();
        EXPECT_TRUE(row.empty());
    }
    {   /// Header only
        std::stringstream ss;
        ss << "A,B,C,D" << std::endl
           << ""        << std::endl;

        csv::Reader reader(ss);
        csv::Row row = reader.read_row();
        EXPECT_TRUE(row.empty());
    }
}

TEST(Csv, queryColumn)
{
    std::stringstream ss;
    ss << "A,B,C,D" << std::endl
       << "1,2,3,4";

    csv::Reader reader(ss);
    csv::Row row = reader.read_row();
    ASSERT_TRUE(!row.empty());
    EXPECT_THROW(row["unknown"], std::runtime_error);
    EXPECT_EQ("3", row["C"]);
}

// TEST(Csv, variousNewline)
// {
//     {   // Carriage Return
//         const char newline = '\r';
//         std::stringstream ss;
//         ss << "A,B,C" << newline
//            << "1,2,3" << newline;

//         csv::Reader reader(ss);
//         csv::Row row = reader.read_row();
//         ASSERT_TRUE(!row.empty());
//         EXPECT_EQ("1", row["A"]);
//         EXPECT_EQ("2", row["B"]);
//         EXPECT_EQ("3", row["C"]);
//     }
//     {   // Line Feed
//         const char newline = '\n';
//         std::stringstream ss;
//         ss << "D,E,F" << newline
//            << "4,5,6" << newline;

//         csv::Reader reader(ss);
//         csv::Row row = reader.read_row();
//         ASSERT_TRUE(!row.empty());
//         EXPECT_EQ("4", row["D"]);
//         EXPECT_EQ("5", row["E"]);
//         EXPECT_EQ("6", row["F"]);
//     }
//     {   // CR + LF
//         const char newline[] = "\n\r";
//         std::stringstream ss;
//         ss << "G,H,I" << newline
//            << "7,8,9" << newline;

//         csv::Reader reader(ss);
//         csv::Row row = reader.read_row();
//         ASSERT_TRUE(!row.empty());
//         EXPECT_EQ("7", row["G"]);
//         EXPECT_EQ("8", row["H"]);
//         EXPECT_EQ("9", row["I"]);
//     }
// }

TEST(Csv, singleColumn)
{
    std::stringstream ss;
    ss << "Id" << std::endl
       << "0"  << std::endl
       << "1"  << std::endl;

    csv::Reader reader(ss);

    // First row
    csv::Row row = reader.read_row();
    ASSERT_TRUE(!row.empty());
    EXPECT_EQ("0", row["Id"]);

    // Second row
    row = reader.read_row();
    ASSERT_TRUE(!row.empty());
    EXPECT_EQ("1", row["Id"]);
}

TEST(Csv, tsvSeparator)
{
    std::stringstream ss;
    ss << "foo\tbar\tfoo,bar"   << std::endl
       << "0\t1\tHello, World!" << std::endl;

    csv::Reader reader(ss, csv::separator::tab);
    csv::Row row = reader.read_row();
    ASSERT_TRUE(!row.empty());
    EXPECT_EQ("0", row["foo"]);
    EXPECT_EQ("1", row["bar"]);
    EXPECT_EQ("Hello, World!", row["foo,bar"]);
}

TEST(Csv, ignoreVaryingRows)
{
    std::stringstream ss;
    ss << "A,B,C,D" << std::endl
       << "1,2"     << std::endl
       << "3,4,5,6" << std::endl;
    
    csv::Reader reader(ss);

    // Ignore line 2
    csv::Row row = reader.read_row();
    ASSERT_TRUE(!row.empty());
    EXPECT_EQ("3", row["A"]);
    EXPECT_EQ("4", row["B"]);
    EXPECT_EQ("5", row["C"]);
    EXPECT_EQ("6", row["D"]);
}

TEST(Csv, iterators)
{
    std::stringstream ss;
    ss << "id	year	length	genre	directors	actors" << std::endl
       << "9bd4cbab-0af9-43c1-a6fe-8440b5f7979a	2015	62	\\N	Stephen Ang	Stephen Ang,George Capacete,Kimberly Cashner,Peggy Glenn" << std::endl
       << "7e0a9d8e-99b7-4a88-b610-3a52faaae2c1	1929	64	Crime,Drama,Mystery	Phil Rosen	Ricardo Cortez,Nancy Welford" << std::endl
       << "6acef12b-404c-4a8f-b365-cef798585893	1993	87	Thriller	Christian González	Leonardo Daniel,Roberto 'Flaco' Guzmán,Armando Silvestre" << std::endl;

    std::vector<std::string> genres;
    const std::vector<std::string> expected_genres {"\\N", "Crime,Drama,Mystery", "Thriller"};

    csv::Reader reader(ss, csv::separator::tab);
    for (csv::Row& row : reader)
        genres.emplace_back(row["genre"]);

    EXPECT_EQ(expected_genres, genres);
}

TEST(Csv, iterateTwice)
{
    std::stringstream ss;
    ss << "A,B,C" << std::endl
       << "1,2,3";

    const std::vector<std::string> expected_row { "1", "2", "3" };

    csv::Reader reader(ss);
    for (csv::Row& row : reader) {
        EXPECT_EQ("1", row["A"]);
        EXPECT_EQ("2", row["B"]);
        EXPECT_EQ("3", row["C"]);
    }

    for (csv::Row& row : reader) {
        EXPECT_EQ("1", row["A"]);
        EXPECT_EQ("2", row["B"]);
        EXPECT_EQ("3", row["C"]);
    }
}