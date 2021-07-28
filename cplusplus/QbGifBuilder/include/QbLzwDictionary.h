#ifndef _QBLZWDICTINARY_H_
#define _QBLZWDICTINARY_H_

#include <sys/types.h>
#include <vector>
#include <map>

#include "./QbLzwDictionaryEntry.h"

using namespace std;

class QbLzwDictionary {

  private :
    map<QbLzwDictionaryEntry, __u_short> entryMapByData;
    map<__u_short, QbLzwDictionaryEntry> entryMapByCode;
    __u_short nextEntryCode;
    int maxEntrySize;
    int initialCodeWidth;
    QbLzwDictionaryEntry codeWidthChangeEntry;

  // CTOR
  public:
    QbLzwDictionary (int initialCodeWidth);

    int getInitialCodeWidth ();
    int getMaxEntrySize ();
    int getEntryMapSize ();
    QbLzwDictionaryEntry * getCodeWidthChangeEntry();

    void clearAll();
    const QbLzwDictionaryEntry * getEntryByData (__u_char * decoded, long pos, int len);
    const QbLzwDictionaryEntry * getEntryByCode (__u_short code);
    __u_short addEntry ( __u_char * entry, long entryLen );
    __u_short addEntry ( const QbLzwDictionaryEntry *, __u_char lastByte );
    void addCodeWidthChangeEntry(int newCodeWidth);
};

#endif // _QBLZWDICTINARY_H_
