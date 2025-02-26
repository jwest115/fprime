
type FrequencyType = U32
type PhaseType = F32

@ Signal Generator Settings
async command Settings(
                                  Frequency: FrequencyType
                                  Amplitude: F32
                                  Phase: PhaseType
                                  SigType: Ref.SignalType
                                ) \
  opcode 0x00

@ Toggle Signal Generator On/Off.
async command Toggle \
  opcode 1

@ Skip next sample
async command Skip \
  opcode 2

enum DpReqType {
    IMMEDIATE
    ASYNC
}


@ Signal Generator Settings
async command Dp(
                   reqType: DpReqType
                   records: U32
                   $priority: U32
                  ) \
  opcode 3

