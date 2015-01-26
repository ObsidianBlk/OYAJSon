#!/bin/bash
SOURCE_PATH="OYAJSon/"
VERSION_SOURCE_FILENAME="OYAJSon_version.cpp"

rm ${SOURCE_PATH}${VERSION_SOURCE_FILENAME}
echo "/* This is an automatically generated file. Do not modify! */
const char* OYAJSON_VERSION = \"`git describe` - `git rev-parse --short HEAD`\";
" >> ${SOURCE_PATH}${VERSION_SOURCE_FILENAME}
