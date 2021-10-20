#ifndef _QBLZWDECODER_H_
#define _QBLZWDECODER_H_

#include <sys/types.h>
#include <vector>

#include "QbLzwDictionary.h"
#include "QbLzwDictionaryEntry.h"

using namespace std;

class QbLzwDecoder {
  public: 
    static vector<__u_char> decode (int initialCodeSize, __u_char * bytes, const size_t len);

#ifndef TDD_MODE
  private:
#endif
     static tuple<const QbLzwDictionaryEntry *, int> processCode(__u_short currentCode, 
                                                                 QbLzwDictionary &dictionary,
                                                                 const QbLzwDictionaryEntry *localCode,  
                                                                 int codeWidth,
                                                                 vector<__u_char> &result);
};

#endif // _QBLZWDECODER_H_
