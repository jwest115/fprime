module Svc {
module CCSDS {
    @ Deframer for the CCSDS Space Packet protocol
    passive component SpacePacketDeframer {

        include "../../Interfaces/DeframerInterface.fppi"

        @ Port to validate a received sequence count for a given APID
        output port validateApidSeqCount: CCSDS.ApidSequenceCount

        @ Deframing received an invalid frame length
        event InvalidLength(transmitted: U16, actual: U32) \
            severity warning high \
            format "Invalid length received. Header specified packet byte size of {} | Actual received data length: {}"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut

    }

} # end CCSDS
} # end Svc
