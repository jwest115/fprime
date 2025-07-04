/*
 * ActiveLoggerTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef ACTIVELOGGER_TEST_UT_ACTIVELOGGER_TESTER_HPP_
#define ACTIVELOGGER_TEST_UT_ACTIVELOGGER_TESTER_HPP_

#include <ActiveLoggerGTestBase.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Os/File.hpp>

namespace Svc {

    class ActiveLoggerTester: public Svc::ActiveLoggerGTestBase {
        public:
            explicit ActiveLoggerTester(Svc::ActiveLoggerImpl& inst);
            virtual ~ActiveLoggerTester();

            void runEventNominal();
            void runFilterEventNominal();
            void runFilterIdNominal();
            void runFilterDump();
            void runFilterInvalidCommands();
            void runEventFatal();
            void runFileDump();
            void runFileDumpErrors();

        private:

            void from_PktSend_handler(
                    const FwIndexType portNum, //!< The port number
                    Fw::ComBuffer &data, //!< Buffer containing packet data
                    U32 context //!< context (not used)
                ) override;
            void from_FatalAnnounce_handler(
                      const FwIndexType portNum, //!< The port number
                      FwEventIdType Id //!< The ID of the FATAL event
                  ) override;

            Svc::ActiveLoggerImpl& m_impl;

            bool m_receivedPacket;
            Fw::ComBuffer m_sentPacket;

            bool m_receivedFatalEvent;
            FwEventIdType m_fatalID;

            void runWithFilters(Fw::LogSeverity filter);

            void writeEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value);
            void readEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value, Os::File& file);

            // enumeration to tell what kind of error to inject
            typedef enum {
                FILE_WRITE_WRITE_ERROR, // return a bad read status
                FILE_WRITE_SIZE_ERROR, // return a bad size
            } FileWriteTestType;
            FileWriteTestType m_writeTestType;
            FwSizeType m_writeSize;

            void textLogIn(const FwEventIdType id, //!< The event ID
                      const Fw::Time& timeTag, //!< The time
                      const Fw::LogSeverity severity, //!< The severity
                      const Fw::TextLogString& text //!< The event string
                      ) override;

            //! Handler for from_pingOut
            //!
            void from_pingOut_handler(
                const FwIndexType portNum, /*!< The port number*/
                U32 key /*!< Value to return to pinger*/
            ) override;
    };

} /* namespace Svc */

#endif /* ACTIVELOGGER_TEST_UT_ACTIVELOGGER_TESTER_HPP_ */
