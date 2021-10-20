#include <catch/catch.hpp>
#include "QbGifBuilder/include/QbLzwDictionaryEntry.h"

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("static int_to_hex", "[QbLzwDictionaryEntry]") {
  CHECK(QbLzwDictionaryEntry::int_to_hex(1, 2) == "0x01");
  CHECK(QbLzwDictionaryEntry::int_to_hex(2, 2) == "0x02");
  CHECK(QbLzwDictionaryEntry::int_to_hex(10, 2) == "0x0a");
  CHECK(QbLzwDictionaryEntry::int_to_hex(20, 2) == "0x14");
  CHECK(QbLzwDictionaryEntry::int_to_hex(260, 2) == "0x104");
  CHECK(QbLzwDictionaryEntry::int_to_hex(1, 4) == "0x0001");
  CHECK(QbLzwDictionaryEntry::int_to_hex(20, 4) == "0x0014");
  CHECK(QbLzwDictionaryEntry::int_to_hex(260, 4) == "0x0104");
}

TEST_CASE("constructor from byte array", "[QbLzwDictionaryEntry]") {

  __u_char test1_byte_array[] = {};
  QbLzwDictionaryEntry test1(test1_byte_array, 0, 1);
  CHECK(test1.toString() == "[0x0001]->(0):");

  __u_char test2_byte_array[] = {1,2,3};
  QbLzwDictionaryEntry test2(test2_byte_array, 3, 8);
  CHECK(test2.toString() == "[0x0008]->(3):0x01.0x02.0x03");
}

TEST_CASE("constructor by copy", "[QbLzwDictionaryEntry]") {

  __u_char test_byte_array[] = {1,2,3};
  QbLzwDictionaryEntry test1(test_byte_array, 3, 8);
  QbLzwDictionaryEntry test2(test1);
  CHECK(test2.toString() == "[0x0008]->(3):0x01.0x02.0x03");
}

TEST_CASE("constructor with type = CODE_WIDTH_CHANGE", "[QbLzwDictionaryEntry]" ) {

  QbLzwDictionaryEntry test;
  CHECK(test.toString() == "---- code width change");
}

TEST_CASE("operator <", "[QbLzwDictionaryEntry]" ) {

  __u_char test1_byte_array[] = {1,2,3};
  __u_char test2_byte_array[] = {1,2,3,4};
  __u_char test3_byte_array[] = {1,2};
  __u_char test4_byte_array[] = {1,2,4};
  __u_char test5_byte_array[] = {1,2,2};
  __u_char test6_byte_array[] = {1,2,3};
  QbLzwDictionaryEntry test1(test1_byte_array, 3, 8);
  QbLzwDictionaryEntry test2(test2_byte_array, 4, 8);
  QbLzwDictionaryEntry test3(test3_byte_array, 2, 8);
  QbLzwDictionaryEntry test4(test4_byte_array, 3, 8);
  QbLzwDictionaryEntry test5(test5_byte_array, 3, 8);
  QbLzwDictionaryEntry test6(test6_byte_array, 3, 8);
  CHECK( (test1 < test2) == true );
  CHECK( (test1 < test3) == false );
  CHECK( (test1 < test4) == true );
  CHECK( (test1 < test5) == false );
  CHECK( (test1 < test6) == false );
}