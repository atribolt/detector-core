# Load version from file
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/version GPIO_CORE_VERSION)


find_program(GIT git)

if (NOT DEFINED GIT-NOTFOUND)
  # Load TWEAK version
  execute_process(
    COMMAND bash -c "git rev-list --count $(git log --pretty=format:%h -1 -- version)..HEAD"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GPIO_CORE_VERSION_TWEAK

    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE TWEAK_OK
  )

  if (TWEAK_OK EQUAL 0)
    string(APPEND GPIO_CORE_VERSION ".${GPIO_CORE_VERSION_TWEAK}")
  endif()

  # Load SHORT HASH
  execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GPIO_CORE_COMMIT

    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE COMMIT_OK
  )

  if (NOT COMMIT_OK EQUAL 0)
    set(GPIO_CORE_COMMIT "unknown")
  endif()

endif()
