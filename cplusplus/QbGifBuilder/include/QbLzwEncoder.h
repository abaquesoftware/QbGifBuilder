#ifndef _QBLZWENCODER_H_
#define _QBLZWENCODER_H_

#include <sys/types.h>
#include <vector>

#include "QbLzwDictionaryEntry.h"

using namespace std;

class QbLzwEncoder {
  public: 
    static vector<__u_char> encode (int initialCodeSize, __u_char * bytes, const size_t iLen);

  private:
     static vector<__u_char> convertNbitTo8bitVector(int initialCodeSize, vector<const QbLzwDictionaryEntry *> entryList);
};

#endif // _QBLZWENCODER_H_
