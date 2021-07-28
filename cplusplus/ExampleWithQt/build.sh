#!/bin/bash

# ---- Compile
COMPILER="g++"

COMPILER_OPTIONS=""
COMPILER_OPTIONS+=" -c"
COMPILER_OPTIONS+=" -pipe"
COMPILER_OPTIONS+=" -O2"
COMPILER_OPTIONS+=" -W"
COMPILER_OPTIONS+=" -D_REENTRANT"
COMPILER_OPTIONS+=" -fPIC"
COMPILER_OPTIONS+=" -DQT_DEPRECATED_WARNINGS"
COMPILER_OPTIONS+=" -DQT_NO_DEBUG"
COMPILER_OPTIONS+=" -DQT_GUI_LIB"
COMPILER_OPTIONS+=" -DQT_CORE_LIB"

COMPILER_INC_PATH=""
COMPILER_INC_PATH+=" -I."
# Qt
COMPILER_INC_PATH+=" -I../QbGifBuilder/include"
# QbGif
COMPILER_INC_PATH+=" -isystem /usr/include/x86_64-linux-gnu/qt5"
COMPILER_INC_PATH+=" -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui"
COMPILER_INC_PATH+=" -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore"
COMPILER_INC_PATH+=" -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets"
COMPILER_INC_PATH+=" -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++"

# Qt
COMPILER_SOURCE_LIST="ExampleWithQt.cpp"
# QbGif
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbLzwEncoder.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbLzwDictionary.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbLzwDictionaryEntry.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbGifBitmap.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbGifFrame.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbGifBuilder.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbStreamTools.cpp"
COMPILER_SOURCE_LIST+=" ../QbGifBuilder/src/QbLzwDecoder.cpp"

for SOURCE_FILE in ${COMPILER_SOURCE_LIST}
do
  OBJECT_FILE="$( basename "${SOURCE_FILE}" | cut -d'.' -f1 ).o"
  echo "$OBJECT_FILE"
  "${COMPILER}" ${COMPILER_OPTIONS} ${COMPILER_INC_PATH} -o "${OBJECT_FILE}" "${SOURCE_FILE}"
done

# ---- Link

LINKER="g++"

LINKER_OPTIONS="-Wl,-O1"

LINKER_LIB_LIST=""
LINKER_LIB_LIST+=" -lpthread"

LINKER_OBJ_LIST=""
LINKER_OBJ_LIST+=" ExampleWithQt.o"
# Qt
LINKER_OBJ_LIST+=" /usr/lib/x86_64-linux-gnu/libQt5Gui.so"
LINKER_OBJ_LIST+=" /usr/lib/x86_64-linux-gnu/libQt5Core.so"
LINKER_OBJ_LIST+=" /usr/lib/x86_64-linux-gnu/libGL.so"
LINKER_OBJ_LIST+=" /usr/lib/x86_64-linux-gnu/libQt5Widgets.so"
# QbGifBuilder
LINKER_OBJ_LIST+=" QbLzwEncoder.o"
LINKER_OBJ_LIST+=" QbLzwDictionary.o"
LINKER_OBJ_LIST+=" QbLzwDictionaryEntry.o"
LINKER_OBJ_LIST+=" QbGifBitmap.o"
LINKER_OBJ_LIST+=" QbGifFrame.o"
LINKER_OBJ_LIST+=" QbGifBuilder.o"
LINKER_OBJ_LIST+=" QbStreamTools.o"
LINKER_OBJ_LIST+=" QbLzwDecoder.o"


LINKER_OUTPUT="-o ExampleWithQt.x"

"${LINKER}" ${LINKER_OPTIONS} ${LINKER_LIB_LIST} ${LINKER_OUTPUT} ${LINKER_OBJ_LIST}

# ------ Clean-up

rm -f *.o
