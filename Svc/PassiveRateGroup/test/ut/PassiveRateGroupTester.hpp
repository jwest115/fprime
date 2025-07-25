/*
* \author Tim Canham
* \file
* \brief
*
* This file is the test component header for the active rate group unit test.
*
* Code Generated Source Code Header
*
*   Copyright 2014-2015, by the California Institute of Technology.
*   ALL RIGHTS RESERVED. United States Government Sponsorship
*   acknowledged.
*/

#ifndef PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_
#define PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_

#include <PassiveRateGroupGTestBase.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroup.hpp>

namespace Svc {

    class PassiveRateGroupTester: public PassiveRateGroupGTestBase {
        public:
            PassiveRateGroupTester(Svc::PassiveRateGroup& inst);
            virtual ~PassiveRateGroupTester();

            void runNominal(U32 contexts[], FwIndexType numContexts, FwEnumStoreType instance);

            static FwSizeType getNumRateGroupMemberOutPorts() {
                return Svc::PassiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS;
            }


        private:

            void from_RateGroupMemberOut_handler(FwIndexType portNum, U32 context);

            Svc::PassiveRateGroup& m_impl;

            void clearPortCalls();

            struct {
                bool portCalled;
                U32 contextVal;
                FwIndexType order;
            } m_callLog[Svc::PassiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];

            FwIndexType m_callOrder; //!< tracks order of port call.

        };

} /* namespace Svc */

#endif /* PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_ */
