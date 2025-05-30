// ======================================================================
// \title  FatalHandlerImpl.cpp
// \author tcanham
// \brief  cpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <taskLib.h>
#include <Fw/Logger/Logger.hpp>

namespace Svc {

    void FatalHandlerComponentImpl::FatalReceive_handler(
            const FwIndexType portNum,
            FwEventIdType Id) {
        Fw::Logger::log("FATAL %d handled.\n",Id,0,0,0,0,0);
        taskSuspend(0);
    }

} // end namespace Svc
