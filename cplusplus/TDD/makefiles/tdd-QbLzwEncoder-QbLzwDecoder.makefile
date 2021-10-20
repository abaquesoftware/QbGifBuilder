include _tdd-header.inc.makefile

include tdd-QbLzwDictionary.makefile
include tdd-QbLzwDictionaryEntry.makefile
include tdd-QbLzwEncoder.makefile
include tdd-QbLzwDecoder.makefile

TARGET = QbLzwEncoder-QbLzwDecoder
BUILD_TARGET_QbLzwEncoder-QbLzwDecoder = 0
DEPS_H = QbLzwEncoder.h QbLzwDecoder.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbLzwEncoder.o QbLzwDecoder.o QbLzwDictionary.o QbLzwDictionaryEntry.o

include _tdd-footer.inc.makefile
