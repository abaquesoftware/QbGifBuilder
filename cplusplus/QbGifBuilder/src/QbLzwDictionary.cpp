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
  // return this->entryMapByCode.size();
  return this->nextEntryCode;
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
  for( int i = 0 ;i < ( 1 << initialCodeWidth); i++) {
    u_char byte = i;
    this->addEntry( &byte, 1 );
  }
  // Add 0x100 and 0x101
  byte = 0;
  this->addRawEntry(&byte, 1, false);
  this->addRawEntry(&byte, 1, false);
  
}

__u_short QbLzwDictionary::addEntry(__u_char * bytes, long len) {
  __u_short entryCode = ERROR_CODE;
  if (this->getEntryMapSize() < MAX_NB_ENTRIES) {   
    entryCode = this->addRawEntry(bytes, len, true);
  }
  return entryCode;
}

__u_short QbLzwDictionary::addEntry(const QbLzwDictionaryEntry * entry, __u_char lastByte) {  
  __u_short entryCode = ERROR_CODE;
  if (this->getEntryMapSize() < MAX_NB_ENTRIES) {
    __u_char bytes[entry->dataLength + 1];
    for (int i = 0; i < entry->dataLength; i++ ) {
      bytes[i] = entry->data[i];
    }
    bytes[entry->dataLength] = lastByte;
    entryCode = this->addRawEntry(bytes, entry->dataLength + 1, true);
  }
  return entryCode;
}

__u_short QbLzwDictionary::addRawEntry( __u_char * entry, long entryLen, bool mapByData) {
    __u_short entryCode = this->nextEntryCode;
    QbLzwDictionaryEntry newEntry(entry, entryLen, entryCode);
    if(entryLen > this->maxEntrySize) {
      this->maxEntrySize = entryLen;    
    }     
    if ( mapByData ) {
      this->entryMapByData.insert( pair<QbLzwDictionaryEntry, __u_short>(newEntry, entryCode) );
    }
    this->entryMapByCode.insert( pair<__u_short, QbLzwDictionaryEntry>(entryCode, newEntry) );
    this->nextEntryCode++;
    return entryCode;
 }
