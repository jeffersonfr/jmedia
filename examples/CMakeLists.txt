cmake_minimum_required (VERSION 3.0)

macro(module_test)
  add_executable(${ARGV0}_example
    ${ARGV0}.cpp
  )

  target_link_libraries(${ARGV0}_example
    PRIVATE
      jmedia
  )
endmacro()

module_test(fullscreen)
module_test(synth)
module_test(teste)
