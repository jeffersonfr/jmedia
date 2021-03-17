cmake_minimum_required (VERSION 3.13)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jmedia)

option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-std=c++20 -g -ggdb")

find_package(PkgConfig REQUIRED)

if (COVERAGE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} --coverage -lgcov"
  )
endif()

if (SANITIZE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope"
  )
endif()

if (PROFILE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0"
  )
  set(CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -pg"
  )
endif()

add_subdirectory(src)

enable_testing()

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

install(DIRECTORY resources/jmedia DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
install(FILES ${CMAKE_BINARY_DIR}/jmedia.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
