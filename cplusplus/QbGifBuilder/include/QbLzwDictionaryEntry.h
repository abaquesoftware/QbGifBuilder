#ifndef _QBLZWDICTINARYENTRY_H_
#define _QBLZWDICTINARYENTRY_H_

#include <sys/types.h>
#include <iostream> // for debug only

using namespace std;


class QbLzwDictionaryEntry {

  public :
    enum EntryType { UNKNOWN, CODE, CODE_WIDTH_CHANGE };

  public:
    EntryType type;
    // CODEs
    int dataLength;
    __u_char * data;
    __u_short   code;

  // CTOR
  public:

    // type = CODE
    QbLzwDictionaryEntry( const QbLzwDictionaryEntry &other);
    QbLzwDictionaryEntry( __u_char * bytes, long len, __u_short code);

    // type = CODE_WIDTH_CHANGE
    QbLzwDictionaryEntry();

    ~QbLzwDictionaryEntry();
    bool operator <(const QbLzwDictionaryEntry &other) const;

    // For debug and TDD (used by TDD scripts)
    string toString() const;

    static string int_to_hex( int i, int size );
};

#endif // _QBLZWDICTINARYENTRY_H_
