#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <param_testerGTestBase.hpp>
#endif
#include "TesterBase.hpp"
#include <Fw/FPrimeBasicTypes.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public Prm::ParamTesterGTestBase {
    public:
        ATester() : Prm::ParamTesterGTestBase("comp",10) {
    }

};

int main(int argc, char* argv[]) {

    ATester testBase;

}
