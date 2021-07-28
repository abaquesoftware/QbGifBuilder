#include "QbStreamTools.h"

#include <istream>
#include <ostream>

using namespace std;

void QbStreamTools::writeByte (ostream &stream, __u_char value) {
  const char byte = value;
  stream.write( &byte, 1);
}


int QbStreamTools::readByte (istream &stream) {
  __u_char bytes[1];
  stream.read((char*)bytes, 1);
  return bytes[0];
}

void QbStreamTools::writeShort (ostream &stream, int value) {
  const char byte1 = value & 0xFF;
  const char byte2 = (value & 0xFF00) >> 8;
  stream.write( &byte1, 1);
  stream.write( &byte2, 1);
}

int QbStreamTools::readShort (istream &stream) {
  __u_char bytes[2];
  stream.read((char*)bytes, 2);
  return bytes[0] + bytes[1] * 256;
}

void QbStreamTools::writeString (ostream &stream, const char *value, int valueLen ) {
  stream.write( value, valueLen);
}
