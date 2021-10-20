include _tdd-header.inc.makefile

include tdd-QbGifBitmap.makefile
include tdd-QbGifFrame.makefile

TARGET = QbGifBuilder
DEPS_H = QbGifBuilder.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbGifBitmap.o QbGifFrame.o QbLzwEncoder.o QbLzwDecoder.o QbLzwDictionary.o  QbLzwDictionaryEntry.o QbStreamTools.o

include _tdd-footer.inc.makefile
