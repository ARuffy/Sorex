set(SOURCE_FILE_EXTENSION_LIST "c" "cc" "cpp" "cxx")
set(INCLUDE_FILE_EXTENSION_LIST "h" "hpp")

function(sorex_target_source EXECUTABLE_NAME SOURCE_DIRS)
  if(SOURCE_DIRS)
    foreach(source_dir IN ITEMS ${SOURCE_DIRS})
      set(src_files_number 0)

      # Find source files recursively in derictory
      foreach(file_extension IN LISTS SOURCE_FILE_EXTENSION_LIST
                                      INCLUDE_FILE_EXTENSION_LIST
      )
        file(GLOB_RECURSE src_file_list "${source_dir}/*.${file_extension}")
        if(src_file_list)
          list(LENGTH src_file_list number)
          math(EXPR src_files_number "${src_files_number}+${number}")

          target_sources(${EXECUTABLE_NAME} PRIVATE ${src_file_list})

          # Debug: show all included files
          foreach(file_name IN ITEMS src_file_list)
            message(DEBUG "[${EXECUTABLE_NAME}] Add source file: ${file_name}")
          endforeach()

        endif()
      endforeach()

      # Display added message files
      if(src_files_number GREATER 0)
        message(
          STATUS
            "[${EXECUTABLE_NAME}] Source dir '${source_dir}' added (${src_files_number})"
        )
      else()
        message(
          STATUS
            "[${EXECUTABLE_NAME}] Skip directory ${source_dir} source files not found"
        )
      endif()

    endforeach()
  endif()

endfunction(sorex_target_source)
