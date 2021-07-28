#include "QbLzwDictionary.h"

#include <iostream> // for debug only
#include <map>

using namespace std;

# define MAX_NB_ENTRIES 4094
# define ERROR_CODE 0xFFFF

// CTOR 
QbLzwDictionary::QbLzwDictionary (int initialCodeWidth) {
  this->initialCodeWidth = initialCodeWidth;
  this->clearAll();
}

const QbLzwDictionaryEntry * QbLzwDictionary::getEntryByData(__u_char * bytes, long pos, int len) {
  const QbLzwDictionaryEntry * result = NULL;
  QbLzwDictionaryEntry testEntry( &bytes[pos], len, 0 );  
  map<QbLzwDictionaryEntry, __u_short>::iterator found = this->entryMapByData.find(testEntry);
  if (found != this->entryMapByData.end()) {
    result = &(found->first);
  }
  return result;
}

const QbLzwDictionaryEntry * QbLzwDictionary::getEntryByCode(__u_short code) {
  const QbLzwDictionaryEntry * result = NULL;
  map<__u_short, QbLzwDictionaryEntry>::iterator found = this->entryMapByCode.find(code);
  if (found != this->entryMapByCode.end()) {
    result = &(found->second);
  }
  return result;
}

int QbLzwDictionary::getInitialCodeWidth () {
  return this->initialCodeWidth;  
}

int QbLzwDictionary::getMaxEntrySize() {
  return this->maxEntrySize;
}

int QbLzwDictionary::getEntryMapSize() {
  // return this->entryMap.size();
  return this->nextEntryCode - 1;
}

QbLzwDictionaryEntry * QbLzwDictionary::getCodeWidthChangeEntry() {
  return &this->codeWidthChangeEntry;
}

void QbLzwDictionary::clearAll() {
  this->entryMapByCode.clear();
  this->entryMapByData.clear();

  this->maxEntrySize = 0;
  // Create the first 256 elements
  this->nextEntryCode = 0;
  u_char byte;
  for( int i = 0 ;i <= ( 1 << initialCodeWidth); i++) {
    u_char byte = i;
    this->addEntry( &byte, 1 );
  }  
  // Skip 0x100 and 0x101
  this->nextEntryCode = ( 1 << initialCodeWidth) + 2; 
}

__u_short QbLzwDictionary::addEntry(__u_char * bytes, long len) {
  __u_short entryCode = ERROR_CODE;
  if (this->entryMapByCode.size() < MAX_NB_ENTRIES) {
    if(len > this->maxEntrySize) {
      this->maxEntrySize = len;    
    }    
    entryCode = this->nextEntryCode++;
    QbLzwDictionaryEntry newEntry(bytes, len, entryCode);
    this->entryMapByData.insert( pair<QbLzwDictionaryEntry, __u_short>(newEntry, entryCode) );
    this->entryMapByCode.insert( pair<__u_short, QbLzwDictionaryEntry>(entryCode, newEntry) );
  }
  return entryCode;
}

__u_short QbLzwDictionary::addEntry(const QbLzwDictionaryEntry * entry, __u_char lastByte) {  
__u_short entryCode = ERROR_CODE;
if (this->entryMapByCode.size() < MAX_NB_ENTRIES) {
    __u_char bytes[entry->dataLength + 1];
    for (int i = 0; i < entry->dataLength; i++ ) {
      bytes[i] = entry->data[i];
    }
    bytes[entry->dataLength] = lastByte;

    entryCode = this->nextEntryCode++;
    QbLzwDictionaryEntry newEntry(bytes, entry->dataLength + 1, entryCode);
    this->entryMapByData.insert( pair<QbLzwDictionaryEntry, __u_short>(newEntry, entryCode) );
    this->entryMapByCode.insert( pair<__u_short, QbLzwDictionaryEntry>(entryCode, newEntry) );
  }
  return entryCode;
}
