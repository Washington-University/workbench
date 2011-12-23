#ifndef NIFTI_ABSTRACT_VOLUME_EXTENSION
#define NIFTI_ABSTRACT_VOLUME_EXTENSION
#include "VolumeBase.h"

namespace caret {
class NiftiAbstractVolumeExtension :
   public AbstractVolumeExtension
{
public:

   NiftiAbstractVolumeExtension(void)
   {
   }

   virtual ~NiftiAbstractVolumeExtension(void)
   {
   }
   ExtensionType getType()
   {
       return type;
   }

};

}
#endif //NIFTI_ABSTRACT_VOLUME_EXTENSION
