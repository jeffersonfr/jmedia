cmake_minimum_required (VERSION 3.13)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jmedia-project)

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

# generate jmedia.pc
configure_file(${PROJECT_SOURCE_DIR}/jmedia.pc.in ${PROJECT_BINARY_DIR}/jmedia.pc.gen @ONLY)
file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/jmedia.pc INPUT "${CMAKE_BINARY_DIR}/jmedia.pc.gen")
install(FILES ${CMAKE_BINARY_DIR}/jmedia.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
