include _tdd-header.inc.makefile

include tdd-QbGifFrame.makefile

TARGET = QbGifBitmap
DEPS_H = QbGifBitmap.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbGifFrame.o QbLzwEncoder.o QbLzwDecoder.o QbLzwDictionary.o  QbLzwDictionaryEntry.o QbStreamTools.o

include _tdd-footer.inc.makefile
