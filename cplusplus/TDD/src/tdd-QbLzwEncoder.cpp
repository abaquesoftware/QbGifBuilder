#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbLzwEncoder.h"

#include "tdd-tools.inc.cpp"

using namespace std;
using namespace Catch::Matchers;

// Note: Tests here use examples from this web page:
// https://en.wikipedia.org/wiki/GIF

TEST_CASE( "convertNbitTo8bitVector", "[QbLzwEncoder]" ) {
  int initialCodeSize = 8;
  vector<const QbLzwDictionaryEntry *> entryList;

  vector<__u_char> res = QbLzwEncoder::convertNbitTo8bitVector( 
                            initialCodeSize, 
                            entryList);
}

TEST_CASE( "encode small byte arrays", "[QbLzwDecoder]" ) {
  int initialCodeSize = 8;

  __u_char decoded1[] = {}; 
  int len1 = 0;
  string encoded1 = "0x00,0x03,0x02";
  vector<__u_char> test1 = QbLzwEncoder::encode(initialCodeSize, decoded1, len1);
  CHECK(convert_uchar_vector_to_string(test1) == encoded1);

  __u_char decoded2[] = {0x12};
  int len2 = 1;
  string encoded2 = "0x00,0x25,0x04,0x04";
  vector<__u_char> test2 = QbLzwEncoder::encode(initialCodeSize, decoded2, len2);
  CHECK(convert_uchar_vector_to_string(test2) == encoded2);

  __u_char decoded3[] = {0x28,0xff,0xff,0xff,0x28,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  int len3 = 15;
  string encoded3 = "0x00,0x51,0xfc,0x1b,0x28,0x70,0xa0,0xc1,0x83,0x01,0x01";
  vector<__u_char> test3 = QbLzwEncoder::encode(initialCodeSize, decoded3, len3);
  CHECK(convert_uchar_vector_to_string(test3) == encoded3);
}

