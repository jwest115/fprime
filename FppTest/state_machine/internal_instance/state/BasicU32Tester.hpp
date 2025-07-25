// ======================================================================
// \title  BasicU32Tester.hpp
// \author bocchino
// \brief  hpp file for BasicU32Tester component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicU32_HPP
#define FppTest_SmInstanceState_BasicU32_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicU32ComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicU32Tester : public BasicU32ComponentBase {
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The history size
    static constexpr FwSizeType historySize = 10;

    //! The queue depth
    static constexpr FwSizeType queueDepth = 10;

    //! The instance ID
    static constexpr FwEnumStoreType instanceId = 0;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicU32Tester object
    BasicU32Tester(const char* const compName  //!< The component name
    );

    //! Destroy BasicU32Tester object
    ~BasicU32Tester();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_BasicU32
    using SmState_BasicU32 = FppTest_SmState_BasicU32;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicU32
    //!
    //! Action a
    void FppTest_SmState_BasicU32_action_a(SmId smId,                               //!< The state machine id
                                           FppTest_SmState_BasicU32::Signal signal  //!< The signal
                                           ) override;

    //! Implementation for action b of state machine FppTest_SmState_BasicU32
    //!
    //! Action b
    void FppTest_SmState_BasicU32_action_b(SmId smId,                                //!< The state machine id
                                           FppTest_SmState_BasicU32::Signal signal,  //!< The signal
                                           U32 value                                 //!< The value
                                           ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test
    void test();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smStateBasicU32
    SmHarness::History<SmState_BasicU32::Signal, historySize> m_smStateBasicU32_action_a_history;

    //! The history associated with action b of smStateBasicU32
    SmHarness::SignalValueHistory<SmState_BasicU32::Signal, U32, historySize> m_smStateBasicU32_action_b_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
