ifndef __TDD_FOOTER_$(TARGET)__
__TDD_FOOTER_$(TARGET)__ = 1

# Build pattern-specific variables
# Note: pattern-specific are used to prevent variable overwrite
#       when this makefile is included into another makefile
TARGET_O := $(BIN_DIR)/$(TARGET).o
TDD_MAIN_X := $(BIN_DIR)/tdd-$(TARGET).x
TARGET_CPP := $(QGB_DIR)/src/$(TARGET).cpp
TARGET_H := $(QGB_DIR)/include/$(TARGET).h
TARGET_TDD_SRC := $(TDD_SRC_DIR)/tdd-$(TARGET).cpp
DEPS_H_EX := $(patsubst %,$(QGB_DIR)/include/%,$(DEPS_H))
DEPS_OBJ_EX := $(patsubst %,$(BIN_DIR)/%,$(DEPS_OBJ))
DEPS_TDD_OBJ_EX := $(patsubst %,$(BIN_DIR)/%,$(DEPS_TDD_OBJ))
$(TARGET_O) $(TDD_MAIN_X): TARGET_O := $(TARGET_O)
$(TARGET_O) $(TDD_MAIN_X): TDD_MAIN_X := $(TDD_MAIN_X)
$(TARGET_O) $(TDD_MAIN_X): TARGET_CPP := $(TARGET_CPP)
$(TARGET_O) $(TDD_MAIN_X): TARGET_H := $(TARGET_H)
$(TARGET_O) $(TDD_MAIN_X): TARGET_TDD_SRC := $(TARGET_TDD_SRC)
$(TARGET_O) $(TDD_MAIN_X): DEPS_H_EX = $(DEPS_H_EX)
$(TARGET_O) $(TDD_MAIN_X): DEPS_OBJ_EX := $(DEPS_TDD_EX)
$(TARGET_O) $(TDD_MAIN_X): DEPS_TDD_OBJ_EX := $(DEPS_TDD_OBJ_EX)

ifeq ($(BUILD_TARGET_$(TARGET)),0)

$(TDD_MAIN_X): $(TDD_MAIN_O) $(TARGET_TDD_SRC) $(DEPS_TDD_OBJ_EX)
	$(CC) $(CFLAGS) -o $(TDD_MAIN_X) $(TDD_MAIN_O) $(DEPS_TDD_OBJ_EX) $(TARGET_TDD_SRC)

else

$(TARGET_O): $(TARGET_CPP) $(TARGET_H) $(DEPS_H_EX) $(DEPS_OBJ_EX)
	$(CC) -c $(CFLAGS) -o $(TARGET_O) $(DEPS_OBJ_EX) $(TARGET_CPP)

$(TDD_MAIN_X): $(TARGET_O) $(TDD_MAIN_O) $(TARGET_TDD_SRC) $(DEPS_TDD_OBJ_EX)
	$(CC) $(CFLAGS) -o $(TDD_MAIN_X) $(TARGET_O) $(TDD_MAIN_O) $(DEPS_TDD_OBJ_EX) $(TARGET_TDD_SRC)

endif

#
#.PHONY: clean
#
#clean:
#	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
#

endif
