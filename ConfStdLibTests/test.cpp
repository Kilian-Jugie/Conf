#include "pch.h"
#include <confstdparse.hpp>
#include <confstdglobal.hpp>
#include <string>
#include <vector>

TEST(ConfParseSplit, All) {
  confstd::string_t tosplit{"[&val](std::string ev) { return val(env); }\n"};
  std::vector<confstd::string_t> splitted = confstd::split(tosplit, " ^[^]:\n", '^');
  EXPECT_EQ(splitted.size(), 10);
  EXPECT_EQ(splitted[0], "[");
  EXPECT_EQ(splitted[1], "&val");
  EXPECT_EQ(splitted[2], "]");
  EXPECT_EQ(splitted[3], "(std");
  EXPECT_EQ(splitted[4], "string");
  EXPECT_EQ(splitted[5], "ev)");
  EXPECT_EQ(splitted[6], "{");
  EXPECT_EQ(splitted[7], "return");
  EXPECT_EQ(splitted[8], "val(env);");
  EXPECT_EQ(splitted[9], "}");
}

TEST(ConfParseItSplit, All) {
  confstd::string_t str{"This is a space separated string"};
  auto s = confstd::it_split(str, [](const confstd::string_t::const_iterator& ch) {
    return *ch == ' ';
    });
  EXPECT_EQ(s.size(), 6);
  EXPECT_EQ(std::get<0>(s[0]), str.begin());
  EXPECT_EQ(std::get<1>(s[0]), str.begin()+4);
  EXPECT_EQ(std::get<0>(s[1]), str.begin()+5);
  EXPECT_EQ(std::get<1>(s[1]), str.begin()+7);
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[0]), std::get<1>(s[0])), "This");
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[1]), std::get<1>(s[1])), "is");
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[2]), std::get<1>(s[2])), "a");
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[3]), std::get<1>(s[3])), "space");
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[4]), std::get<1>(s[4])), "separated");
  EXPECT_EQ(confstd::string_view_t(std::get<0>(s[5]), std::get<1>(s[5])), "string");
}

TEST(ConfParseItSplitAs, All) {
  confstd::string_t str{"This is a space separated string"};
  auto s = confstd::it_split_as<confstd::string_view_t>(str, [](const confstd::string_t::const_iterator& ch) {
    return *ch == ' ';
    });
  EXPECT_EQ(s.size(), 6);
  EXPECT_EQ(s[0], "This");
  EXPECT_EQ(s[1], "is");
  EXPECT_EQ(s[2], "a");
  EXPECT_EQ(s[3], "space");
  EXPECT_EQ(s[4], "separated");
  EXPECT_EQ(s[5], "string");
}

TEST(ConfParseStrReplace, NoFunc) {
  confstd::string_t str{"This string contains string but no strings"};
  confstd::strreplace(str, "string", "text");
  EXPECT_EQ(str, "This text contains text but no texts");
}

TEST(ConfParseStrReplace, Func) {
  confstd::string_t str{"This string contains string but no strings"};
  confstd::strreplace(str, "string", "text", [&str](std::size_t pos) {
    pos += 6;
    return pos >= str.size() || !isalnum(str[pos]) || isspace(str[pos]);
    });
  EXPECT_EQ(str, "This text contains text but no strings");
}

TEST(ConfParseTrim, Front) {
  confstd::string_t packed("hello");
  confstd::string_t unpacked("  \t hello");
  confstd::trim(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParseTrim, Back) {
  confstd::string_t packed("hello");
  confstd::string_t unpacked("hello      \t\t\t");
  confstd::trim(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParseTrim, Not) {
  confstd::string_t packed("hello");
  confstd::string_t unpacked("hello");
  confstd::trim(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParseTrim, FrontBack) {
  confstd::string_t packed("hello");
  confstd::string_t unpacked("  \t hello      \t\t\t");
  confstd::trim(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParsePress, All) {
  confstd::string_t packed("this is a text with spaces");
  confstd::string_t unpacked("this  \t\t      is a text    with\tspaces");
  confstd::press(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParsePress, Not) {
  confstd::string_t packed("this is a text with spaces");
  confstd::string_t unpacked("this is a text with spaces");
  confstd::press(unpacked);
  EXPECT_EQ(packed, unpacked);
}

TEST(ConfParseLength, All) {
  constexpr confstd::char_t atext[] = "this is a text";
  EXPECT_EQ(confstd::length(atext), sizeof(atext) / sizeof(confstd::char_t));
}

#define CONF_TEST_VER 1,2,3

TEST(ConfVerLib, All) {
  EXPECT_EQ(CONF_STD_VERLIB_INT(CONF_TEST_VER), 123);
  EXPECT_STREQ(CONF_STD_VERLIB_STR(CONF_TEST_VER), "1.2.3");
}