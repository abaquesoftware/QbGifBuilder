include _tdd-header.inc.makefile

include tdd-QbLzwDecoder.makefile
include tdd-QbLzwEncoder.makefile
include tdd-QbStreamTools.makefile

TARGET = QbGifFrame
DEPS_H = QbGifFrame.h QbGifBitmap.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbLzwEncoder.o QbLzwDecoder.o QbLzwDictionary.o  QbLzwDictionaryEntry.o QbStreamTools.o

include _tdd-footer.inc.makefile
