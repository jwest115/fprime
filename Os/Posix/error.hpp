// ======================================================================
// \title Os/Posix/error.hpp
// \brief header for posix errno conversion
// ======================================================================
#ifndef OS_POSIX_ERRNO_HPP
#define OS_POSIX_ERRNO_HPP
#include "Os/File.hpp"
#include "Os/Task.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Directory.hpp"
#include "Os/RawTime.hpp"
#include "Os/Condition.hpp"

namespace Os {
namespace Posix {

//! Convert an errno representation of an error to the Os::File::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::File::Status errno_to_file_status(int errno_input);

//! Convert an errno representation of an error to the Os::FileSystem::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::FileSystem::Status representation of the error
//!
Os::FileSystem::Status errno_to_filesystem_status(int errno_input);

//! Convert an errno representation of an error to the Os::FileSystem::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::Directory::Status representation of the error
//!
Os::Directory::Status errno_to_directory_status(int errno_input);

//! Convert an errno representation of an error to the Os::RawTime::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::RawTime::Status representation of the error
//!
Os::RawTime::Status errno_to_rawtime_status(int errno_input);

//! Convert an posix task representation of an error to the Os::Task::Status representation.
//! \param posix_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status posix_status_to_task_status(int posix_status);

//! Convert a Posix return status (int) for mutex operations to the Os::Mutex::Status representation.
//! \param posix_status: return status
//! \return: Os::Mutex::Status representation of the error
//!
Os::Mutex::Status posix_status_to_mutex_status(int posix_status);

//! Convert a Posix return status (int) for Conditional Variable operations to the Os::ConditionVariable::Status
//! representation.
//! \param posix_status: return status
//! \return: Os::ConditionVariable::Status representation of the error
//!
Os::ConditionVariable::Status posix_status_to_conditional_status(int posix_status);

}
}
#endif
