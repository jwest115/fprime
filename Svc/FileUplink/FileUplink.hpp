// ======================================================================
// \title  FileUplink.hpp
// \author bocchino
// \brief  hpp file for FileUplink component implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_FileUplink_HPP
#define Svc_FileUplink_HPP

#include <Svc/FileUplink/FileUplinkComponentAc.hpp>
#include <Fw/FilePacket/FilePacket.hpp>
#include <Os/File.hpp>

namespace Svc {

  class FileUplink final :
    public FileUplinkComponentBase
  {

    friend class FileUplinkTester;

    private:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      //! The ReceiveMode type
      typedef enum { START, DATA } ReceiveMode;

      //! An object representing an incoming file
      class File {

        friend class FileUplinkTester;

        public:

          //! The file size
          U32 size;

          //! The file name
          Fw::LogStringArg name;

          //! The underlying OS file
          Os::File osFile;

        private:

          //! The checksum for the file
          ::CFDP::Checksum m_checksum;

        public:

          //! Open the OS file for writing and initialize the checksum
          Os::File::Status open(
              const Fw::FilePacket::StartPacket& startPacket
          );

          //! Write bytes into the OS file and update the checksum
          Os::File::Status write(
              const U8 *const data,
              const U32 byteOffset,
              const U32 length
          );

          //! Get the checksum
          void getChecksum(::CFDP::Checksum& checksum) {
            checksum = this->m_checksum;
          }

      };

      //! Object to record files received
      class FilesReceived {

        friend class FileUplinkTester;

        public:

          //! Construct a FilesReceived object
          FilesReceived(FileUplink *const fileUplink) :
            m_received_files_counter(0),
            m_fileUplink(fileUplink)
          { }

        public:

          //! Record a received file
          void fileReceived() {
            ++this->m_received_files_counter;
            this->m_fileUplink->tlmWrite_FilesReceived(m_received_files_counter);
          }

        private:

          //! The total number of files received
          U32 m_received_files_counter;

          //! The enclosing FileUplink object
          FileUplink *const m_fileUplink;

      };

      //! Object to record packets received
      class PacketsReceived {

        friend class FileUplinkTester;

        public:

          //! Construct a PacketsReceived object
          PacketsReceived(FileUplink *const fileUplink) :
            m_received_packet_count(0),
            m_fileUplink(fileUplink)
          { }

        public:

          //! Record a packet received
          void packetReceived() {
            ++this->m_received_packet_count;
            this->m_fileUplink->tlmWrite_PacketsReceived(m_received_packet_count);
          }

        private:

          //! The total number of received packets
          U32 m_received_packet_count;

          //! The enclosing FileUplink object
          FileUplink *const m_fileUplink;

      };

      //! Object to record warnings
      class Warnings {

        friend class FileUplinkTester;

        public:

          //! Construct a Warnings object
          Warnings(FileUplink *const fileUplink) :
            m_warning_count(0),
            m_fileUplink(fileUplink)
          { }

        public:

          //! Record an Invalid Receive Mode warning
          void invalidReceiveMode(const Fw::FilePacket::Type packetType);

          //! Record a File Open warning
          void fileOpen(Fw::LogStringArg& fileName);

          //! Record a Packet Out of Bounds warning
          void packetOutOfBounds(
              const U32 sequenceIndex,
              Fw::LogStringArg& fileName
          );

          //! Record a Packet Out of Order warning
          void packetOutOfOrder(
              const U32 sequenceIndex,
              const U32 lastSequenceIndex
          );

          //! Record a Duplicate Packet warning
          void packetDuplicate(
              const U32 sequenceIndex
          );

          //! Record a File Write warning
          void fileWrite(Fw::LogStringArg& fileName);

          //! Record a Bad Checksum warning
          void badChecksum(
              const U32 computed,
              const U32 read
          );

        private:

          //! Record a warning
          void warning() {
            ++this->m_warning_count;
            this->m_fileUplink->tlmWrite_Warnings(m_warning_count);
          }

        private:

          //! The total number of warnings
          U32 m_warning_count;

          //! The enclosing FileUplink object
          FileUplink *const m_fileUplink;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileUplink
      //!
      FileUplink(
          const char *const name //!< The component name
      );

      //! Destroy object FileUplink
      //!
      ~FileUplink();

    private:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for bufferSendIn
      //!
      void bufferSendIn_handler(
          const FwIndexType portNum, //!< The port number
          Fw::Buffer& buffer //!< Buffer wrapping data
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const FwIndexType portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );


    private:

      // ----------------------------------------------------------------------
      // Private helper functions
      // ----------------------------------------------------------------------

      //! Handle a start packet
      void handleStartPacket(const Fw::FilePacket::StartPacket& startPacket);

      //! Handle a data packet
      void handleDataPacket(const Fw::FilePacket::DataPacket& dataPacket);

      //! Handle an end packet
      void handleEndPacket(const Fw::FilePacket::EndPacket& endPacket);

      //! Handle a cancel packet
      void handleCancelPacket();

      //! Check sequence index
      void checkSequenceIndex(const U32 sequenceIndex);

      //! Check if a received packet is a duplicate
      bool checkDuplicatedPacket(const U32 sequenceIndex);

      //! Compare checksums
      void compareChecksums(const Fw::FilePacket::EndPacket& endPacket);

      //! Go to START mode
      void goToStartMode();

      //! Go to DATA mode
      void goToDataMode();

    private:

      // ----------------------------------------------------------------------
      // Member variables
      // ----------------------------------------------------------------------

      //! The receive mode
      ReceiveMode m_receiveMode;

      //! The sequence index of the last packet received
      U32 m_lastSequenceIndex;

      //! The write status of the last packet received
      Os::File::Status m_lastPacketWriteStatus;

      //! The file being assembled
      File m_file;

      //! The total number of files received
      FilesReceived m_filesReceived;

      //! The total number of cancel packets
      PacketsReceived m_packetsReceived;

      //! The total number of warnings
      Warnings m_warnings;

  };

}

#endif
