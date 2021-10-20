include _tdd-header.inc.makefile

include tdd-QbLzwDictionary.makefile
include tdd-QbLzwDictionaryEntry.makefile

TARGET = QbLzwEncoder
DEPS_H = QbLzwEncoder.h
DEPS_OBJ = 
DEPS_TDD_OBJ = QbLzwDictionary.o QbLzwDictionaryEntry.o

include _tdd-footer.inc.makefile
