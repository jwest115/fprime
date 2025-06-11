// ======================================================================
// \title  SpacePacketFramer.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketFramer component implementation class
// ======================================================================

#include "Svc/CCSDS/SpacePacketFramer/SpacePacketFramer.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpacePacketFramer ::SpacePacketFramer(const char* const compName) : SpacePacketFramerComponentBase(compName) {}

SpacePacketFramer ::~SpacePacketFramer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void SpacePacketFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    SpacePacketHeader header;
    Fw::SerializeStatus status;
    FwSizeType frameSize = SpacePacketHeader::SERIALIZED_SIZE + data.getSize();
    FW_ASSERT(data.getSize() <= std::numeric_limits<Fw::Buffer::SizeType>::max() - SpacePacketHeader::SERIALIZED_SIZE, static_cast<FwAssertArgType>(data.getSize()));

    // Allocate frame buffer
    Fw::Buffer frameBuffer = this->bufferAllocate_out(0, static_cast<Fw::Buffer::SizeType>(frameSize));
    auto frameSerializer = frameBuffer.getSerializer();

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    // PVN is always 0 per Standard - Packet Type is 0 for Telemetry (downlink) - SecHdr flag is 0 for no secondary header
    U16 packetIdentification = 0;
    ComCfg::APID::T apid = context.getapid();
    FW_ASSERT((apid >> SpacePacketSubfields::ApidWidth) == 0, static_cast<FwAssertArgType>(apid)); // apid must fit in 11 bits
    packetIdentification |= static_cast<U16>(apid) & SpacePacketSubfields::ApidMask; // 11 bit APID

    U16 sequenceCount = this->getApidSeqCount_out(0, apid, 0); // retrieve the sequence count for this APID
    U16 packetSequenceControl = 0;
    packetSequenceControl |= 0x3 << SpacePacketSubfields::SeqFlagsOffset; // Sequence Flags 0b11 = unsegmented User Data
    packetSequenceControl |= sequenceCount & SpacePacketSubfields::SeqCountMask; // 14 bit sequence count

    FW_ASSERT(data.getSize() <= std::numeric_limits<U16>::max(), static_cast<FwAssertArgType>(data.getSize()));
    U16 packetDataLength = static_cast<U16>(data.getSize() - 1); // Standard specifies length is number of bytes minus 1

    header.setpacketIdentification(packetIdentification);
    header.setpacketSequenceControl(packetSequenceControl);
    header.setpacketDataLength(packetDataLength);

    // -----------------------------------------------
    // Serialize the packet
    // -----------------------------------------------
    status = frameSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->dataOut_out(0, frameBuffer, context);
    this->dataReturnOut_out(0, data, context); // return ownership of the original data buffer
}

void SpacePacketFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void SpacePacketFramer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& frameBuffer, const ComCfg::FrameContext& context) {
    // dataReturnIn is the allocated buffer coming back from the dataOut port
    this->bufferDeallocate_out(0, frameBuffer);
}


}  // namespace CCSDS
}  // namespace Svc
