// ======================================================================
// \title  FileUplink.cpp
// \author bocchino
// \brief  cpp file for FileUplink component implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/Com/ComPacket.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/FileUplink/FileUplink.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FileUplink ::FileUplink(const char* const name)
    : FileUplinkComponentBase(name),
      m_receiveMode(START),
      m_lastSequenceIndex(0),
      m_lastPacketWriteStatus(Os::File::MAX_STATUS),
      m_filesReceived(this),
      m_packetsReceived(this),
      m_warnings(this) {}

FileUplink ::~FileUplink() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FileUplink ::bufferSendIn_handler(const FwIndexType portNum, Fw::Buffer& buffer) {
    // If packet is too small to contain a packet type, log + deallocate and return
    if (buffer.getSize() < sizeof(FwPacketDescriptorType)) {
        this->log_WARNING_HI_InvalidPacketReceived(Fw::ComPacketType::FW_PACKET_UNKNOWN);
        this->bufferSendOut_out(0, buffer);
        return;
    }

    // Read the packet type from the packet buffer
    FwPacketDescriptorType packetType;
    Fw::SerializeStatus status = buffer.getDeserializer().deserialize(packetType);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // If packet type is not a file packet, log + deallocate and return
    if (packetType != Fw::ComPacketType::FW_PACKET_FILE) {
        this->log_WARNING_HI_InvalidPacketReceived(packetType);
        this->bufferSendOut_out(0, buffer);
        return;
    }

    // Deserialize the file packet contents into Fw::FilePacket (remove packet type token)
    Fw::Buffer packetBuffer(buffer.getData() + sizeof(packetType),
                            buffer.getSize() - static_cast<Fw::Buffer::SizeType>(sizeof(packetType)));
    Fw::FilePacket filePacket;
    status = filePacket.fromBuffer(packetBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DecodeError(status);
    } else {
        Fw::FilePacket::Type header_type = filePacket.asHeader().getType();
        switch (header_type) {
            case Fw::FilePacket::T_START:
                this->handleStartPacket(filePacket.asStartPacket());
                break;
            case Fw::FilePacket::T_DATA:
                this->handleDataPacket(filePacket.asDataPacket());
                break;
            case Fw::FilePacket::T_END:
                this->handleEndPacket(filePacket.asEndPacket());
                break;
            case Fw::FilePacket::T_CANCEL:
                this->handleCancelPacket();
                break;
            default:
                FW_ASSERT(0);
                break;
        }
    }
    this->bufferSendOut_out(0, buffer);
}

void FileUplink ::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

void FileUplink ::handleStartPacket(const Fw::FilePacket::StartPacket& startPacket) {
    // Clear all event throttles in preparation for new start packet
    this->log_WARNING_HI_FileWriteError_ThrottleClear();
    this->log_WARNING_HI_InvalidReceiveMode_ThrottleClear();
    this->log_WARNING_HI_PacketOutOfBounds_ThrottleClear();
    this->log_WARNING_HI_PacketOutOfOrder_ThrottleClear();
    this->m_packetsReceived.packetReceived();
    if (this->m_receiveMode != START) {
        this->m_file.osFile.close();
        this->m_warnings.invalidReceiveMode(Fw::FilePacket::T_START);
    }
    const Os::File::Status status = this->m_file.open(startPacket);
    if (status == Os::File::OP_OK) {
        this->goToDataMode();
    } else {
        this->m_warnings.fileOpen(this->m_file.name);
        this->goToStartMode();
    }
}

void FileUplink ::handleDataPacket(const Fw::FilePacket::DataPacket& dataPacket) {
    this->m_packetsReceived.packetReceived();
    if (this->m_receiveMode != DATA) {
        this->m_warnings.invalidReceiveMode(Fw::FilePacket::T_DATA);
        return;
    }

    const U32 sequenceIndex = dataPacket.asHeader().getSequenceIndex();

    // skip this packet if it is a duplicate and it has already been written
    if (this->m_lastPacketWriteStatus == Os::File::OP_OK && this->checkDuplicatedPacket(sequenceIndex)) {
        return;
    }

    this->checkSequenceIndex(sequenceIndex);
    const U32 byteOffset = dataPacket.getByteOffset();
    const U32 dataSize = dataPacket.getDataSize();
    if (byteOffset + dataSize > this->m_file.size) {
        this->m_warnings.packetOutOfBounds(sequenceIndex, this->m_file.name);
        return;
    }
    const Os::File::Status status = this->m_file.write(dataPacket.getData(), byteOffset, dataSize);
    if (status != Os::File::OP_OK) {
        this->m_warnings.fileWrite(this->m_file.name);
    }

    this->m_lastPacketWriteStatus = status;
}

void FileUplink ::handleEndPacket(const Fw::FilePacket::EndPacket& endPacket) {
    this->m_packetsReceived.packetReceived();
    if (this->m_receiveMode == DATA) {
        this->m_filesReceived.fileReceived();
        this->checkSequenceIndex(endPacket.asHeader().getSequenceIndex());
        this->compareChecksums(endPacket);
        this->log_ACTIVITY_HI_FileReceived(this->m_file.name);
    } else {
        this->m_warnings.invalidReceiveMode(Fw::FilePacket::T_END);
    }
    this->goToStartMode();
}

void FileUplink ::handleCancelPacket() {
    this->m_packetsReceived.packetReceived();
    this->log_ACTIVITY_HI_UplinkCanceled();
    this->goToStartMode();
}

void FileUplink ::checkSequenceIndex(const U32 sequenceIndex) {
    if (sequenceIndex != this->m_lastSequenceIndex + 1) {
        this->m_warnings.packetOutOfOrder(sequenceIndex, this->m_lastSequenceIndex);
    }
    this->m_lastSequenceIndex = sequenceIndex;
}

bool FileUplink ::checkDuplicatedPacket(const U32 sequenceIndex) {
    // check for duplicate packet
    if (sequenceIndex == this->m_lastSequenceIndex) {
        this->m_warnings.packetDuplicate(sequenceIndex);
        return true;
    }

    return false;
}

void FileUplink ::compareChecksums(const Fw::FilePacket::EndPacket& endPacket) {
    CFDP::Checksum computed, stored;
    this->m_file.getChecksum(computed);
    endPacket.getChecksum(stored);
    if (computed != stored) {
        this->m_warnings.badChecksum(computed.getValue(), stored.getValue());
    }
}

void FileUplink ::goToStartMode() {
    this->m_file.osFile.close();
    this->m_receiveMode = START;
    this->m_lastSequenceIndex = 0;
    this->m_lastPacketWriteStatus = Os::File::MAX_STATUS;
}

void FileUplink ::goToDataMode() {
    this->m_receiveMode = DATA;
    this->m_lastSequenceIndex = 0;
    this->m_lastPacketWriteStatus = Os::File::MAX_STATUS;
}

}  // namespace Svc
