/*
 * LogBuffer.hpp
 *
 *  Created on: Sep 10, 2012
 *      Author: ppandian
 */

/*
 * Description:
 * This object contains the LogBuffer type, used for storing log entries
 */
#ifndef FW_LOG_BUFFER_HPP
#define FW_LOG_BUFFER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/SerIds.hpp>

namespace Fw {

    class LogBuffer final : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_LOG_BUFF,
                SERIALIZED_SIZE = FW_LOG_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            LogBuffer(const U8 *args, FwSizeType size);
            LogBuffer();
            LogBuffer(const LogBuffer& other);
            virtual ~LogBuffer();
            LogBuffer& operator=(const LogBuffer& other);

            FwSizeType getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;

        private:
            U8 m_bufferData[FW_LOG_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
