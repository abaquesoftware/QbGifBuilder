#!/bin/bash

SRC_LIST="dataBuilder.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbLzwEncoder.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbLzwDictionary.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbLzwDictionaryEntry.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbGifBitmap.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbGifFrame.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbGifBuilder.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbStreamTools.cpp"
SRC_LIST+=" ../../QbGifBuilder/src/QbLzwDecoder.cpp"


#gcc -I../include -lstdc++ -o Sample.x ${SRC_LIST}
gcc ${SRC_LIST} -I../../QbGifBuilder/include -lstdc++ -lm -o ../bin/dataBuilder.x 
