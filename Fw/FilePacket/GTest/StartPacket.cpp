// ======================================================================
// \title  Fw/FilePacket/GTest/StartPacket.cpp
// \author bocchino
// \brief  Test utilities for start file packets
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FilePacket/GTest/FilePackets.hpp>
#include <Fw/Types/GTest/Bytes.hpp>

namespace Fw {

  namespace GTest {

    void FilePackets::StartPacket ::
      compare(
          const FilePacket::StartPacket& expected,
          const FilePacket::StartPacket& actual
      )
    {
      FilePackets::Header::compare(expected.asHeader(), actual.asHeader());
      ASSERT_EQ(expected.getFileSize(), actual.getFileSize());
      PathName::compare(expected.getSourcePath(), actual.getSourcePath());
      PathName::compare(expected.getDestinationPath(), actual.getDestinationPath());
    }

  }

}
