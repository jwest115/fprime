#ifndef FW_COMP_BASE_HPP
#define FW_COMP_BASE_HPP

#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {

    class PassiveComponentBase : public Fw::ObjBase {
        public:
            //! Set the ID base
            void setIdBase(
                const FwBaseIdType //< The new ID base
            );
            //! Get the ID base
            //! \return The ID base
            FwBaseIdType getIdBase() const;

        PROTECTED:
            PassiveComponentBase(const char* name); //!< Named constructor
            virtual ~PassiveComponentBase(); //!< Destructor
            void init(FwEnumStoreType instance); //!< Initialization function
            FwEnumStoreType getInstance() const;


#if FW_OBJECT_TO_STRING == 1
            virtual const char* getToStringFormatString(); //!< Return the format  for a generic component toString
            void toString(char* str, FwSizeType size) override; //!< returns string description of component
#endif
        PRIVATE:
            FwBaseIdType m_idBase; //!< ID base for opcodes etc.
            FwEnumStoreType m_instance; //!< instance of component object


    };

}

#endif
