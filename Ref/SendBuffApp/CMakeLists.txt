####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
    -Wno-conversion
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SendBuffApp.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SendBuffComponentImpl.cpp"
)
register_fprime_module()
