#!/bin/bash

RED=$"\e[31m"
CYAN=$"\e[36m"
YELLOW=$"\e[33m"
WHITE=$"\e[97m"


function split_string {
  local STRING="$1"
  local SEPARATOR="$2"
  local RESULT=""
  [ "${SEPARATOR}" = "" ] && SEPARATOR=","
  if [ -n "${STRING}" ] ; then
    RESULT="\"$( echo "${STRING}" | sed "s/${SEPARATOR}/\" \"/g" )\""
  fi
  echo "${RESULT}"
}

function build_file {
  local OBJ_FILE="$1"
  local SRC_FILE="$2"
  local GPP_OPTIONS="$3"
  [[ "${OBJ_FILE}" =~ .*\.o$ ]] && GPP_OPTIONS+=" -c"
  echo -e "${YELLOW}... build ${OBJ_FILE}${WHITE}"
  set -x
  #g++ -std=c++11 -Wall -I ../../ -I ../../QbGifBuilder/include ${GPP_OPTIONS} -o "${OBJ_FILE}" "${SRC_FILE}"
  g++ -Wall -I ../../ -I ../../QbGifBuilder/include ${GPP_OPTIONS} -o "${OBJ_FILE}" "${SRC_FILE}"
  set +x
}

function build_obj_file_if_needed {
  local OBJ_FILE="$1"
  local SRC_FILE="$2"
  local DEP_FILE_LIST="$3"
  local GPP_OPTIONS="$4"
  local MUST_BUILD=0
  [ $(is_more_recent "${SRC_FILE}" "${OBJ_FILE}" ) -eq 1 ] && MUST_BUILD=1
  OLD_IFS="$IFS"
  IFS=","
  for DEP_FILE in ${DEP_FILE_LIST}
  do
    [ $(is_more_recent "${DEP_FILE}" "${OBJ_FILE}" ) -eq 1 ] && MUST_BUILD=1
  done
  IFS="${OLD_IFS}"
  if [ ${MUST_BUILD} -eq 1 ]; then
    build_file "${OBJ_FILE}" "${SRC_FILE}" "${GPP_OPTIONS}"
  else
    return 0
  fi
}

function is_more_recent {
  local FILE1="$1"
  local FILE2="$2"
  local FILE1_TS=9999999999
  local FILE2_TS=0  
  [ ! -f "${FILE1}" ] && ( echo -e "${RED}Error: ${FILE1} doesn't exist !${WHITE}" 1>&2 )
  [ -f "${FILE1}" ] && FILE1_TS=$( stat -c "%Y" "${FILE1}" )
  [ -f "${FILE2}" ] && FILE2_TS=$( stat -c "%Y" "${FILE2}" )
  local RESULT=1
  [ ${FILE1_TS} -lt ${FILE2_TS} ] && RESULT=0
  echo "$RESULT"
}
