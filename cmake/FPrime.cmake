####
# FPrime.cmake:
#
# This file is the entry point for building fprime projects, libraries. It does not setup F prime as a project, but
# rather allows the users to build against fprime, fprime libraries while taking advantage of fprime's autocoding
# support. This file includes the cmake build system setup for building like fprime.
####
include_guard()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
include(utilities)
include(options)
include(sanitizers) # Enable sanitizers if they are requested
include(required)
include(config_assembler)

# Add project root's cmake folder to module path
if (IS_DIRECTORY "${FPRIME_PROJECT_ROOT}/cmake")
    list(APPEND CMAKE_MODULE_PATH "${FPRIME_PROJECT_ROOT}/cmake")
endif()

# for adding libraries from the _fprime_packages directory
if (IS_DIRECTORY "${FPRIME_PROJECT_ROOT}/_fprime_packages")
    if (EXISTS "${FPRIME_PROJECT_ROOT}/_fprime_packages/packages.cmake")
        include("${FPRIME_PROJECT_ROOT}/_fprime_packages/packages.cmake")
        message(STATUS "[FPRIME] Including libraries from ${FPRIME_PROJECT_ROOT}/_fprime_packages")
    else()
        message(WARNING "[FPRIME] ${FPRIME_PROJECT_ROOT}/_fprime_packages/packages.cmake does not exist. Skipping.")
    endif()
endif()

# Setup fprime library locations
list(REMOVE_DUPLICATES FPRIME_LIBRARY_LOCATIONS)

