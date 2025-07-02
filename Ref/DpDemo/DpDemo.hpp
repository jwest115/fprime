// ======================================================================
// \title  DpDemo.hpp
// \author jawest
// \brief  hpp file for DpDemo component implementation class
// ======================================================================

#ifndef Ref_DpDemo_HPP
#define Ref_DpDemo_HPP

#include "Ref/DpDemo/DpDemoComponentAc.hpp"

namespace Ref {

class DpDemo final : public DpDemoComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct DpDemo object
    DpDemo(const char* const compName  //!< The component name
    );

    //! Destroy DpDemo object
    ~DpDemo();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for run
    //!
    //! Example port: receiving calls from the rate group
    void run_handler(FwIndexType portNum,  //!< The port number
                     U32 context           //!< The call order
                     ) override;

    //! Handler implementation for command SelectColor
    //!
    //! Select color
    void SelectColor_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq,           //!< The command sequence number
                                Ref::DpDemo_ColorEnum color) override;

    //! Handler implementation for command Dp
    //!
    //! Command for generating a DP
    void Dp_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                       U32 cmdSeq,           //!< The command sequence number
                       U32 records,
                       U32 priority) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for data products
    // ----------------------------------------------------------------------

    // DP cleanup helper
    void cleanupAndSendDp();

    // Member variables
    DpDemo_ColorEnum selectedColor;
    U32 numDps;
    U32 currDp;
    U32 dpPriority;
    U32 dpBytes;
    DpContainer dpContainer;
    bool dpInProgress;
};

}  // namespace Ref

#endif
