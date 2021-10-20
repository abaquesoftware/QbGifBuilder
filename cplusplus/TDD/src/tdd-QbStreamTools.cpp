#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbStreamTools.h"

using namespace std;
using namespace Catch::Matchers;

TEST_CASE( "write / read bytes", "[QbStreamToos]" ) {
  stringstream stream;
  QbStreamTools::writeByte(stream, (__u_char)'A');
  QbStreamTools::writeByte(stream, (__u_char)'B');

  stream.seekg(0);
  char output[16];
  streamsize len = stream.readsome(output, 16);
  CHECK(len == 2);
  CHECK(output[0] == 'A');
  CHECK(output[1] == 'B');

  stream.seekg(0);
  __u_char output_0 = QbStreamTools::readByte(stream);
  __u_char output_1 = QbStreamTools::readByte(stream);
  __u_char output_2 = QbStreamTools::readByte(stream);
  CHECK(output_0 == 'A');
  CHECK(output_1 == 'B');
  CHECK(output_2 == 0xFF);
}

TEST_CASE( "write / read short", "[QbStreamToos]" ) {
  stringstream stream;
  QbStreamTools::writeShort(stream, (__u_short)0x0000);
  QbStreamTools::writeShort(stream, (__u_short)0x00AB);
  QbStreamTools::writeShort(stream, (__u_short)0xABCD);

  stream.seekg(0);
  char output[16];
  streamsize len = stream.readsome(output, 16);
  CHECK(len == 6);
  // 0x0000
  CHECK((__u_char)output[0] == 0x00);
  CHECK((__u_char)output[1] == 0x00);
  // 0x00AB
  CHECK((__u_char)output[2] == 0xAB);
  CHECK((__u_char)output[3] == 0x00);
  // 0xABCD
  CHECK((__u_char)output[4] == 0xCD);
  CHECK((__u_char)output[5] == 0xAB);

  stream.seekg(0);
  __u_short output_0 = QbStreamTools::readShort(stream);
  __u_short output_1 = QbStreamTools::readShort(stream);
  __u_short output_2 = QbStreamTools::readShort(stream);
  __u_short output_3 = QbStreamTools::readShort(stream);
  CHECK(output_0 == 0x0000);
  CHECK(output_1 == 0x00AB);
  CHECK(output_2 == 0xABCD);
  CHECK(output_3 == 0xFFFF);
}

TEST_CASE( "write string", "[QbStreamToos]" ) {
  stringstream stream;
  QbStreamTools::writeString(stream, "Abcd", 4);
  stream.seekg(0);
  char output[16];
  streamsize len = stream.readsome(output, 16);
  CHECK(len == 4);
  CHECK(output[0] == 'A');
  CHECK(output[1] == 'b');
  CHECK(output[2] == 'c');
  CHECK(output[3] == 'd');
}
