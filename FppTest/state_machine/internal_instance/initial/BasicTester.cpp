// ======================================================================
// \title  BasicTester.cpp
// \author bocchino
// \brief  cpp file for BasicTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/initial/BasicTester.hpp"

namespace FppTest {

namespace SmInstanceInitial {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicTester::BasicTester(const char* const compName)
    : BasicComponentBase(compName), m_basic1_action_a_history(), m_smInitialBasic1_action_a_history() {}

BasicTester::~BasicTester() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void BasicTester::schedIn_handler(FwIndexType portNum, U32 context) {
    // Nothing to do
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicTester::FppTest_SmInitial_Basic_action_a(SmId smId, FppTest_SmInitial_Basic::Signal signal) {
    ASSERT_TRUE((smId == SmId::smInitialBasic1) || (smId == SmId::smInitialBasic2));
    if (smId == SmId::smInitialBasic1) {
        this->m_smInitialBasic1_action_a_history.push(signal);
    } else {
        this->m_smInitialBasic2_action_a_history.push(signal);
    }
}

void BasicTester::FppTest_SmInstanceInitial_Basic_Basic_action_a(SmId smId,
                                                                 FppTest_SmInstanceInitial_Basic_Basic::Signal signal) {
    ASSERT_TRUE((smId == SmId::basic1) || (smId == SmId::basic2));
    if (smId == SmId::basic1) {
        this->m_basic1_action_a_history.push(signal);
    } else {
        this->m_basic2_action_a_history.push(signal);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicTester::test() {
    this->m_basic1_action_a_history.clear();
    this->m_smInitialBasic1_action_a_history.clear();
    ASSERT_EQ(this->basic1_getState(), Basic_Basic::State::__FPRIME_AC_UNINITIALIZED);
    ASSERT_EQ(this->smInitialBasic1_getState(), SmInitial_Basic::State::__FPRIME_AC_UNINITIALIZED);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->basic1_getState(), Basic_Basic::State::S);
    ASSERT_EQ(this->smInitialBasic1_getState(), SmInitial_Basic::State::S);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_basic1_action_a_history.getSize(), expectedSize);
    ASSERT_EQ(this->m_basic2_action_a_history.getSize(), expectedSize);
    ASSERT_EQ(this->m_smInitialBasic1_action_a_history.getSize(), expectedSize);
    ASSERT_EQ(this->m_smInitialBasic2_action_a_history.getSize(), expectedSize);
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(this->m_basic1_action_a_history.getItemAt(i), Basic_Basic::Signal::__FPRIME_AC_INITIAL_TRANSITION);
        ASSERT_EQ(this->m_basic2_action_a_history.getItemAt(i), Basic_Basic::Signal::__FPRIME_AC_INITIAL_TRANSITION);
        ASSERT_EQ(this->m_smInitialBasic1_action_a_history.getItemAt(i),
                  SmInitial_Basic::Signal::__FPRIME_AC_INITIAL_TRANSITION);
        ASSERT_EQ(this->m_smInitialBasic2_action_a_history.getItemAt(i),
                  SmInitial_Basic::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
}

}  // namespace SmInstanceInitial

}  // namespace FppTest
