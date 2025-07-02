// ======================================================================
// \title  DpDemo.cpp
// \author jawest
// \brief  cpp file for DpDemo component implementation class
// ======================================================================

#include "Ref/DpDemo/DpDemo.hpp"

namespace Ref {

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    DpDemo ::DpDemo(const char* const compName) : DpDemoComponentBase(compName) {
        this->selectedColor = DpDemo_ColorEnum::RED;
        this->numDps = 0;
        this->currDp = 0;
        this->dpPriority = 0;

        this->tlmWrite_Color(this->selectedColor);
    }

    DpDemo ::~DpDemo() {}

    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    void DpDemo ::run_handler(FwIndexType portNum, U32 context) {
    // if a Data product is being generated, store a record
        if (this->dpInProgress) {
            DpDemo_ColorInfoStruct colorInfo(this->selectedColor);
            Fw::SerializeStatus stat = this->dpContainer.serializeRecord_ColorDataRecord(colorInfo);
            this->currDp++;
            this->dpBytes += DpDemo_ColorInfoStruct::SERIALIZED_SIZE;
            // check for full data product
            if (Fw::SerializeStatus::FW_SERIALIZE_NO_ROOM_LEFT == stat) {
                this->log_WARNING_LO_DpRecordFull(this->currDp, this->dpBytes);
                this->cleanupAndSendDp();
            } else if (this->currDp == this->numDps) { // if we reached the target number of DPs
                this->log_ACTIVITY_LO_DpComplete(this->numDps, this->dpBytes);
                this->cleanupAndSendDp();
            }

            // this->tlmWrite_DpBytes(this->m_dpBytes);
            // this->tlmWrite_DpRecords(this->m_currDp);
        }
    }

    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    void DpDemo ::SelectColor_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, Ref::DpDemo_ColorEnum color) {
        this->selectedColor = color;
        tlmWrite_Color(color);
        log_ACTIVITY_HI_ColorSelected(color);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void DpDemo ::Dp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 records, U32 priority) {
        // Get DP buffer, use sync request
        FwSizeType dpSize = records*(DpDemo_ColorInfoStruct::SERIALIZED_SIZE + sizeof(FwDpIdType));
        this->numDps = records;
        this->currDp = 0;
        this->dpPriority = static_cast<FwDpPriorityType>(priority);
        this->log_ACTIVITY_LO_DpMemRequested(dpSize);
        Fw::Success stat = this->dpGet_ColorDataContainer(dpSize, this->dpContainer);
        // make sure we got the memory we wanted
        if (Fw::Success::FAILURE == stat) {
            this->log_WARNING_HI_DpMemoryFail();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        } else {
            this->dpInProgress = true;
            this->log_ACTIVITY_LO_DpStarted(records);
            this->log_ACTIVITY_LO_DpMemReceived(this->dpContainer.getBuffer().getSize());
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            // override priority with requested priority
            this->dpContainer.setPriority(priority);
        }
    }

    // ----------------------------------------------------------------------
    // Handler implementations for data products
    // ----------------------------------------------------------------------

    void DpDemo ::cleanupAndSendDp() {
        this->dpSend(this->dpContainer);
        this->dpInProgress = false;
        this->dpBytes = 0;
        this->numDps = 0;
        this->currDp = 0;
    }

}  // namespace Ref
