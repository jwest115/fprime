####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimerComponentImplTaskDelay.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimerComponentImplCommon.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimerComponentImplTimerFd.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimerComponentImplCommon.cpp"
	)
else()
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxTimerComponentImplCommon.cpp"
	)
endif()

register_fprime_module()

set(UT_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxTimerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)

register_fprime_ut()
