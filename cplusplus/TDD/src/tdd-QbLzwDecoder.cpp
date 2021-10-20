#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbLzwDecoder.h"

#include "tdd-tools.inc.cpp"

using namespace std;
using namespace Catch::Matchers;

// Note: Tests here use examples from this web page:
// https://en.wikipedia.org/wiki/GIF

TEST_CASE( "processCode", "[QbLzwDecoder]" ) {

  //                            Table           9-bit
  //                      string --> code      code    Action
  //                           #0 | 000h               Initialize root table of 9-bit codes
  //                     palette  |  :
  //                      colors  |  :
  //                         #255 | 0FFh
  //                          clr | 100h
  //                          end | 101h
  //                              |            100h     Clear
  // Pixel          Local         |
  // color Palette  string        |
  // BLACK  #40     28            |            028h     1st pixel always to output
  // WHITE  #255    FF            |                     String found in table
  //                   28 FF      | 102h                Always add 1st string to table
  //                FF            |                     Initialize local string
  // WHITE  #255    FF FF         |                     String not found in table
  //                              |            0FFh     - output code for previous string
  //                   FF FF      | 103h                - add latest string to table
  //                              |            103h     - output code for previous string
  //
  // -> array to decode : 0x100,0x28,0xFF,0x103

  int initialCodeWidth = 8;
  int codeWidth = 9;
  QbLzwDictionary dictionary(initialCodeWidth);
  vector<__u_char> result;

  // --- Decode 0x100

  __u_short currentCode1 = 0x100;
  const QbLzwDictionaryEntry * localCode1 = NULL;
  tuple<const QbLzwDictionaryEntry *, int> res1 = 
     QbLzwDecoder::processCode(currentCode1,
                               dictionary,
                               localCode1,
                               codeWidth,
                               result);
  CHECK(get<0>(res1) == NULL );
  CHECK(get<1>(res1) == codeWidth ); // Check new code width
  CHECK(convert_uchar_vector_to_string(result) == "");

  // --- Decode 0x28

  __u_short currentCode2 = 0x28;
  const QbLzwDictionaryEntry *localCode2 = get<0>(res1);

  tuple<const QbLzwDictionaryEntry *, int> res2 = 
     QbLzwDecoder::processCode(currentCode2,
                               dictionary,
                               localCode2,
                               codeWidth,
                               result);
  CHECK(get<0>(res2)->toString() == "[0x0028]->(1):0x28");
  CHECK(get<1>(res2) == codeWidth ); // Check new code width
  CHECK(convert_uchar_vector_to_string(result) == "0x28");

  // --- Decode 0xff

  __u_short currentCode3 = 0xFF;
  const QbLzwDictionaryEntry *localCode3 = get<0>(res2);

  tuple<const QbLzwDictionaryEntry *, int> res3 = 
     QbLzwDecoder::processCode(currentCode3,
                               dictionary,
                               localCode3,
                               codeWidth,
                               result);
  CHECK(get<0>(res3)->toString() == "[0x00ff]->(1):0xff");
  CHECK(get<1>(res3) == codeWidth ); // Check new code width
  CHECK(convert_uchar_vector_to_string(result) == "0x28,0xff");

  // --- Decode 0x103

__u_short currentCode4 = 0x103;
  const QbLzwDictionaryEntry *localCode4 = get<0>(res3);

  tuple<const QbLzwDictionaryEntry *, int> res4 = 
     QbLzwDecoder::processCode(currentCode4,
                               dictionary,
                               localCode4,
                               codeWidth,
                               result);
  CHECK(get<0>(res4)->toString() == "[0x0103]->(2):0xff.0xff");
  CHECK(get<1>(res4) == codeWidth ); // Check new code width
  CHECK(convert_uchar_vector_to_string(result) == "0x28,0xff,0xff,0xff");

}

TEST_CASE( "decode small byte arrays", "[QbLzwDecoder]" ) {
  int initialCodeSize = 8;

  __u_char encoded1[] = {0x00,0x03};
  int len1 = 2;
  string decoded1 = "";
  vector<__u_char> test1 = QbLzwDecoder::decode (initialCodeSize, encoded1, len1);
  CHECK(convert_uchar_vector_to_string(test1) == decoded1);

  __u_char encoded2[] = {0x00,0x25,0x04};
  int len2 = 3;
  string decoded2 = "0x12";
  vector<__u_char> test2 = QbLzwDecoder::decode (initialCodeSize, encoded2, len2);
  CHECK(convert_uchar_vector_to_string(test2) == decoded2);

  __u_char encoded3[] = {0x00,0x51,0xfc,0x1b,0x28,0x70,0xa0,0xc1,0x83,0x01,0x01};
  int len3 = 11;
  string decoded3 = "0x28,0xff,0xff,0xff,0x28,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff";
  vector<__u_char> test3 = QbLzwDecoder::decode (initialCodeSize, encoded3, len3);
  CHECK(convert_uchar_vector_to_string(test3) == decoded3);
}
