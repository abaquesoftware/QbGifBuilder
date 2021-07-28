#ifndef _QBSTREAMTOOLS_H_
#define _QBSTREAMTOOLS_H_

#include <sys/types.h>
#include <iostream>

using namespace std;

class QbStreamTools {
  public:
    static void writeByte (ostream &stream, __u_char value);
    static int readByte (istream &stream);

    static void writeShort (ostream &stream, int value);
    static int readShort (istream &stream);

    static void writeString (ostream &stream, const char *value, int valueLen);
};

#endif // _QBSTREAMTOOLS_H_
