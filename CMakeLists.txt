cmake_minimum_required(VERSION 3.16)

# compiler settings
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-9" )
set(CMAKE_C_COMPILER "/usr/local/bin/gcc-9" )

project(sort LANGUAGES CXX )

# source codes
set( SRC_WILDCARDS src/*.cpp )

# macros
set(MACRO "")
set(NDEBUG_MACRO NDEBUG=1)
set(DEBUG_MACRO DEBUG=1;_DEBUG=1)

# compile options
set(COMPLIE_OPT -Wall -march=native -fopenmp )
set(NDEBUG_COMPLIE_OPT -O2)
set(DEBUG_COMPLIE_OPT -g -O0)

# link options
set(LINK_OPT -Wall -lpthread -lgomp )
set(NDEBUG_LINK_OPT "")
set(DEBUG_LINK_OPT "")

set(CMAKE_CXX_FLAGS -std=c++17)

file(GLOB SOURCES ${SRC_WILDCARDS})
file(GLOB REL_SOURCES 
  RELATIVE ${CMAKE_CURRENT_BINARY_DIR}
  ${SRC_WILDCARDS})
file(GLOB TEST_SOURCES ${TEST_SRC_WILDCARDS})

function(define_build)
  cmake_parse_arguments(
    DEF           # 変数名につく prefix
    "DEBUG;TEST"  # bool 型の変数のリスト
    "COND;TARGET" # 値を一つとる変数のリスト
    ""            # 値を複数取る変数のリスト
    ${ARGN})

  set(TNAME "${DEF_COND}" )
  add_executable( "${TNAME}" )
  target_sources( "${TNAME}"
    PRIVATE
      ${SOURCES}
      $<$<BOOL:${DEF_TEST}>:${TEST_SOURCES}>
  )
  set_target_properties("${TNAME}" 
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${DEF_COND}"
      RUNTIME_OUTPUT_NAME "${DEF_TARGET}"
  )
  target_compile_definitions("${TNAME}"
    PRIVATE
      ${MACRO}
      $<$<NOT:$<BOOL:${DEF_DEBUG}>>:${NDEBUG_MACRO}>
      $<$<BOOL:${DEF_DEBUG}>:${DEBUG_MACRO}>
      $<$<BOOL:${DEF_TEST}>:${TEST_MACRO}>
  )
  target_compile_options( "${TNAME}"
    PRIVATE
      ${COMPLIE_OPT}
      $<$<NOT:$<BOOL:${DEF_DEBUG}>>:${NDEBUG_COMPLIE_OPT}>
      $<$<BOOL:${DEF_DEBUG}>:${DEBUG_COMPLIE_OPT}>
      $<$<BOOL:${DEF_TEST}>:${TEST_COMPLIE_OPT}>
  )
  target_include_directories("${TNAME}" PRIVATE
    "src"
  )
  target_link_options( "${TNAME}"
    PUBLIC
      ${LINK_OPT}
      $<$<NOT:$<BOOL:${DEF_DEBUG}>>:${NDEBUG_LINK_OPT}>
      $<$<BOOL:${DEF_DEBUG}>:${DEBUG_LINK_OPT}>
      $<$<BOOL:${DEF_TEST}>:${TEST_LINK_OPT}>
  )
  target_compile_features("${TNAME}" PUBLIC
    cxx_std_17
  )

endfunction()

define_build(
  COND "release"
  TARGET "sample.out")

define_build(
  DEBUG
  COND "debug"
  TARGET "sample.out")

