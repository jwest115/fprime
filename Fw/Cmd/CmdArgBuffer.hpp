/*
 *
 *
 *  Created on: March 1, 2014
 *      Author: T. Canham
 */

/*
 * Description:
 * This object contains the CmdARgBuffer type, used for holding the serialized arguments of commands
 */
#ifndef FW_CMD_ARG_BUFFER_HPP
#define FW_CMD_ARG_BUFFER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/SerIds.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

class CmdArgBuffer final : public SerializeBufferBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_CMD_BUFF,                    //!< type id for CmdArgBuffer
        SERIALIZED_SIZE = FW_CMD_ARG_BUFFER_MAX_SIZE + sizeof(I32)  //!< size when serialized. Buffer + size of buffer
    };

    CmdArgBuffer(const U8* args, FwSizeType size);       //!< buffer source constructor
    CmdArgBuffer();                                      //!< default constructor
    CmdArgBuffer(const CmdArgBuffer& other);             //!< other arg buffer constructor
    virtual ~CmdArgBuffer();                             //!< destructor
    CmdArgBuffer& operator=(const CmdArgBuffer& other);  //!< Equal operator

    FwSizeType getBuffCapacity() const;  //!< return capacity of buffer (how much it can hold)
    U8* getBuffAddr();                   //!< return address of buffer (non const version)
    const U8* getBuffAddr() const;       //!< return address of buffer (const version)

  private:
    U8 m_bufferData[FW_CMD_ARG_BUFFER_MAX_SIZE];  //!< command argument buffer
};

}  // namespace Fw

#endif
