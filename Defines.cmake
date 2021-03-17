macro (assert TEST COMMENT)
  if (NOT ${TEST})
    message ("Assertion failed: ${COMMENT}")
  endif()
endmacro()


