####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
  -Wno-double-promotion
)
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/TypeDemo.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/TypeDemo.cpp"
)
set(MOD_DEPS
    Os
)
register_fprime_module()
