#include "QbLzwEncoder.h"

#include <stdlib.h>

#include "QbLzwDictionary.h"

// #define DEBUG_MODE

using namespace std;

vector<__u_char> QbLzwEncoder::encode (int initialCodeWidth, __u_char * bytes, const size_t size) {
  /*
  // Debug
  std::cout << "...........................................\n";
  std::cout << "Array to encode\n";
  std::cout << "...........................................\n";
  for(size_t i = 0; i < size; i++) {
    if (i > 0) std::cout << ".";
    std::cout << QbLzwDictionaryEntry::int_to_hex( bytes[i], 2);
  }
  std::cout << "\n";
  std::cout << "...........................................\n";
  */

  QbLzwDictionaryEntry clearEntry( NULL, 0, 1 << initialCodeWidth);
  QbLzwDictionaryEntry stopEntry(  NULL, 0, (1 << initialCodeWidth) + 1);


  // build vector of dictionary items
  vector<const QbLzwDictionaryEntry *> entryList;
  vector<QbLzwDictionary *> dictionaryList;
  QbLzwDictionary * dictionary = new QbLzwDictionary(initialCodeWidth);
  dictionaryList.push_back(dictionary);

  int n = initialCodeWidth + 1;
  entryList.push_back(&clearEntry);
  size_t pos = 0;
  while( (size_t)pos < size ) {
    int entryLen = 0;
    const QbLzwDictionaryEntry * foundEntry = NULL;
    const QbLzwDictionaryEntry * entry = NULL;
    while( (entry || entryLen == 0) && (size_t)(entryLen + pos) < size ) {
      entryLen++;
      entry = dictionary->getEntryByData(bytes, pos, entryLen);
      if( (size_t)(pos + entryLen) >= size ) {
      }
      if (entry ) {
        foundEntry = entry;
      }
    }
    entryList.push_back(foundEntry);
    entryLen = foundEntry->dataLength + 1;
    if( (size_t)(entryLen + pos - 1) <= size ) {
      __u_short newCode = dictionary->addEntry(&bytes[pos], entryLen);      
      if(newCode == ( 1 << n ) ) {
        n++;
        entryList.push_back(dictionary->getCodeWidthChangeEntry());
      }
      if (newCode == 0x0FF0) {
        entryList.push_back(&clearEntry);
        dictionary = new QbLzwDictionary(initialCodeWidth);
        dictionaryList.push_back(dictionary);
        n = initialCodeWidth + 1;
      }
    }
    pos += foundEntry->dataLength;
  }
  entryList.push_back(&stopEntry);
  
#ifdef DEBUG_MODE
  // Debug
  std::cout << "...........................................\n";
  std::cout << "ENCODER - List of dictionary entries\n";
  std::cout << "Code table size = " << std::to_string(dictionary->getEntryMapSize()) << "\n";
  std::cout << "Max entry size = " << std::to_string(dictionary->getMaxEntrySize()) << "\n";
  std::cout << "...........................................\n";
  
  for(size_t i = 0; i < entryList.size(); i++) {
    std::cout << entryList[i]->toString() << "\n";
  }
  std::cout << "...........................................\n";
#endif
   
  vector<__u_char> result = convertNbitTo8bitVector(initialCodeWidth, entryList);
  
  #ifdef DEBUG_MODE
  // Debug
  std::cout << "...........................................\n";
  std::cout << "ENCODER - final output (after Nbit->8bit conv.)\n";
  std::cout << "Result - size =" << result.size() << "\n";
  std::cout << "...........................................\n";
  for(size_t i = 0; i < result.size(); i++) {
    if (i > 0) std::cout << ".";
    std::cout << QbLzwDictionaryEntry::int_to_hex( result[i], 2);
  }
  std::cout << "\n";
  std::cout << "...........................................\n";
  /*
  // Debug
  __u_char *debugBytes = (__u_char *)malloc(result.size());
  for(size_t i = 0; i < result.size(); i++) {
    debugBytes[i] = result[i];
  }
  vector<__u_char> debug = QbLzwDecoder::decode(initialCodeWidth, debugBytes, result.size());
  free(debugBytes);
  */
#endif

  // Clean-up :
  for (QbLzwDictionary *dictionary : dictionaryList) {
    delete(dictionary);
  }
  return result;
}

// Conver n-bit -> N bit
vector<__u_char> QbLzwEncoder::convertNbitTo8bitVector(int initialCodeWidth, vector<const QbLzwDictionaryEntry *> entryList) {
  vector<__u_char> result;
  int clearCode = 1 << initialCodeWidth;
  int codeWidth = initialCodeWidth  + 1;
  bool output_remains = false;
  __u_char output = 0;
  int outputPos = 0;
  for( auto entry : entryList) {
    __u_short input = entry->code;
    switch (entry->type) {
      case QbLzwDictionaryEntry::EntryType::CODE : {
        for( int inputPos = 0; inputPos < codeWidth; inputPos++ ) {
          output_remains = true;
          int bit = (input & (1 << inputPos) );
          if( bit ) {
            output |= ( 1 << outputPos );
          }
          outputPos++;
          if( outputPos == 8 ) {
            result.push_back(output);
            output = 0;
            outputPos = 0;
            output_remains = false;
          }
        }
        break;
      }
      case QbLzwDictionaryEntry::EntryType::CODE_WIDTH_CHANGE : {
        codeWidth++;
        break;
      }
    }

    // Case input = clearCode
    if(input == clearCode) {
      codeWidth = initialCodeWidth  + 1;
    }
  }

  if (output_remains) {
    result.push_back(output);
  }

  return result;
}
