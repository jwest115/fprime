#ifndef FW_ASSERT_HPP
#define FW_ASSERT_HPP

#include <Fw/FPrimeBasicTypes.hpp>

// Return only the first argument passed to the macro.
#define FW_ASSERT_FIRST_ARG(ARG_0, ...) ARG_0
// Return all the arguments of the macro, but the first one
#define FW_ASSERT_NO_FIRST_ARG(ARG_0, ...) __VA_ARGS__

#if FW_ASSERT_LEVEL == FW_NO_ASSERT
// Users may override the NO_ASSERT case should they choose
#ifndef FW_ASSERT
#define FW_ASSERT(...) ((void)(FW_ASSERT_FIRST_ARG(__VA_ARGS__)))
#endif
#define FILE_NAME_ARG const CHAR*
#else  // ASSERT is defined

// Passing the __LINE__ argument at the end of the function ensures that
// the FW_ASSERT_NO_FIRST_ARG macro will never have an empty variadic variable
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT && defined ASSERT_FILE_ID
#define FILE_NAME_ARG U32
#define FW_ASSERT(...)                            \
    ((void)((FW_ASSERT_FIRST_ARG(__VA_ARGS__, 0)) \
                ? (0)                             \
                : (Fw::SwAssert(ASSERT_FILE_ID, FW_ASSERT_NO_FIRST_ARG(__VA_ARGS__, __LINE__)))))
#elif FW_ASSERT_LEVEL == FW_FILEID_ASSERT && !defined ASSERT_FILE_ID
#define FILE_NAME_ARG U32
#define FW_ASSERT(...)                            \
    ((void)((FW_ASSERT_FIRST_ARG(__VA_ARGS__, 0)) \
                ? (0)                             \
                : (Fw::SwAssert(static_cast<U32>(0), FW_ASSERT_NO_FIRST_ARG(__VA_ARGS__, __LINE__)))))
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT && defined ASSERT_RELATIVE_PATH
#define FILE_NAME_ARG const CHAR*
#define FW_ASSERT(...)                            \
    ((void)((FW_ASSERT_FIRST_ARG(__VA_ARGS__, 0)) \
                ? (0)                             \
                : (Fw::SwAssert(ASSERT_RELATIVE_PATH, FW_ASSERT_NO_FIRST_ARG(__VA_ARGS__, __LINE__)))))
#else
#define FILE_NAME_ARG const CHAR*
#define FW_ASSERT(...)                            \
    ((void)((FW_ASSERT_FIRST_ARG(__VA_ARGS__, 0)) \
                ? (0)                             \
                : (Fw::SwAssert(__FILE__, FW_ASSERT_NO_FIRST_ARG(__VA_ARGS__, __LINE__)))))
#endif
#endif  // if ASSERT is defined

// Helper macro asserting that a value fits into a type without overflow. Helpful for checking before static casts
#define FW_ASSERT_NO_OVERFLOW(value, T) \
    FW_ASSERT((value) <= std::numeric_limits<T>::max(), \
              static_cast<FwAssertArgType>(value))

// F' Assertion functions can technically return even though the intention is for the assertion to terminate the
// program. This breaks static analysis depending on assertions, since the analyzer has to assume the assertion will
// return. When supported, annotate assertion functions as noreturn when statically analyzing.
#ifndef CLANG_ANALYZER_NORETURN
#ifndef __has_feature
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#if __has_feature(attribute_analyzer_noreturn)
#define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#else
#define CLANG_ANALYZER_NORETURN
#endif
#endif

namespace Fw {
//! Assert with no arguments
I8 SwAssert(FILE_NAME_ARG file, FwSizeType lineNo) CLANG_ANALYZER_NORETURN;

//! Assert with one argument
I8 SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo) CLANG_ANALYZER_NORETURN;

//! Assert with two arguments
I8 SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo)
    CLANG_ANALYZER_NORETURN;

//! Assert with three arguments
I8 SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwSizeType lineNo) CLANG_ANALYZER_NORETURN;

//! Assert with four arguments
I8 SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwSizeType lineNo) CLANG_ANALYZER_NORETURN;

//! Assert with five arguments
I8 SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwSizeType lineNo) CLANG_ANALYZER_NORETURN;

//! Assert with six arguments
I8 SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwAssertArgType arg6,
                         FwSizeType lineNo) CLANG_ANALYZER_NORETURN;
}  // namespace Fw

// Base class for declaring an assert hook
// Each of the base class functions can be overridden
// or used by derived classes.

namespace Fw {
// Base class for declaring an assert hook
class AssertHook {
  public:
    AssertHook() : previousHook(nullptr){};  //!< constructor
    virtual ~AssertHook(){};                 //!< destructor
    // override this function to intercept asserts
    virtual void reportAssert(FILE_NAME_ARG file,
                              FwSizeType lineNo,
                              FwSizeType numArgs,
                              FwAssertArgType arg1,
                              FwAssertArgType arg2,
                              FwAssertArgType arg3,
                              FwAssertArgType arg4,
                              FwAssertArgType arg5,
                              FwAssertArgType arg6);
    // default reportAssert() will call this when the message is built
    // override it to do another kind of print. printf by default
    virtual void printAssert(const CHAR* msg);
    // do assert action. By default, calls assert.
    // Called after reportAssert()
    virtual void doAssert();
    // register the hook
    void registerHook();
    // deregister the hook
    void deregisterHook();

  protected:
  private:
    // the previous assert hook
    AssertHook* previousHook;
};
}  // namespace Fw

#endif  // FW_ASSERT_HPP
