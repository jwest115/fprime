####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveRateGroup.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveRateGroup.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/ActiveRateGroup/ActiveRateGroup.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveRateGroupTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveRateGroupTester.cpp"
)
register_fprime_ut()
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
