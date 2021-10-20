#!/bin/bash
set -e

source "./tdd-scripts.inc.sh"

date

# Build Main (if needed)
OBJ_FILE="../bin/tdd-main.o"
SRC_FILE="../src/tdd-main.cpp"
DEP_FILE_LIST=""
build_obj_file_if_needed "${OBJ_FILE}" "${SRC_FILE}" "${DEP_FILE_LIST}"

# Build Dependencies (if needed)
# -- QbLzwDictionaryEntry
OBJ_FILE="../bin/tdd-QbLzwDictionaryEntry.o"
SRC_FILE="../../QbGifBuilder/src/QbLzwDictionaryEntry.cpp"
DEP_FILE_LIST="../../QbGifBuilder/include/QbLzwDictionaryEntry.h"
build_obj_file_if_needed "${OBJ_FILE}" "${SRC_FILE}" "${DEP_FILE_LIST}"
# -- QbLzwDictionary
OBJ_FILE="../bin/tdd-QbLzwDictionary.o"
SRC_FILE="../../QbGifBuilder/src/QbLzwDictionary.cpp"
DEP_FILE_LIST="../../QbGifBuilder/include/QbLzwDictionary.h"
build_obj_file_if_needed "${OBJ_FILE}" "${SRC_FILE}" "${DEP_FILE_LIST}"
# -- QbLzwDecoder
OBJ_FILE="../bin/tdd-QbLzwDecoder.o"
SRC_FILE="../../QbGifBuilder/src/QbLzwDecoder.cpp"
DEP_FILE_LIST="../../QbGifBuilder/include/QbLzwDecoder.h"
build_obj_file_if_needed "${OBJ_FILE}" "${SRC_FILE}" "${DEP_FILE_LIST}"

echo -e "${YELLOW}---- Build test...${WHITE}"
OBJ_FILE="../bin/tdd-QbLzwDecoder.x"
SRC_FILE="../src/tdd-QbLzwDecoder.cpp"
DEP_FILE_LIST="../bin/tdd-main.o"
DEP_FILE_LIST+=",../bin/tdd-QbLzwDictionaryEntry.o"
DEP_FILE_LIST+=",../bin/tdd-QbLzwDictionary.o"
DEP_FILE_LIST+=",../bin/tdd-QbLzwDecoder.o"
GPP_OPTIONS="$( echo "${DEP_FILE_LIST}" | sed 's/,/ /g' )"
build_obj_file_if_needed "${OBJ_FILE}" "${SRC_FILE}" "${DEP_FILE_LIST}" "${GPP_OPTIONS}"
date
echo ""
echo -e "${YELLOW}---- Run test...${WHITE}"
echo ""
 ../bin/./tdd-QbLzwDecoder.x

 exit $?
