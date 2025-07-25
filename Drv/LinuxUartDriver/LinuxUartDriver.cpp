// ======================================================================
// \title  LinuxUartDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxUartDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <unistd.h>
#include <Drv/LinuxUartDriver/LinuxUartDriver.hpp>
#include <Os/TaskString.hpp>

#include "Fw/Types/BasicTypes.hpp"

#include <fcntl.h>
#include <termios.h>
#include <cerrno>


namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxUartDriver ::LinuxUartDriver(const char* const compName)
    : LinuxUartDriverComponentBase(compName), m_fd(-1), m_allocationSize(0),  m_device("NOT_EXIST"), m_quitReadThread(false) {
}

bool LinuxUartDriver::open(const char* const device,
                           UartBaudRate baud,
                           UartFlowControl fc,
                           UartParity parity,
                           FwSizeType allocationSize) {
    FW_ASSERT(device != nullptr);
    int fd = -1;
    int stat = -1;
    this->m_allocationSize = allocationSize;

    this->m_device = device;


    /*
     The O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port. If you
     don't specify this then any input (such as keyboard abort signals and so forth) will affect your process. Programs
     like getty(1M/8) use this feature when starting the login process, but normally a user program does not want this
     behavior.
     */
    fd = ::open(device, O_RDWR | O_NOCTTY);

    if (fd == -1) {
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, this->m_fd, _err);
        return false;
    }

    this->m_fd = fd;

    // Configure blocking reads
    struct termios cfg;

    stat = tcgetattr(fd, &cfg);
    if (-1 == stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    /*
     If MIN > 0 and TIME = 0, MIN sets the number of characters to receive before the read is satisfied. As TIME is
     zero, the timer is not used.

     If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied if a single character is read,
     or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned.

     If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are
     received, or the time between two characters exceeds TIME. The timer is restarted every time a character is
     received and only becomes active after the first character has been received.

     If MIN = 0 and TIME = 0, read will be satisfied immediately. The number of characters currently available, or the
     number of characters requested will be returned. According to Antonino (see contributions), you could issue a
     fcntl(fd, F_SETFL, FNDELAY); before reading to get the same result.
     */
    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 10;  // 1 sec timeout on no-data

    stat = tcsetattr(fd, TCSANOW, &cfg);
    if (-1 == stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // Set flow control
    if (fc == HW_FLOW) {
        struct termios t;

        stat = tcgetattr(fd, &t);
        if (-1 == stat) {
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_OpenError(_arg, fd, _err);
            return false;
        }

        // modify flow control flags
        t.c_cflag |= CRTSCTS;

        stat = tcsetattr(fd, TCSANOW, &t);
        if (-1 == stat) {
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_OpenError(_arg, fd, _err);
            return false;
        }
    }

    int relayRate = B0;
    switch (baud) {
        case BAUD_9600:
            relayRate = B9600;
            break;
        case BAUD_19200:
            relayRate = B19200;
            break;
        case BAUD_38400:
            relayRate = B38400;
            break;
        case BAUD_57600:
            relayRate = B57600;
            break;
        case BAUD_115K:
            relayRate = B115200;
            break;
        case BAUD_230K:
            relayRate = B230400;
            break;
#if defined TGT_OS_TYPE_LINUX
        case BAUD_460K:
            relayRate = B460800;
            break;
        case BAUD_921K:
            relayRate = B921600;
            break;
        case BAUD_1000K:
            relayRate = B1000000;
            break;
        case BAUD_1152K:
            relayRate = B1152000;
            break;
        case BAUD_1500K:
            relayRate = B1500000;
            break;
        case BAUD_2000K:
            relayRate = B2000000;
            break;
#ifdef B2500000
        case BAUD_2500K:
            relayRate = B2500000;
            break;
#endif
#ifdef B3000000
        case BAUD_3000K:
            relayRate = B3000000;
            break;
#endif
#ifdef B3500000
        case BAUD_3500K:
            relayRate = B3500000;
            break;
#endif
#ifdef B4000000
        case BAUD_4000K:
            relayRate = B4000000;
            break;
#endif
#endif
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(baud));
            break;
    }

    struct termios newtio;

    stat = tcgetattr(fd, &newtio);
    if (-1 == stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // CS8 = 8 data bits, CLOCAL = Local line, CREAD = Enable Receiver
    /*
      Even parity (7E1):
      options.c_cflag |= PARENB
      options.c_cflag &= ~PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
      Odd parity (7O1):
      options.c_cflag |= PARENB
      options.c_cflag |= PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
     */
    newtio.c_cflag |= CS8 | CLOCAL | CREAD;

    switch (parity) {
        case PARITY_ODD:
            newtio.c_cflag |= (PARENB | PARODD);
            break;
        case PARITY_EVEN:
            newtio.c_cflag |= PARENB;
            break;
        case PARITY_NONE:
            newtio.c_cflag &= static_cast<unsigned int>(~PARENB);
            break;
        default:
            FW_ASSERT(0, parity);
            break;
    }

    // Set baud rate:
    stat = cfsetispeed(&newtio, static_cast<speed_t>(relayRate));
    if (stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }
    stat = cfsetospeed(&newtio, static_cast<speed_t>(relayRate));
    if (stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // Raw output:
    newtio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;

    newtio.c_iflag = INPCK;

    // Flush old data:
    (void)tcflush(fd, TCIFLUSH);

    // Set attributes:
    stat = tcsetattr(fd, TCSANOW, &newtio);
    if (-1 == stat) {
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // All done!
    Fw::LogStringArg _arg = device;
    this->log_ACTIVITY_HI_PortOpened(_arg);
    if (this->isConnected_ready_OutputPort(0)) {
        this->ready_out(0); // Indicate the driver is connected
    }
    return true;
}

LinuxUartDriver ::~LinuxUartDriver() {
    if (this->m_fd != -1) {
        (void)close(this->m_fd);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void LinuxUartDriver ::send_handler(const FwIndexType portNum, Fw::Buffer& serBuffer) {
    Drv::ByteStreamStatus status = Drv::ByteStreamStatus::OP_OK;
    if (this->m_fd == -1 || serBuffer.getData() == nullptr || serBuffer.getSize() == 0) {
        status = Drv::ByteStreamStatus::OTHER_ERROR;
    } else {
        unsigned char *data = serBuffer.getData();
        FW_ASSERT_NO_OVERFLOW(serBuffer.getSize(), size_t);
        size_t xferSize = static_cast<size_t>(serBuffer.getSize());

        ssize_t stat = ::write(this->m_fd, data, xferSize);

        if (-1 == stat || static_cast<size_t>(stat) != xferSize) {
          Fw::LogStringArg _arg = this->m_device;
          this->log_WARNING_HI_WriteError(_arg, static_cast<I32>(stat));
          status = Drv::ByteStreamStatus::OTHER_ERROR;
        }
    }
    // Return the buffer back to the caller
    sendReturnOut_out(0, serBuffer, status);
}


void LinuxUartDriver::recvReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->deallocate_out(0, fwBuffer);
}

void LinuxUartDriver ::serialReadTaskEntry(void* ptr) {
    FW_ASSERT(ptr != nullptr);
    Drv::ByteStreamStatus status = ByteStreamStatus::OTHER_ERROR;  // added by m.chase 03.06.2017
    LinuxUartDriver* comp = reinterpret_cast<LinuxUartDriver*>(ptr);
    while (!comp->m_quitReadThread) {
        Fw::Buffer buff = comp->allocate_out(0,comp->m_allocationSize);

        // On failed allocation, error
        if (buff.getData() == nullptr) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_NoBuffers(_arg);
            status = ByteStreamStatus::OTHER_ERROR;
            comp->recv_out(0, buff, status);
            // to avoid spinning, wait 50 ms
            Os::Task::delay(Fw::TimeInterval(0, 50000));
            continue;
        }

        int stat = 0;

        // Read until something is received or an error occurs. Only loop when
        // stat == 0 as this is the timeout condition and the read should spin
        FW_ASSERT_NO_OVERFLOW(buff.getSize(), size_t);
        while ((stat == 0) && !comp->m_quitReadThread) {
            stat = static_cast<int>(::read(comp->m_fd, buff.getData(), static_cast<size_t>(buff.getSize())));
        }
        buff.setSize(0);

        // On error stat (-1) must mark the read as error
        // On normal stat (>0) pass a recv ok
        // On timeout stat (0) and m_quitReadThread, error to return the buffer
        if (stat == -1) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_ReadError(_arg, stat);
            status = ByteStreamStatus::OTHER_ERROR;
        } else if (stat > 0) {
            buff.setSize(static_cast<U32>(stat));
            status = ByteStreamStatus::OP_OK;  // added by m.chase 03.06.2017
        } else {
            status = ByteStreamStatus::OTHER_ERROR; // Simply to return the buffer
        }
        comp->recv_out(0, buff, status);  // added by m.chase 03.06.2017
    }
}

void LinuxUartDriver ::start(FwTaskPriorityType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity) {
    Os::TaskString task("SerReader");
    Os::Task::Arguments arguments(task, serialReadTaskEntry, this, priority, stackSize, cpuAffinity);
    Os::Task::Status stat = this->m_readTask.start(arguments);
    FW_ASSERT(stat == Os::Task::OP_OK, stat);
}

void LinuxUartDriver ::quitReadThread() {
    this->m_quitReadThread = true;
}

Os::Task::Status LinuxUartDriver ::join() {
    return m_readTask.join();
}

}  // end namespace Drv
