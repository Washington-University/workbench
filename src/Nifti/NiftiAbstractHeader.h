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
       m_niftiVersion = 1;//default to version 1
   }

   virtual ~NiftiAbstractHeader(void)
   {
   }
   
   HeaderType getType() {
        switch (m_niftiVersion)
        {
        case 1:
            return AbstractHeader::NIFTI1;
        case 2:
            return AbstractHeader::NIFTI2;
        default:
            CaretAssert(false);
            throw NiftiException("programmer error, nifti extension version set incorrectly");
        }
   }
   Nifti1Header n1header;
   Nifti2Header n2header;
   int m_niftiVersion;
};

}

#endif //NIFTI_ABSTRACT_HEADER
