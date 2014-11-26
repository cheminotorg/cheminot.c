PROJECT(libprotobuf)

set(PROTOBUF_ROOT ${CMAKE_CURRENT_BINARY_DIR}/lib/protobuf)
set(INCLUDE_DIR include)
set(LIB_DIR lib)

include_directories(${PROTOBUF_ROOT} ${PROTOBUF_ROOT}/src)

function(append_if_exist OUTPUT_LIST)
    set(${OUTPUT_LIST})
    foreach(f ${ARGN})
        if(EXISTS ${f})
            list(APPEND ${OUTPUT_LIST} "${f}")
        else()
            message("Warning: file missing: ${f}")
        endif()
    endforeach()
    set(${OUTPUT_LIST} ${${OUTPUT_LIST}} PARENT_SCOPE)
endfunction()

append_if_exist(SOURCES
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/importer.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/parser.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor.pb.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor_database.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/dynamic_message.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/extension_set.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/extension_set_heavy.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/generated_message_reflection.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/generated_message_util.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/coded_stream.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/gzip_stream.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/printer.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/tokenizer.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream_impl.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/message.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/message_lite.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/reflection_ops.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/repeated_field.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/service.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/common.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/once.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/stringprintf.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/structurally_valid.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/strutil.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/substitute.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/text_format.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/unknown_field_set.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/wire_format.cc
  ${PROTOBUF_ROOT}/src/google/protobuf/wire_format_lite.cc
)

add_library(libprotobuf ${SOURCES})
set_target_properties(libprotobuf PROPERTIES OUTPUT_NAME "protobuf")
install(TARGETS libprotobuf DESTINATION ${LIB_DIR})

append_if_exist(INSTALL_FILES_LIST
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/code_generator.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/command_line_interface.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/cpp/cpp_generator.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/importer.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/java/java_generator.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/parser.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/plugin.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/plugin.pb.h
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/plugin.proto
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/python/python_generator.h
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor_database.h
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor.h
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor.pb.h
  ${PROTOBUF_ROOT}/src/google/protobuf/descriptor.proto
  ${PROTOBUF_ROOT}/src/google/protobuf/dynamic_message.h
  ${PROTOBUF_ROOT}/src/google/protobuf/extension_set.h
  ${PROTOBUF_ROOT}/src/google/protobuf/generated_enum_reflection.h
  ${PROTOBUF_ROOT}/src/google/protobuf/generated_message_reflection.h
  ${PROTOBUF_ROOT}/src/google/protobuf/generated_message_util.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/coded_stream.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/gzip_stream.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/printer.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/tokenizer.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream_impl.h
  ${PROTOBUF_ROOT}/src/google/protobuf/io/zero_copy_stream_impl_lite.h
  ${PROTOBUF_ROOT}/src/google/protobuf/message.h
  ${PROTOBUF_ROOT}/src/google/protobuf/message_lite.h
  ${PROTOBUF_ROOT}/src/google/protobuf/reflection_ops.h
  ${PROTOBUF_ROOT}/src/google/protobuf/repeated_field.h
  ${PROTOBUF_ROOT}/src/google/protobuf/service.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_arm_gcc.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_arm_qnx.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_atomicword_compat.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_macosx.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_mips_gcc.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_pnacl.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_x86_gcc.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/atomicops_internals_x86_msvc.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/common.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/once.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/platform_macros.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/template_util.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/type_traits.h
  ${PROTOBUF_ROOT}/src/google/protobuf/stubs/shared_ptr.h
  ${PROTOBUF_ROOT}/src/google/protobuf/text_format.h
  ${PROTOBUF_ROOT}/src/google/protobuf/unknown_field_set.h
  ${PROTOBUF_ROOT}/src/google/protobuf/wire_format.h
  ${PROTOBUF_ROOT}/src/google/protobuf/wire_format_lite.h
  ${PROTOBUF_ROOT}/src/google/protobuf/wire_format_lite_inl.h
)

foreach(f ${INSTALL_FILES_LIST})
  FILE(RELATIVE_PATH relative_file ${PROTOBUF_ROOT}/src ${f})
  GET_FILENAME_COMPONENT(relative_header_dir ${relative_file} PATH)
  INSTALL(FILES ${f} DESTINATION ${INCLUDE_DIR}/${relative_header_dir})
endforeach()
