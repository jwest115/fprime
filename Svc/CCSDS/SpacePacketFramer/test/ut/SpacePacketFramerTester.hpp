// ======================================================================
// \title  SpacePacketFramerTester.hpp
// \author thomas-bc
// \brief  hpp file for SpacePacketFramer component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_SpacePacketFramerTester_HPP
#define Svc_CCSDS_SpacePacketFramerTester_HPP

#include "Svc/CCSDS/SpacePacketFramer/SpacePacketFramer.hpp"
#include "Svc/CCSDS/SpacePacketFramer/SpacePacketFramerGTestBase.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

class SpacePacketFramerTester final : public SpacePacketFramerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object SpacePacketFramerTester
    SpacePacketFramerTester();

    //! Destroy object SpacePacketFramerTester
    ~SpacePacketFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testDataReturnPassthrough();
    void testNominalFraming();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    // ----------------------------------------------------------------------
    // Test Harness: output port overrides
    // ----------------------------------------------------------------------
    U16 from_getApidSeqCount_handler(FwIndexType portNum,  //!< The port number
                                     const ComCfg::APID& apid,
                                     U16 sequenceCount) override;

    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum, U32 size) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SpacePacketFramer component;

    //! Test buffer
    static const FwSizeType MAX_TEST_PACKET_DATA_SIZE = 200;
    U8 m_internalDataBuffer[SpacePacketHeader::SERIALIZED_SIZE + MAX_TEST_PACKET_DATA_SIZE];

    U16 m_nextSeqCount;  // Sequence count to be returned by getApidSeqCount output port
};

}  // namespace CCSDS

}  // namespace Svc

#endif
