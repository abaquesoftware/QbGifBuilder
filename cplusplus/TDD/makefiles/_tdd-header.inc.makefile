ifndef __TDD_HEADER__
define __TDD_HEADER__
  1
endef

CC = g++
CFLAGS = 
CFLAGS +=  -I ../../ 
CFLAGS +=  -I ../../QbGifBuilder/include
BIN_DIR = ../bin
TDD_SRC_DIR = ../src
QGB_DIR = ../../QbGifBuilder

TDD_MAIN_O := $(BIN_DIR)/tdd-main.o
TDD_MAIN_SRC := $(TDD_SRC_DIR)/tdd-main.cpp

$(TDD_MAIN_O): $(TDD_MAIN_SRC)
	$(CC) -c $(CFLAGS) -o $(TDD_MAIN_O) $(TDD_MAIN_SRC)

endif