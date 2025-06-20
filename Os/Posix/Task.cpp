// ======================================================================
// \title Os/Posix/Task.cpp
// \brief implementation of Posix implementation of Os::Task
// ======================================================================
#include <cstring>
#include <unistd.h>
#include <climits>
#include <cerrno>
#include <pthread.h>

#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Task.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/Posix/error.hpp"

namespace Os {
namespace Posix {
namespace Task {
    std::atomic<bool> PosixTask::s_permissions_reported(false);
    static const int SCHED_POLICY = SCHED_RR;

    typedef void* (*pthread_func_ptr)(void*);

    void* pthread_entry_wrapper(void* wrapper_pointer) {
        FW_ASSERT(wrapper_pointer != nullptr);
        Os::Task::TaskRoutineWrapper& wrapper = *reinterpret_cast<Os::Task::TaskRoutineWrapper*>(wrapper_pointer);
        wrapper.run(&wrapper);
        return nullptr;
    }

    int set_stack_size(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        int status = PosixTaskHandle::SUCCESS;
        FwSizeType stack = arguments.m_stackSize;
// Check for stack size multiple of page size or skip when the function
// is unavailable.
#ifdef _SC_PAGESIZE
        long page_size = sysconf(_SC_PAGESIZE);
#else
        long page_size = -1; // Force skip and warning
#endif
        if (page_size <= 0) {
            Fw::Logger::log(
                    "[WARNING] %s could not determine page size %s. Skipping stack-size check.\n",
                    const_cast<CHAR*>(arguments.m_name.toChar()),
                    strerror(errno)
            );
        }
        else if ((stack % static_cast<FwSizeType>(page_size)) != 0) {
            // Round-down to nearest page size multiple
            FwSizeType rounded = (stack / static_cast<FwSizeType>(page_size)) * static_cast<FwSizeType>(page_size);
            Fw::Logger::log(
                    "[WARNING] %s stack size of %" PRI_FwSizeType " is not multiple of page size %ld, rounding to %" PRI_FwSizeType "\n",
                    const_cast<CHAR*>(arguments.m_name.toChar()),
                    stack,
                    page_size,
                    rounded
            );
            stack = rounded;
        }

        // Clamp invalid stack sizes
        if (stack <= static_cast<FwSizeType>(PTHREAD_STACK_MIN)) {
            Fw::Logger::log(
                    "[WARNING] %s stack size of %" PRI_FwSizeType "  is too small, clamping to %" PRI_FwSizeType "\n",
                    const_cast<CHAR*>(arguments.m_name.toChar()),
                    stack,
                    static_cast<FwSizeType>(PTHREAD_STACK_MIN)
            );
            stack = static_cast<FwSizeType>(PTHREAD_STACK_MIN);
        }
        status = pthread_attr_setstacksize(&attributes, static_cast<size_t>(stack));
        return status;
    }

    int set_priority_params(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        const FwSizeType min_priority = static_cast<FwSizeType>(sched_get_priority_min(SCHED_POLICY));
        const FwSizeType max_priority = static_cast<FwSizeType>(sched_get_priority_max(SCHED_POLICY));
        int status = PosixTaskHandle::SUCCESS;
        FwSizeType priority = arguments.m_priority;
        // Clamp to minimum priority
        if (priority < min_priority) {
            Fw::Logger::log("[WARNING] %s low task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                               const_cast<CHAR*>(arguments.m_name.toChar()),
                               priority,
                               min_priority);
            priority = min_priority;
        }
        // Clamp to maximum priority
        else if (priority > max_priority) {
            Fw::Logger::log("[WARNING] %s high task priority of %" PRI_FwSizeType " clamped to %" PRI_FwSizeType "\n",
                               const_cast<CHAR*>(arguments.m_name.toChar()),
                               priority,
                               max_priority);
            priority = max_priority;
        }

        // Set attributes required for priority
        status = pthread_attr_setschedpolicy(&attributes, SCHED_POLICY);
        if (status == PosixTaskHandle::SUCCESS) {
            status = pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
        }
        if (status == PosixTaskHandle::SUCCESS) {
            sched_param schedParam;
            memset(&schedParam, 0, sizeof(sched_param));
            schedParam.sched_priority = static_cast<int>(priority);
            status = pthread_attr_setschedparam(&attributes, &schedParam);
        }
        return status;
    }

    int set_cpu_affinity(pthread_attr_t& attributes, const Os::Task::Arguments& arguments) {
        int status = 0;
// pthread_attr_setaffinity_np is a non-POSIX function. Notably, it is not available on musl.
// Limit its use to builds that involve glibc, on Linux, with _GNU_SOURCE defined.
// That's the circumstance in which we expect this feature to work.
#if defined(TGT_OS_TYPE_LINUX) && defined(__GLIBC__) && defined(_GNU_SOURCE)
        const FwSizeType affinity = arguments.m_cpuAffinity;
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(static_cast<int>(affinity), &cpu_set);

        // According to the man-page this function sets errno rather than returning an error status like other functions
        status = pthread_attr_setaffinity_np(&attributes, sizeof(cpu_set_t), &cpu_set);
        status = (status == PosixTaskHandle::SUCCESS) ? status : errno;
#else
            Fw::Logger::log("[WARNING] %s setting CPU affinity is only available with GNU pthreads\n",
                            const_cast<CHAR*>(arguments.m_name.toChar()));
#endif
        return status;
    }

