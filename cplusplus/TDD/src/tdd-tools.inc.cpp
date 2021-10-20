#ifndef __TD_TOOLS_INC_CPP__
#define __TD_TOOLS_INC_CPP__

#include <sys/types.h>
#include <vector>
#include <bits/stdc++.h>
#include <sys/stat.h>

#include "QbGifBuilder/include/QbLzwDictionaryEntry.h"

string convert_uchar_vector_to_string(vector<__u_char> v) {  
  string result = "";
  std::for_each (v.begin(), v.end(), [&result](__u_char &c) {
    if (result != "") {
      result += ",";
    }
    result += QbLzwDictionaryEntry::int_to_hex(c,2);
  } );
  return result;
}

bool are_files_identical(const char * fileName1, const char * fileName2) {
  bool result = true;
  struct stat file1_buffer;
  struct stat file2_buffer;
  int file1_exists = !stat(fileName1, &file1_buffer);
  int file2_exists = !stat(fileName2, &file2_buffer);
  if (!file1_exists || !file2_exists) {
    result = false;
  }
  off_t file1_size;
  off_t file2_size;
  if (result) {
    file1_size = file1_buffer.st_size;
    file2_size = file2_buffer.st_size;
    if (file1_size != file2_size) {
      result = false;
    }
  }

  if (result) {
    ifstream file1 = ifstream(fileName1, std::ios_base::binary);
    ifstream file2 = ifstream(fileName2, std::ios_base::binary);
    if( !file1.good() || !file2.good() ) {
      result = false;
    }
    char byte1;
    char byte2;
    if (result) {
      for (off_t i = 0; result && i < file1_size; i++) {
        file1.read(&byte1, 1);
        file2.read(&byte2, 1);
        if (byte1 != byte2) {
          result = false;
        }
      }
    }  
    file1.close();
    file2.close();
  }
  return result;
}

#endif