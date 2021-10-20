include _tdd-header.inc.makefile

include tdd-QbLzwDictionary.makefile
include tdd-QbLzwDictionaryEntry.makefile

TARGET = QbLzwDecoder
DEPS_H = QbLzwDecoder.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbLzwDictionary.o QbLzwDictionaryEntry.o

include _tdd-footer.inc.makefile
