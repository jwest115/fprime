####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Time.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Time.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TimeInterval.cpp"
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeIntervalTester.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/Fw/Time"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
register_fprime_ut()
