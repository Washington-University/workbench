#ifndef NIFTI_ABSTRACT_HEADER
#define NIFTI_ABSTRACT_HEADER

#include "VolumeBase.h"
#include "Nifti1Header.h"
#include "Nifti2Header.h"
namespace caret {
class NiftiAbstractHeader :
   public AbstractHeader
{
public:

   NiftiAbstractHeader(void)
   {
   }

   virtual ~NiftiAbstractHeader(void)
   {
   }
   HeaderType getType() {
       return type;
   }
   Nifti1Header n1header;
   Nifti2Header n2header;
};

}

#endif //NIFTI_ABSTRACT_HEADER