    Os::Task::Status PosixTask::create(const Os::Task::Arguments& arguments, const PosixTask::PermissionExpectation permissions) {
        int pthread_status = PosixTaskHandle::SUCCESS;
        PosixTaskHandle& handle = this->m_handle;
        const bool expect_permission = (permissions == EXPECT_PERMISSION);
        // Initialize and clear pthread attributes
        pthread_attr_t attributes;
        memset(&attributes, 0, sizeof(attributes));
        pthread_status = pthread_attr_init(&attributes);
        if ((arguments.m_stackSize != Os::Task::TASK_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_stack_size(attributes, arguments);
        }
        if ((arguments.m_priority != Os::Task::TASK_PRIORITY_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_priority_params(attributes, arguments);
        }
        if ((arguments.m_cpuAffinity != Os::Task::TASK_DEFAULT) && (expect_permission) && (pthread_status == PosixTaskHandle::SUCCESS)) {
            pthread_status = set_cpu_affinity(attributes, arguments);
        }
        if (pthread_status == PosixTaskHandle::SUCCESS) {
            pthread_status = pthread_create(&handle.m_task_descriptor, &attributes, pthread_entry_wrapper,
                                            arguments.m_routine_argument);
        }
        // Successful execution of all precious steps will result in a valid task handle
        if (pthread_status == PosixTaskHandle::SUCCESS) {
            handle.m_is_valid = true;
        }

        (void) pthread_attr_destroy(&attributes);
        return Posix::posix_status_to_task_status(pthread_status);
    }

    void PosixTask::onStart() {}

    Os::Task::Status PosixTask::start(const Arguments& arguments) {
        FW_ASSERT(arguments.m_routine != nullptr);

        // Try to create thread with assuming permissions
        Os::Task::Status status = this->create(arguments, PermissionExpectation::EXPECT_PERMISSION);
        // Failure due to permission automatically retried
        if (status == Os::Task::Status::ERROR_PERMISSION) {
            if (not PosixTask::s_permissions_reported) {
                Fw::Logger::log("\n");
                Fw::Logger::log("[NOTE] Task Permissions:\n");
                Fw::Logger::log("[NOTE]\n");
                Fw::Logger::log("[NOTE] You have insufficient permissions to create a task with priority and/or cpu affinity.\n");
                Fw::Logger::log("[NOTE] A task without priority and affinity will be created.\n");
                Fw::Logger::log("[NOTE]\n");
                Fw::Logger::log("[NOTE] There are three possible resolutions:\n");
                Fw::Logger::log("[NOTE] 1. Use tasks without priority and affinity using parameterless start()\n");
                Fw::Logger::log("[NOTE] 2. Run this executable as a user with task priority permission\n");
                Fw::Logger::log("[NOTE] 3. Grant capability with \"setcap 'cap_sys_nice=eip'\" or equivalent\n");
                Fw::Logger::log("\n");
                PosixTask::s_permissions_reported = true;
            }
            // Fallback with no permission
            status = this->create(arguments, PermissionExpectation::EXPECT_NO_PERMISSION);
        } else if (status != Os::Task::Status::OP_OK) {
            Fw::Logger::log("[ERROR] Failed to create task with status: %d",
                            static_cast<int>(status));
        }
        return status;
    }

    Os::Task::Status PosixTask::join() {
        Os::Task::Status status = Os::Task::Status::JOIN_ERROR;
        if (not this->m_handle.m_is_valid) {
            status = Os::Task::Status::INVALID_HANDLE;
        } else {
            int stat = ::pthread_join(this->m_handle.m_task_descriptor, nullptr);
            status = (stat == PosixTaskHandle::SUCCESS) ? Os::Task::Status::OP_OK : Os::Task::Status::JOIN_ERROR;
        }
        return status;
    }

    TaskHandle* PosixTask::getHandle() {
        return &this->m_handle;
    }

    // Note: not implemented for Posix threads. Must be manually done using a mutex or other blocking construct as there
    // is no top-level pthreads support for suspend and resume.
    void PosixTask::suspend(Os::Task::SuspensionType suspensionType) {
        FW_ASSERT(0);
    }

    void PosixTask::resume() {
        FW_ASSERT(0);
    }


    Os::Task::Status PosixTask::_delay(Fw::TimeInterval interval) {
        Os::Task::Status task_status = Os::Task::OP_OK;
        timespec sleep_interval;
        sleep_interval.tv_sec = interval.getSeconds();
        sleep_interval.tv_nsec = interval.getUSeconds() * 1000;

        timespec remaining_interval;
        remaining_interval.tv_sec = 0;
        remaining_interval.tv_nsec = 0;

        while (true) {
            int status = nanosleep(&sleep_interval, &remaining_interval);
            // Success, return ok
            if (0 == status) {
                break;
            }
            // Interrupted, reset sleep and iterate
            else if (EINTR == errno) {
                sleep_interval = remaining_interval;
                continue;
            }
            // Anything else is an error
            else {
                task_status = Os::Task::Status::DELAY_ERROR;
                break;
            }
        }
        return task_status;
    }


} // end namespace Task
} // end namespace Posix
} // end namespace Os
