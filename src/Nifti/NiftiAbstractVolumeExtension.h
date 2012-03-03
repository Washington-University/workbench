#ifndef NIFTI_ABSTRACT_VOLUME_EXTENSION
#define NIFTI_ABSTRACT_VOLUME_EXTENSION
#include "VolumeBase.h"
#include "NiftiException.h"
#include "CaretAssert.h"

namespace caret {
class NiftiAbstractVolumeExtension :
   public AbstractVolumeExtension
{
public:
    int m_niftiVersion;
    int m_ecode;

    NiftiAbstractVolumeExtension(void)
    {
        m_niftiVersion = 1;//default to version 1
        m_ecode = 0;//default to "unknown private" code
    }

    virtual ~NiftiAbstractVolumeExtension(void)
    {
    }
    ExtensionType getType()
    {
        switch (m_niftiVersion)
        {
        case 1:
            return AbstractVolumeExtension::NIFTI1;
        case 2:
            return AbstractVolumeExtension::NIFTI2;
        default:
            CaretAssert(false);
            throw NiftiException("programmer error, nifti extension version set incorrectly");
        }
    }

};

}
#endif //NIFTI_ABSTRACT_VOLUME_EXTENSION
