#include "QbLzwDecoder.h"

#include <stdlib.h>
#include <tuple>

#include "../include/QbLzwDictionary.h"

// #define DEBUG_MODE

using namespace std;

vector<__u_char> QbLzwDecoder::decode (int initialCodeWidth, __u_char * bytes, const size_t size) {
  /*
  // Debug
  std::cout << "...........................................\n";  
  std::cout << "Array to decode\n";
  std::cout << "...........................................\n";
  for(size_t i = 0; i < size; i++) {
    if (i > 0) std::cout << ".";
    std::cout << QbLzwDictionaryEntry::int_to_hex( bytes[i], 2);
  }
  std::cout << "\n";
  */

#ifdef DEBUG_MODE
  // Debug
  std::cout << "...........................................\n";
  std::cout << "DECODER - List of Dic.entries (after 8bits->Nbits conv.)\n";
  std::cout << "Array to decode\n";
  std::cout << "...........................................\n";
#endif

  // Main loop
  vector<__u_char> result;
  QbLzwDictionary dictionary(initialCodeWidth);
  const QbLzwDictionaryEntry * localCode = NULL;
  int n = initialCodeWidth + 1;

  __u_short output = 0L;
  int outputPos = 0;
  bool stop_output = false;
  for (size_t inputIndex = 0; !stop_output && inputIndex < size; inputIndex++ ) {    
    __u_char input = bytes[inputIndex];
    // cout << "Read input = " << QbLzwDictionaryEntry::int_to_hex(input,2) << "\n";        
    for (int inputPos = 0; !stop_output && inputPos < 8; inputPos++) {
      int bit = (input & (1 << inputPos) );
      if( bit ) {
        output |= ( 1 << outputPos );
      }
      outputPos++;
      if (outputPos == n) {
        // cout << " o  processCode " << QbLzwDictionaryEntry::int_to_hex(output,4) << "\n"; 
        int oldN = n;

#ifdef DEBUG_MODE
        // Debug
        int code = -1;
        if(localCode) code = localCode->code;
        printf("%02x, - localCode = %d - ", output, code);
#endif
        tuple<const QbLzwDictionaryEntry *, int> ret = QbLzwDecoder::processCode(output , dictionary, localCode, n, result);
        if ( output == (1 << initialCodeWidth) + 1 ) {
          stop_output = true;
        }
        localCode = get<0>(ret);
        n = get<1>(ret);
#ifdef DEBUG_MODE
  // Debug
  string str = "";
  if (localCode) str = localCode->toString();
  printf("output = %02x - localCode = %s - n = %d\n", output, str.c_str(), n);
#endif
        // if( n != oldN) cout << " o    ------ New code width : " << to_string(n) << "\n";
        output = 0L;
        outputPos = 0;
      }
    }
  }

  /*
  // Debug
  std::cout << "\n";
  std::cout << "...........................................\n";
  std::cout << "Decoded - Size = " << to_string(result.size()) << "\n";
  std::cout << "...........................................\n";
  for (size_t i = 0; i < result.size(); i++ ) {
    std::cout << QbLzwDictionaryEntry::int_to_hex( result[i], 2) << ".";
  }
  std::cout << "\n";
  */

  return result;
}

// Return value = < new localCode, new codeWidth >
tuple<const QbLzwDictionaryEntry *, int> QbLzwDecoder::processCode(__u_short currentCode, 
                                                                   QbLzwDictionary &dictionary,
                                                                   const QbLzwDictionaryEntry *localCode,  
                                                                   int codeWidth,
                                                                   vector<__u_char> &result) {
 
  // result
  const QbLzwDictionaryEntry *resultLocalCode = NULL;
  int resultCodeWidth = codeWidth;

  const int startCode = 1 << (dictionary.getInitialCodeWidth());
  const int stopCode = startCode + 1;
   
  // cout << "currentCode = " << QbLzwDictionaryEntry::int_to_hex( currentCode, 2) << " - startCode = " << QbLzwDictionaryEntry::int_to_hex( startCode, 2) << "\n";
  if (currentCode == startCode ) {
    localCode = NULL;
    // clear dictionnary entryMap
    dictionary.clearAll();
    resultCodeWidth = dictionary.getInitialCodeWidth() + 1;
  }
  if (currentCode == stopCode ) {
      // Do nothing
      // cout << "Stop found !";
  }
  if (currentCode != startCode && currentCode != stopCode ) {
    const QbLzwDictionaryEntry * entry = dictionary.getEntryByCode(currentCode);
    if (entry) {
      // Entry found
      // cout << "                    Entry found.\n";
      if (localCode) {
        __u_short newEntryCode = dictionary.addEntry(localCode, entry->data[0]);
        //printf("oooooo : newEntryCode = %02x\n", newEntryCode);
        if (newEntryCode != 0xFFFF) {

          // Debug
          // const QbLzwDictionaryEntry * debugEntry = dictionary.getEntryByCode(newEntryCode);
          // cout << "                    Add new Entry - code = " << debugEntry->toString() << "\n";          
          if ( newEntryCode == (1 << codeWidth) - 1 ) {
            /*
            printf("oooooo : xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
            printf("oooooo : newEntryCode = %02x\n", newEntryCode);
            printf("oooooo : codeWidth = %d\n", codeWidth );
            printf("oooooo : max = %02x\n", ((1 << codeWidth) - 1) );
            printf("oooooo : QbLzwDecoder::processCode - resultCodeWidth++\n");
            printf("oooooo : xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
            */
            if( resultCodeWidth < 12 ) resultCodeWidth++;
          }
        }
      }
    } else {
      // cout << "                    Entry not found.\n";
      if (localCode) {
        // Entry not found
        __u_short newEntryCode = dictionary.addEntry(localCode, localCode->data[0]);
        if (newEntryCode != 0xFFFF) {
          // Debug
          // const QbLzwDictionaryEntry * debugEntry = dictionary.getEntryByCode(newEntryCode);
          // cout << "                    Add new Entry - code = " << debugEntry->toString() << "\n";
          if ( newEntryCode == ( (1 << codeWidth) - 1) ) {
            if( resultCodeWidth < 12 ) resultCodeWidth++;
          }
          // cout << "                    Entry not found - new EntryCode  = " << QbLzwDictionaryEntry::int_to_hex(newEntryCode, 2) << "\n";
          entry = dictionary.getEntryByCode(newEntryCode);
        } else {
          // cout << " ****************** ERROR - cannot convert code : " <<  QbLzwDictionaryEntry::int_to_hex(newEntryCode, 2) << "\n";
        }
 
      }
    }
    // cout << "                    Add ";
    for (int i = 0; i < entry->dataLength; i++) {
      // cout << QbLzwDictionaryEntry::int_to_hex( entry->data[i], 2) << ".";
      result.push_back(entry->data[i]);
    }
    // cout << "\n";
    resultLocalCode = entry;
  }

  // Debug
  string resultLocalCodeAsStr = "";
  if (resultLocalCode) resultLocalCodeAsStr = resultLocalCode->toString();

  return make_tuple(resultLocalCode, resultCodeWidth);
}
