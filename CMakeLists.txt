cmake_minimum_required (VERSION 3.13)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jmedia-project)
project(jmedia
  LANGUAGES CXX
  HOMEPAGE_URL https://github.com/jeffersonfr/jmedia
  DESCRIPTION "A library for abstract media engine"
  VERSION 1.0.0
)

option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -pedantic -Werror)

if (COVERAGE)
  add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs -lgcov)
endif()

if (SANITIZE)
  set(SANITIZE_FLAGS -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope -U_FORTIFY_SOURCE -fno-omit-frame-pointer -fno-common)

  add_compile_options(${SANITIZE_FLAGS})
  add_link_options(${SANITIZE_FLAGS} -lasan -lubsan)
endif()

if (PROFILE)
  add_compile_options(-pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0)
endif()

find_package(PkgConfig REQUIRED)

enable_testing()

add_subdirectory(src)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

# generate pc file
configure_file(jmedia.pc.in jmedia.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/jmedia.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
