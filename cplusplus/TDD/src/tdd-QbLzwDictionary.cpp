#include <catch/catch.hpp>
#include "QbGifBuilder/include/QbLzwDecoder.h"

using namespace std;
using namespace Catch::Matchers;

TEST_CASE( "Constructor and getters", "[QbLzwDictionary]" ) {

  QbLzwDictionary test1(8);
  QbLzwDictionary test2(7);
  CHECK(test1.getInitialCodeWidth() == 8);
  CHECK(test1.getEntryMapSize() == (1 << 8) + 2);
  CHECK(test1.getMaxEntrySize() == 1);
  CHECK(test1.getCodeWidthChangeEntry()->toString() == "---- code width change");
  // KO !!!  
  // CHECK( test1.getEntryByData (NULL, 0, 1) == NULL );
  __u_char bytes[] = {0x00, 0x01, 0xcd, 0xff};
  CHECK(test1.getEntryByCode(0x01)->toString() == "[0x0001]->(1):0x01");
  CHECK(test1.getEntryByCode(0xab)->toString() == "[0x00ab]->(1):0xab");
  CHECK(test1.getEntryByCode(0xff)->toString() == "[0x00ff]->(1):0xff");
  CHECK(test1.getEntryByCode(0x100)->toString() == "[0x0100]->(1):0x00");
  CHECK(test1.getEntryByCode(0x101)->toString() == "[0x0101]->(1):0x00");

  CHECK( test1.getEntryByData(bytes, 0, 1)->toString() == "[0x0000]->(1):0x00");
  CHECK( test1.getEntryByData(bytes, 1, 1)->toString() == "[0x0001]->(1):0x01");
  CHECK( test1.getEntryByData(bytes, 2, 1)->toString() == "[0x00cd]->(1):0xcd");
  CHECK( test1.getEntryByData(bytes, 3, 1)->toString() == "[0x00ff]->(1):0xff");
  CHECK( test1.getEntryByData(bytes, 0, 2) == NULL );

  CHECK( test2.getInitialCodeWidth() == 7 );
  CHECK( test2.getEntryMapSize() == (1 << 7) + 2 );
  CHECK( test2.getEntryByCode(1)->toString() == "[0x0001]->(1):0x01");
}

TEST_CASE( "Add entry from byte array", "[QbLzwDictionary]" ) {
  QbLzwDictionary test(8);
  __u_char bytes[] = {0x00, 0x01, 0x02};
  u_short newCode = test.addEntry(bytes, 2);
  CHECK( newCode == ((1 << 8) + 2 ) );
  CHECK( test.getEntryByCode(0x0102)->toString() == "[0x0102]->(2):0x00.0x01");
  CHECK( test.getEntryByData(bytes, 0, 2)->toString() == "[0x0102]->(2):0x00.0x01");
  CHECK( test.getEntryMapSize() == (1 << 8) + 2 + 1 );
  CHECK( test.getMaxEntrySize() == 2 );

  newCode = test.addEntry(bytes, 3);
  CHECK( newCode == ((1 << 8) + 2 + 1 ) );
  CHECK( test.getEntryByCode(0x0103)->toString() == "[0x0103]->(3):0x00.0x01.0x02");
  CHECK( test.getEntryByData(bytes, 0, 3)->toString() == "[0x0103]->(3):0x00.0x01.0x02");
  CHECK( test.getEntryMapSize() == (1 << 8) + 2 + 2 );
  CHECK( test.getMaxEntrySize() == 3 );
}

TEST_CASE( "Add entry from QbLzwDictionaryEntry", "[QbLzwDictionary]" ) {
  QbLzwDictionary test(8);
  __u_char bytes[] = {0x00, 0x01, 0x7F};
  QbLzwDictionaryEntry entry(bytes, 2, 8);
  u_short newCode = test.addEntry(&entry, 0x7F);
  CHECK( newCode == ((1 << 8) + 2 ) );
  CHECK( test.getEntryByCode(0x0102)->toString() == "[0x0102]->(3):0x00.0x01.0x7f");
  CHECK( test.getEntryByData(bytes, 0, 3)->toString() == "[0x0102]->(3):0x00.0x01.0x7f");
  CHECK( test.getEntryMapSize() == (1 << 8) + 2 + 1 );
  CHECK( test.getMaxEntrySize() == 3 );
}


TEST_CASE( "clearAll", "[QbLzwDictionary]" ) {
  QbLzwDictionary test(8);
  __u_char *bytes = (__u_char *)malloc(sizeof(__u_char) * 256);
  for (int i = 0; i < 256; i++) {
    bytes[i] = i;
  }
  u_short newCode = test.addEntry(bytes, 2);
  CHECK( test.getInitialCodeWidth() == 8 );
  CHECK( test.getEntryMapSize() == (1 << 8) + 2 + 1 );
  CHECK( test.getMaxEntrySize() == 2 );
  CHECK( newCode == ((1 << 8) + 2 ) );
  test.clearAll();
  CHECK( test.getInitialCodeWidth() == 8 );
  CHECK( test.getEntryMapSize() == (1 << 8) + 2 );
  CHECK( test.getMaxEntrySize() == 1 );
}

TEST_CASE( "Test entry stack overflow", "[QbLzwDictionary]" ) {
  int max_entries = 4094;
  QbLzwDictionary test(8);
  u_short newCode;

  // Add 4093 - 259 entries
  __u_char *bytes = (__u_char *)malloc(sizeof(__u_char) * 256);
  int nb_new_entries = 0;
  int nb_created_entries = 0;
  int nb_error_entries = 0;
  for (int i = 0; i < 230; i++) {
    for (int j = 2; j < 20; j++) {
      if (nb_new_entries < (max_entries - 256 - 2)) {
        newCode = test.addEntry(&(bytes[i]), j);
        if (newCode == 0xFFFF ) {
          nb_error_entries++;
        } else {
          nb_created_entries++;
        }
        nb_new_entries++;
      }
    }
  }
  CHECK( nb_created_entries == (max_entries - 256 - 2) );
  CHECK( nb_error_entries == 0 );

  // Add 1 more entries
  newCode = test.addEntry(&(bytes[254]), 2);
  CHECK( newCode == 0xFFFF );
}