# F Prime build locations represent the root of the module paths in F Prime. This allows us to detect module names from the
# paths to given files.
# Now that modules can build within the build cache, the build cache locations (root, F-Prime) are added to the list of
# locations. This allows for the detection of modules that are built within the build cache.
set(FPRIME_BUILD_LOCATIONS "${FPRIME_FRAMEWORK_PATH}" ${FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}"
    "${CMAKE_BINARY_DIR}/F-Prime" "${CMAKE_BINARY_DIR}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)
resolve_path_variables(FPRIME_BUILD_LOCATIONS)

# Message describing the fprime setup
message(STATUS "[FPRIME] Module locations: ${FPRIME_BUILD_LOCATIONS}")
message(STATUS "[FPRIME] Installation directory: ${CMAKE_INSTALL_PREFIX}")
include(platform/platform) # Now that module locations are known, load platform settings

# Module setup functions, attaches targets to modules, etc.
include(module)
# Support for autocoder implementations
include(autocoder/autocoder)
# Support for build target registration
include(target/target)
# Load domain-specific CMake functions
include(API)
# Sub-build support
include(sub-build/sub-build)
# C and C++ settings for building the framework
include(settings)

####
# Function `fprime_setup_global_includes`:
#
# Adds basic include directories that make fprime work. This ensures that configuration, framework, and project all
# function as expected. This will also include the internal build-cache directories.
####
function(fprime_setup_global_includes)
    # Setup the global include directories that exist outside of the build cache
    include_directories("${FPRIME_FRAMEWORK_PATH}")
    include_directories("${FPRIME_PROJECT_ROOT}")

    # Setup the include directories that exist within the build-cache
    include_directories("${CMAKE_BINARY_DIR}")
    include_directories("${CMAKE_BINARY_DIR}/F-Prime")
endfunction(fprime_setup_global_includes)

####
# Function `fprime_detect_libraries`:
#
# This function detects libraries using the FPRIME_LIBRARY_LOCATIONS variable. Fore each library path, the following is
# done:
# 1. Detect a manifest file from in-order: `library.cmake`, and then `<library name>.cmake`
# 2. Add the library's top-level cmake directory to the CMAKE_MODULE_PATH
# 3. Add the library root as an include directory
# 4. Add option() to disable library UTs
####
macro(fprime_detect_libraries)
    foreach (LIBRARY_DIRECTORY IN LISTS FPRIME_LIBRARY_LOCATIONS)
        get_filename_component(LIBRARY_NAME "${LIBRARY_DIRECTORY}" NAME)
        get_fprime_library_option_string(LIBRARY_OPTION "${LIBRARY_NAME}")
        # Detect manifest file:
        #  1. library.cmake (preferred)
        #  2. <library>.cmake (old standard)
        if (EXISTS "${LIBRARY_DIRECTORY}/library.cmake")
            set(MANIFEST_FILE "${LIBRARY_DIRECTORY}/library.cmake")
        elseif (EXISTS "${LIBRARY_DIRECTORY}/${LIBRARY_NAME}.cmake")
            set(MANIFEST_FILE "${LIBRARY_DIRECTORY}/${LIBRARY_NAME}.cmake")
        else()
            message(WARNING "[LIBRARY] ${LIBRARY_DIRECTORY} does not define library.cmake nor ${LIBRARY_NAME}.cmake. Skipping.")
            continue()
        endif()
        message(STATUS "[LIBRARY] Including library ${LIBRARY_NAME} at ${LIBRARY_DIRECTORY}")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[LIBRARY] ${LIBRARY_NAME} using manifest ${MANIFEST_FILE}")
        endif()
        append_list_property("${MANIFEST_FILE}" GLOBAL PROPERTY FPRIME_LIBRARY_MANIFESTS)
        # Check to see if the cmake directory exists and add it
        if (IS_DIRECTORY "${LIBRARY_DIRECTORY}/cmake")
            list(APPEND CMAKE_MODULE_PATH "${LIBRARY_DIRECTORY}/cmake")
        endif()
        include_directories("${LIBRARY_DIRECTORY}")
        option(FPRIME_ENABLE_${LIBRARY_OPTION}_UTS "Enable UT generation for ${LIBRARY_NAME}" ON)
    endforeach()
endmacro(fprime_detect_libraries)

####
# Function `fprime_setup_standard_targets`:
#
# Registers the targets required for a standard fprime build. This will be changed when FPRIME_SUB_BUILD_TARGETS.
####
macro(fprime_setup_standard_targets)
    # Prevent registration of standard targets when specific targets are supplied
    # This is done for efficiency to not load all the below files
    if (NOT DEFINED FPRIME_SUB_BUILD_TARGETS)
        # FPP locations must come at the front of the list, then build
        register_fprime_target(target/build)
        register_fprime_build_autocoder(autocoder/fpp OFF)
        register_fprime_target(target/version)
        register_fprime_target(target/install)
        register_fprime_ut_target(target/ut)
        register_fprime_target(target/sbom)

        if (FPRIME_ENABLE_UTIL_TARGETS)
            register_fprime_target(target/refresh_cache)
            register_fprime_ut_target(target/check)
        endif()
    endif()
endmacro(fprime_setup_standard_targets)

####
# Function `fprime_setup_override_targets`:
#
# Override the targets that are registered by the default build with those supplied in FPRIME_SUB_BUILD_TARGETS. If
# FPRIME_SUB_BUILD_TARGETS is defined, nothing happens.
#####
macro(fprime_setup_override_targets)
    # Required to prevent overriding when not used
    if (FPRIME_SUB_BUILD_TARGETS)
        foreach (OVERRIDE_TARGET IN LISTS FPRIME_SUB_BUILD_TARGETS)
            register_fprime_target("${OVERRIDE_TARGET}")
        endforeach ()
    elseif(CMAKE_DEBUG_OUTPUT)
        message(STATUS "FPRIME_SUB_BUILD_TARGETS not defined, skipping.")
    endif()
endmacro(fprime_setup_override_targets)

macro(fprime_initialize_build_system)
    cmake_minimum_required(VERSION 3.16)
    fprime_setup_global_includes()
    fprime_detect_libraries()
    fprime_setup_standard_targets()
    fprime_setup_override_targets()
    set_property(GLOBAL PROPERTY FPRIME_BUILD_SYSTEM_LOADED ON)

    # Perform necessary sub-builds
    if (NOT FPRIME_IS_SUB_BUILD)
        run_sub_build(info-cache target/sub-build/fpp_locs target/sub-build/fpp_depend target/sub-build/module_info)
        # Import the pre-computed properties!
        include("${CMAKE_BINARY_DIR}/fprime_module_info.cmake")
    endif()
endmacro(fprime_initialize_build_system)

####
# Function `fprime_setup_included_code`:
#
# Sets up the code/build for fprime and libraries. Call after all project specific targets and autocoders are set up and
# registered.
####
function(fprime_setup_included_code)
    # Must be done before code is registered but after custom target registration
    setup_global_targets()
    # For BUILD_TESTING builds then set up libraries that support testing
    if (BUILD_TESTING AND NOT DEFINED FPRIME_SUB_BUILD_TARGETS)
        if (NOT EXISTS "${FPRIME_FRAMEWORK_PATH}/googletest/CMakeLists.txt")
            message(FATAL_ERROR "googletest submodule not initialized or corrupted. Please run `git submodule update --init --recursive`.")
        endif()
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/googletest/" "${CMAKE_BINARY_DIR}/F-Prime/googletest")
        # Flags attached to GTest compile: disable conversion warnings
        set(GTEST_FLAGS "-Wno-conversion")
        target_compile_options(gmock      PRIVATE "${GTEST_FLAGS}")
        target_compile_options(gmock_main PRIVATE "${GTEST_FLAGS}")
        target_compile_options(gtest      PRIVATE "${GTEST_FLAGS}")
        target_compile_options(gtest_main PRIVATE "${GTEST_FLAGS}")
    endif()
    if (BUILD_TESTING)
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/STest/" "${CMAKE_BINARY_DIR}/F-Prime/STest")
    endif()
    # Check if we are allowing framework UTs
    if (FPRIME_ENABLE_FRAMEWORK_UTS)
        set(__FPRIME_NO_UT_GEN__ OFF)
    endif()
    message(STATUS "[LIBRARY] Adding modules from F´ framework")
    # Faux libraries used as interfaces to non-autocoded fpp items
    add_library(Fpp INTERFACE)

    # Specific configuration module handling:
    #
    # add_fprime_subdirectory cannot be run until later in the build process. Otherwise detection
    # for model specific post processing is messed up. Thus we synthesize the behavior by setting
    # the current module and then calling stock "add_subdirectory".
    fprime__include_platform_file()
    
    set(_FP_CORE_PACKAGES Fpp default Fw Svc Os Drv CFDP Utils)
    foreach (_FP_PACKAGE_DIR IN LISTS _FP_CORE_PACKAGES)
        set(FPRIME_CURRENT_MODULE "${_FP_PACKAGE_DIR}")
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/${_FP_PACKAGE_DIR}/" "${CMAKE_BINARY_DIR}/F-Prime/${_FP_PACKAGE_DIR}")
    endforeach ()
    unset(FPRIME_CURRENT_MODULE)
    message(STATUS "[LIBRARY] Adding modules from F´ framework - DONE")
    get_property(FPRIME_LIBRARY_MANIFESTS GLOBAL PROPERTY FPRIME_LIBRARY_MANIFESTS)
    foreach (LIBRARY_MANIFEST IN LISTS FPRIME_LIBRARY_MANIFESTS)
        set(__FPRIME_NO_UT_GEN__ OFF)
        get_filename_component(LIBRARY_DIRECTORY "${LIBRARY_MANIFEST}" DIRECTORY)
        get_filename_component(LIBRARY_NAME "${LIBRARY_DIRECTORY}" NAME)
        get_fprime_library_option_string(LIBRARY_OPTION "${LIBRARY_NAME}")
        if (NOT FPRIME_ENABLE_${LIBRARY_OPTION}_UTS)
            set(__FPRIME_NO_UT_GEN__ ON)
        endif()
        message(STATUS "[LIBRARY] Adding modules from ${LIBRARY_NAME}")
        include("${LIBRARY_MANIFEST}")
	message(STATUS "[LIBRARY] Adding modules from ${LIBRARY_NAME} - DONE")
    endforeach()
    # Always enable UTs for a project
    set(__FPRIME_NO_UT_GEN__ OFF)
endfunction(fprime_setup_included_code)


# Load the build system exactly one time
get_property(FPRIME_BUILD_SYSTEM_LOADED GLOBAL PROPERTY FPRIME_BUILD_SYSTEM_LOADED)
if (NOT FPRIME_BUILD_SYSTEM_LOADED)
    fprime_initialize_build_system()
endif ()
