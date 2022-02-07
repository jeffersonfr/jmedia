cmake_minimum_required (VERSION 3.13)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jmedia-project)

option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)
option(DEBUG "Enable debug" OFF)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

if (COVERAGE)
  set(COVERAGE_FLAGS
    "--coverage -lgcov"
  )
endif()

if (SANITIZE)
  set(SANITIZE_FLAGS
    "-fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope"
  )
endif()

if (PROFILE)
  set(PROFILE_FLAGS
    "-pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0"
  )
endif()

if (DEBUG)
  set(DEBUG_FLAGS
    "-g -ggdb"
  )
endif()

set(CMAKE_CXX_FLAGS 
  "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS} ${SANITIZE_FLAGS} ${DEBUG_FLAGS}"
)

set(CMAKE_EXE_LINKER_FLAGS 
  "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_FLAGS} ${SANITIZE_FLAGS} ${DEBUG_FLAGS}"
)

enable_testing()

add_subdirectory(src)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

# generate jmedia.pc
set (JMEDIA_INCLUDE_DIRS "")
set (JMEDIA_LIBRARIES "-lpthread")

configure_file(
  ${PROJECT_SOURCE_DIR}/jmedia.pc.in
  ${PROJECT_BINARY_DIR}/jmedia.pc
  @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/jmedia.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
