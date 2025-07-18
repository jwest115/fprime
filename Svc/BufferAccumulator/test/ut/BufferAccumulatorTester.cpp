// ======================================================================
// \title  BufferAccumulatorTester.hpp
// \author bocchino, mereweth
// \brief  BufferAccumulator test harness implementation
//
// \copyright
// Copyright 2009-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "BufferAccumulatorTester.hpp"

#include "Fw/Types/BasicTypes.hpp"

#include "Fw/Types/MallocAllocator.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 30
#define QUEUE_DEPTH 30

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

BufferAccumulatorTester ::BufferAccumulatorTester(bool a_doAllocateQueue)
    :
#if FW_OBJECT_NAMES == 1
      BufferAccumulatorGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferAccumulator"),
#else
      BufferAccumulatorGTestBase(MAX_HISTORY_SIZE),
      component(),
#endif
      doAllocateQueue(a_doAllocateQueue) {
  this->initComponents();
  this->connectPorts();

  // Witch to BufferAccumulator_OpState::DRAIN at start so we don't have to
  // change ut
  component.m_mode = BufferAccumulator_OpState::DRAIN;
  component.m_send = true;

  if (this->doAllocateQueue) {
    Fw::MallocAllocator buffAccumMallocator;
    this->component.allocateQueue(0, buffAccumMallocator, MAX_NUM_BUFFERS);
  }
}

BufferAccumulatorTester ::~BufferAccumulatorTester() {
  if (this->doAllocateQueue) {
    Fw::MallocAllocator buffAccumMallocator;
    this->component.deallocateQueue(buffAccumMallocator);
  }
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void BufferAccumulatorTester ::from_bufferSendOutDrain_handler(const FwIndexType portNum,
                                              Fw::Buffer& fwBuffer) {
  this->pushFromPortEntry_bufferSendOutDrain(fwBuffer);
}

void BufferAccumulatorTester ::from_bufferSendOutReturn_handler(const FwIndexType portNum,
                                               Fw::Buffer& fwBuffer) {
  this->pushFromPortEntry_bufferSendOutReturn(fwBuffer);
}

void BufferAccumulatorTester ::from_pingOut_handler(const FwIndexType portNum, U32 key) {
  this->pushFromPortEntry_pingOut(key);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void BufferAccumulatorTester ::connectPorts() {
  // bufferSendInFill
  this->connect_to_bufferSendInFill(
      0, this->component.get_bufferSendInFill_InputPort(0));

  // bufferSendInReturn
  this->connect_to_bufferSendInReturn(
      0, this->component.get_bufferSendInReturn_InputPort(0));

  // cmdIn
  this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

  // pingIn
  this->connect_to_pingIn(0, this->component.get_pingIn_InputPort(0));

  // bufferSendOutDrain
  this->component.set_bufferSendOutDrain_OutputPort(
      0, this->get_from_bufferSendOutDrain(0));

  // bufferSendOutReturn
  this->component.set_bufferSendOutReturn_OutputPort(
      0, this->get_from_bufferSendOutReturn(0));

  // cmdRegOut
  this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

  // cmdResponseOut
  this->component.set_cmdResponseOut_OutputPort(
      0, this->get_from_cmdResponseOut(0));

  // eventOut
  this->component.set_eventOut_OutputPort(0, this->get_from_eventOut(0));

  // eventOutText
  this->component.set_eventOutText_OutputPort(0,
                                              this->get_from_eventOutText(0));
  // pingOut
  this->component.set_pingOut_OutputPort(0, this->get_from_pingOut(0));

  // timeCaller
  this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));

  // tlmOut
  this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));
}

void BufferAccumulatorTester ::initComponents() {
  this->init();
  this->component.init(QUEUE_DEPTH, INSTANCE);
}

void BufferAccumulatorTester ::doDispatch() {
  this->component.doDispatch();
}

}  // end namespace Svc
