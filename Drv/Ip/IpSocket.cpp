// ======================================================================
// \title  IpSocket.cpp
// \author mstarch, crsmith
// \brief  cpp file for IpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <cstring>
#include <Drv/Ip/IpSocket.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <sys/time.h>

// This implementation has primarily implemented to isolate
// the socket interface from the F' Fw::Buffer class.
// There is a macro in VxWorks (m_data) that collides with
// the m_data member in Fw::Buffer.

#ifdef TGT_OS_TYPE_VXWORKS
#include <socket.h>
#include <inetLib.h>
#include <fioLib.h>
#include <hostLib.h>
#include <ioLib.h>
#include <vxWorks.h>
#include <sockLib.h>
#include <fioLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <errnoLib.h>
#include <cstring>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#else
#error OS not supported for IP Socket Communications
#endif


namespace Drv {

IpSocket::IpSocket() : m_timeoutSeconds(0), m_timeoutMicroseconds(0), m_port(0) {
    ::memset(m_hostname, 0, sizeof(m_hostname));
}

SocketIpStatus IpSocket::configure(const char* const hostname, const U16 port, const U32 timeout_seconds, const U32 timeout_microseconds) {
    FW_ASSERT(timeout_microseconds < 1000000, static_cast<FwAssertArgType>(timeout_microseconds));
    FW_ASSERT(this->isValidPort(port), static_cast<FwAssertArgType>(port));
    FW_ASSERT(hostname != nullptr);
    this->m_timeoutSeconds = timeout_seconds;
    this->m_timeoutMicroseconds = timeout_microseconds;
    this->m_port = port;
    (void) Fw::StringUtils::string_copy(this->m_hostname, hostname, static_cast<FwSizeType>(SOCKET_MAX_HOSTNAME_SIZE));
    return SOCK_SUCCESS;
}

bool IpSocket::isValidPort(U16 port) {
    return true;
}

SocketIpStatus IpSocket::setupTimeouts(int socketFd) {
// Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
    // No timeouts set on Vxworks
#else
    // Set timeout socket option
    struct timeval timeout;
    timeout.tv_sec = static_cast<time_t>(this->m_timeoutSeconds);
    timeout.tv_usec = static_cast<suseconds_t>(this->m_timeoutMicroseconds);
    // set socket write to timeout after 1 sec
    if (setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) < 0) {
        return SOCK_FAILED_TO_SET_SOCKET_OPTIONS;
    }
#endif
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::addressToIp4(const char* address, void* ip4) {
    FW_ASSERT(address != nullptr);
    FW_ASSERT(ip4 != nullptr);
    // Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
    int ip = inet_addr(address);
    if (ip == ERROR) {
        return SOCK_INVALID_IP_ADDRESS;
    }
    // from sin_addr, which has one struct
    // member s_addr, which is unsigned int
    *reinterpret_cast<unsigned long*>(ip4) = ip;
#else
    // First IP address to socket sin_addr
    if (not ::inet_pton(AF_INET, address, ip4)) {
        return SOCK_INVALID_IP_ADDRESS;
    };
#endif
    return SOCK_SUCCESS;
}

void IpSocket::close(const SocketDescriptor& socketDescriptor) {
    (void)::close(socketDescriptor.fd);
}

void IpSocket::shutdown(const SocketDescriptor& socketDescriptor) {
    errno = 0;
    int status = ::shutdown(socketDescriptor.fd, SHUT_RDWR);
    // If shutdown fails, go straight to the hard-shutdown
    if (status != 0) {
        this->close(socketDescriptor);
    }
}

SocketIpStatus IpSocket::open(SocketDescriptor& socketDescriptor) {
    SocketIpStatus status = SOCK_SUCCESS;
    errno = 0;
    // Open a TCP socket for incoming commands, and outgoing data if not using UDP
    status = this->openProtocol(socketDescriptor);
    if (status != SOCK_SUCCESS) {
        socketDescriptor.fd = -1;
        return status;
    }
    return status;
}

SocketIpStatus IpSocket::send(const SocketDescriptor& socketDescriptor, const U8* const data, const U32 size) {
    U32 total = 0;
    I32 sent  = 0;
    // Attempt to send out data and retry as necessary
    for (U32 i = 0; (i < SOCKET_MAX_ITERATIONS) && (total < size); i++) {
        errno = 0;
        // Send using my specific protocol
        sent = this->sendProtocol(socketDescriptor, data + total, size - total);
        // Error is EINTR or timeout just try again
        if (((sent == -1) && (errno == EINTR)) || (sent == 0)) {
            continue;
        }
        // Error bad file descriptor is a close along with reset
        else if ((sent == -1) && ((errno == EBADF) || (errno == ECONNRESET))) {
            return SOCK_DISCONNECTED;
        }
        // Error returned, and it wasn't an interrupt nor a disconnect
        else if (sent == -1) {
            return SOCK_SEND_ERROR;
        }
        FW_ASSERT(sent > 0, static_cast<FwAssertArgType>(sent));
        total += static_cast<U32>(sent);
    }
    // Failed to retry enough to send all data
    if (total < size) {
        return SOCK_INTERRUPTED_TRY_AGAIN;
    }
    // Ensure we sent everything
    FW_ASSERT(total == size, static_cast<FwAssertArgType>(total), static_cast<FwAssertArgType>(size));
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::recv(const SocketDescriptor& socketDescriptor, U8* data, U32& req_read) {
    I32 size = 0;
    // Try to read until we fail to receive data
    for (U32 i = 0; (i < SOCKET_MAX_ITERATIONS) && (size <= 0); i++) {
        errno = 0;
        // Attempt to recv out data
        size = this->recvProtocol(socketDescriptor, data, req_read);

        // Nothing to be received
        if ((size == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            req_read = 0;
            return SOCK_NO_DATA_AVAILABLE;
        }

        // Error is EINTR, just try again
        if ((size == -1) && (errno == EINTR)) {
            continue;
        }
        // Zero bytes read reset or bad ef means we've disconnected
        else if (size == 0 || ((size == -1) && ((errno == ECONNRESET) || (errno == EBADF)))) {
            req_read = static_cast<U32>(size);
            return SOCK_DISCONNECTED;
        }
        // Error returned, and it wasn't an interrupt, nor a disconnect
        else if (size == -1) {
            req_read = static_cast<U32>(size);
            return SOCK_READ_ERROR;  // Stop recv task on error
        }
    }
    req_read = static_cast<U32>(size);
    // Prevent interrupted socket being viewed as success
    if (size == -1) {
        return SOCK_INTERRUPTED_TRY_AGAIN;
    }
    return SOCK_SUCCESS;
}

}  // namespace Drv
