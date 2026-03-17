# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
)
register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS
  Fw/Test
  STest
)

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
