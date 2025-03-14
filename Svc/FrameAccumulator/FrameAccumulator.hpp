// ======================================================================
// \title  FrameAccumulator.hpp
// \author mstarch
// \brief  hpp file for FrameAccumulator component implementation class
// ======================================================================

#ifndef Svc_FrameAccumulator_HPP
#define Svc_FrameAccumulator_HPP

#include "Fw/Types/MemAllocator.hpp"
#include "Svc/FrameAccumulator/FrameAccumulatorComponentAc.hpp"
#include "Svc/FrameAccumulator/FrameDetector.hpp"
#include "Utils/Types/CircularBuffer.hpp"

namespace Svc {

class FrameAccumulator final : public FrameAccumulatorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! \brief Construct FrameAccumulator object
    FrameAccumulator(const char* const compName  //!< The component name
    );

    //! \brief Destroy FrameAccumulator object
    ~FrameAccumulator();

    //! \brief configure memory allocation for the circular buffer
    //!
    //! Takes in parameters used in the Fw::MemAllocator pattern and configures a memory allocation for storing the
    //! circular buffer.
    void configure(const FrameDetector& detector,  //!< Frame detector helper instance
                   FwEnumStoreType allocationId,  //!< Identifier used  when dealing with the Fw::MemAllocator
                   Fw::MemAllocator& allocator,    //!< Fw::MemAllocator used to acquire memory
                   FwSizeType store_size           //!< Size to request for circular buffer
    );

    //! \brief Deallocate internal resources (set up by configure() call)
    void cleanup();


  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Receives raw data from a ByteStreamDriver, ComStub, or other buffer producing component
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& recvBuffer,
                        const Drv::RecvStatus& recvStatus) override;

  PRIVATE:
    //! \brief process raw buffer
    //! \return raw data buffer
    void processBuffer(Fw::Buffer& buffer);

    //! \brief process circular buffer
    void processRing();

    //! Circular buffer for storing data
    Types::CircularBuffer m_inRing;

    //! Pointer to helper class that detects frames
    FrameDetector const* m_detector;

    //! Memory allocator instance used with deallocating
    Fw::MemAllocator* m_memoryAllocator;

    //! Memory pointer for allocated memory
    U8* m_memory;

    //! Identification used with the memory allocator
    FwEnumStoreType m_allocatorId;
};

}  // namespace Svc

#endif
