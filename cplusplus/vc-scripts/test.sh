#!/bin/bash

TEST_SRC_FILENAME="$1"

if [[ ! "${TEST_SRC_FILENAME}" =~ .*/TDD/src/.*\.cpp ]] ; then
  echo ""
  echo "ERROR: '${TEST_SRC_FILENAME}' is not a TDD source file !"
  echo ""
  exit 1
fi

TEST_FILENAME="$( basename "${TEST_SRC_FILENAME}" | sed "s/\.cpp$//" )"
MAKEFILES_DIR="$( dirname "${TEST_SRC_FILENAME}" )/../makefiles"

echo "************* TEST : ${TEST_FILENAME}"
echo ""

cd "${MAKEFILES_DIR}"
make --trace -f ${TEST_FILENAME}.makefile ../bin/${TEST_FILENAME}.x
RC=$?
[ $RC -eq 0 ] && "../bin/${TEST_FILENAME}.x"

exit $?