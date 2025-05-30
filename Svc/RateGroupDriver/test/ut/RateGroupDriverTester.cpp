/*
 * RateGroupDriverTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/RateGroupDriver/test/ut/RateGroupDriverImplTester.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::RateGroupDriver& impl, Svc::RateGroupDriverImplTester& tester) {

    for(FwIndexType i=0; i<Svc::RateGroupDriver::DIVIDER_SIZE; i++)
    {
        impl.set_CycleOut_OutputPort(i,tester.get_from_CycleOut(i));
    }

    tester.connect_to_CycleIn(0,impl.get_CycleIn_InputPort(0));
#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(RateGroupDriverTest,NominalSchedule) {

    Svc::RateGroupDriver::DividerSet dividersSet{};
    for(FwIndexType i=0; i<static_cast<FwIndexType>(Svc::RateGroupDriver::DIVIDER_SIZE); i++)
    {
        dividersSet.dividers[i] = {static_cast<FwSizeType>(i+1), static_cast<FwSizeType>(i%2)};
    }

    Svc::RateGroupDriver impl("RateGroupDriver");
    impl.configure(dividersSet);

    Svc::RateGroupDriverImplTester tester(impl);

    tester.init();
    impl.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runSchedNominal(dividersSet,FW_NUM_ARRAY_ELEMENTS(dividersSet.dividers));

}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
