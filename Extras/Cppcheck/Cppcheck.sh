#!/bin/bash

PROJECT_NAME='Cppcheck'

CPPCHECK_EXE=$(whereis -b cppcheck | awk '{print $2}')

CPPCHECK_DIR=$(pwd)"/build/cppcheck"
CPPCHECK_PROJ_DIR="${CPPCHECK_DIR}/proj"
OUTPUT_FILE_NAME="${CPPCHECK_DIR}/cppcheck-result.xml"

USE_CLANG=0

get_absolute_path() {
  local input_path="$1"
  local absolute_path

  # Check if the input path starts with "./" (relative path)
  if [[ "$input_path" == "./"* ]]; then
    absolute_path=$(pwd)"/${input_path#./}"
  else
    # Assume it's already an absolute path
    absolute_path="$input_path"
  fi

  echo "$absolute_path"
}

if [[ -z "${CPPCHECK_EXE}" ]]; then
    echo "[${PROJECT_NAME}] cppcheck utils isn't found"
    exit 1
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
    --clang | -c)
      USE_CLANG=1
      shift ;;
    --output-file | -o)
      OUTPUT_FILE_NAME=$(get_absolute_path "$2")
      if [[ -z "${OUTPUT_FILE_NAME}" ]]; then
        echo "invalid output file name"
        exit 1
      fi
      shift 2 ;;
    ?) echo "Script unknown option: '${1}'" >&2
       exit 1 ;;
  esac
done


if [ ! -d "${CPPCHECK_PROJ_DIR}" ]; then
    mkdir -vp "${CPPCHECK_PROJ_DIR}"
fi

CLANG_PATH=$(whereis -b clang | awk '{print $2}')
if (($USE_CLANG)) && [ -n "$CLANG_PATH" ]; then
    echo "[${PROJECT_NAME}] Enable clang parser '${CLANG_PATH}'"
    CLANG_FLAG="--clang=${CLANG_PATH}"
else
    CLANG_FLAG=""
fi

USE_THREADS=1
THREADS_NUM=$(cat /proc/cpuinfo | grep processor | wc -l)
if [[ $USE_THREADS -gt $THREADS_NUM ]]; then
    USE_THREADS=$THREADS_NUM
    echo "[${PROJECT_NAME}] Use threads: ${THREADS_NUM}"
fi

${CPPCHECK_EXE} -j${USE_THREADS}\
      ${CLANG_FLAG}  \
    --platform=unix64 \
    --std=c++20 \
    --cppcheck-build-dir=${CPPCHECK_PROJ_DIR} \
    --enable=style,performance,portability \
    --inconclusive \
    --library=std,opengl.cfg,googletest.cfg,zlib.cfg \
    --inline-suppr \
    --suppressions-list=.cppcheck-supressions \
    --xml \
    --output-file=${OUTPUT_FILE_NAME} \
    --error-exitcode=1 \
    -DSOREX_DEBUG_MODE=1 \
    -DTARGET_PLATFORM_LINUX=1 \
    -I./Engine/Include/ \
    -I./Precompiled/ \
    ./Engine/Source
