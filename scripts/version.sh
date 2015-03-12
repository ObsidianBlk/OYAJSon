#!/bin/bash

BASE_PATH="`pwd`"
if ! [ -z "$1" ]; then
  BASE_PATH="$1"
fi
SOURCE_PATH="/"

VERSION_SOURCE_FILENAME="OYAJSon_version.cpp"

create_version_source() {
  if ! [ -d ${BASE_PATH}${SOURCE_PATH} ]; then
    >&2 echo "Cannot find expected source path \"${BASE_PATH}${SOURCE_PATH}\"."
    exit 1
  fi

  FILE_PATH=${BASE_PATH}${SOURCE_PATH}${VERSION_SOURCE_FILENAME}
  if [ -f ${FILE_PATH} ]; then
    rm ${FILE_PATH}
  fi

  VERSION="UNKNOWN"
  if command -v git &>/dev/null && [ -d ${BASE_PATH}/.git ]; then
    VERSION="`git describe`"
  else
    echo "Either GIT is not installed, or \"${BASE_PATH}/.git\" does not exist."
  fi

  echo "/* This is an automatically generated file. Do not modify! */" >> ${FILE_PATH}
  echo "const char* OYAJSON_VERSION = \"${VERSION}\";" >> ${FILE_PATH}
  echo "" >> ${FILE_PATH}

  echo "FILE ${FILE_PATH} GENERATED SUCCESSFULLY"
}


create_version_source
