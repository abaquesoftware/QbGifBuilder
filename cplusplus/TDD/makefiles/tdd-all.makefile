include _tdd-header.inc.makefile

include tdd-QbGifBuilder.makefile

TARGET = all
BUILD_TARGET_all = 0
DEPS_H = 
DEPS_OBJ = 
DEPS_TDD_OBJ = QbGifBuilder.o QbGifBitmap.o QbGifFrame.o QbLzwEncoder.o QbLzwDecoder.o QbLzwDictionary.o  QbLzwDictionaryEntry.o QbStreamTools.o

include _tdd-footer.inc.makefile
