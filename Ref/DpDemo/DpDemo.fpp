module Ref {
    @ DP Demo
    active component DpDemo {

        enum ColorEnum {
            RED
            GREEN
            BLUE
        }

        struct ColorInfoStruct {
            Color: ColorEnum
            # TODO: add more things here
        }

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ Select color
        async command SelectColor(color: ColorEnum) opcode 0

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        @ Command for generating a DP
        sync command Dp(records: U32, $priority: U32)

        @ DP counter
        telemetry DpCounter: U64

        @ Current color selected
        telemetry Color: ColorEnum
        
        @ Color selected event
        event ColorSelected(color: ColorEnum) severity activity high id 0 format "Color selected {}"

        @ DP started event
        event DpStarted(records: U32) \
            severity activity low \
            id 1 \
            format "Writing {} DP records"

        @ DP complete event
        event DpComplete(records: U32, bytes: U32) \
            severity activity low \
            id 2 \
            format "Writing {} DP records {} bytes total"

        event DpRecordFull(records: U32, bytes: U32) \
            severity warning low \
            id 3 \
            format "DP container full with {} records and {} bytes. Closing DP."

        event DpMemRequested($size: FwSizeType) \
            severity activity low \
            id 7 \
            format "Requesting {} bytes for DP"

        event DpMemReceived($size: FwSizeType) \
            severity activity low \
            id 8 \
            format "Received {} bytes for DP"

        event DpMemoryFail \
            severity warning high \
            id 5 \
            format "Failed to acquire a DP buffer"

        @ Example port: receiving calls from the rate group
        sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

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

        @ Data product get port
        product get port productGetOut

        @ Data product send port
        product send port productSendOut

        @ Data product record
        product record ColorDataRecord: ColorInfoStruct id 0

        @ Data product container
        product container ColorDataContainer id 0 default priority 10

    }
}
