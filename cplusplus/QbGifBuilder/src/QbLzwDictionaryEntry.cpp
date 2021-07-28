#include "QbLzwDictionary.h"
#include <iostream> // for debug only
#include <iomanip>

using namespace std;

QbLzwDictionaryEntry::QbLzwDictionaryEntry( const QbLzwDictionaryEntry &other) {
  this->type = CODE;
  int len = other.dataLength;
  this->dataLength = len;
  if (len > 0) {
    this->data = (__u_char *)malloc(len);
  } else {
    this->data = NULL;
  }
  for (int i = 0; i < len; i++) {
    this->data[i] = other.data[i];
  }
  this->code = other.code;
}

QbLzwDictionaryEntry::QbLzwDictionaryEntry( __u_char * bytes, long len, __u_short code)  {
  this->type = CODE;
  this->dataLength = len;
  if (len > 0) {
    this->data = (__u_char *)malloc(len);
  } else {
    this->data = NULL;
  }
  for (int i = 0; i < len; i++) {
    this->data[i] = bytes[i];
  }
  this->code = code;
}

QbLzwDictionaryEntry::QbLzwDictionaryEntry()  {
  this->type = CODE_WIDTH_CHANGE;
  this->dataLength = 0;
  this->data = NULL;
  this->code = 0xFFFF;
}

QbLzwDictionaryEntry::~QbLzwDictionaryEntry() {
  if (this->data)
    free(this->data);
}

bool QbLzwDictionaryEntry::operator <(const QbLzwDictionaryEntry &other) const {
  bool result = 0;
  if( this->dataLength < other.dataLength ) {
    result = 1;
  }
  if( this->dataLength > other.dataLength ) {
    result = 0;
  }
  if( this->dataLength == other.dataLength ) {
    bool exit = 0;
    for (int i = 0; i < this->dataLength && !exit; i++) {
      if( this->data[i] < other.data[i]) {
        result = 1;
        exit = 1;
      }
      if( this->data[i] > other.data[i]) {
        result = 0;
        exit = 1;
      }
    }
  }
  return result;
}

string QbLzwDictionaryEntry::toString() const {
  string result = "";
  switch (this->type) {
    case CODE: {
      result += "[" + int_to_hex(this->code, 4) + "]->";
      result += "(" + std::to_string(this->dataLength) + "):";
      for( int i = 0; i < this->dataLength; i++) {
        if( i > 0 ) result += ".";
        result += int_to_hex(this->data[i], 2);
      }
      break;
    case CODE_WIDTH_CHANGE: {
      result += "---- code width change";
      }
      break;
    }
  }
  return result;
}

// Convert int to hexa string
string QbLzwDictionaryEntry::int_to_hex( int value, int size ) {
  stringstream stream; 
  stream << "0x" << setfill ('0') << setw(size)  << hex << value;
  return stream.str();
}
