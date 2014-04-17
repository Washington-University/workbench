#ifndef NIFTI_ABSTRACT_VOLUME_EXTENSION
#define NIFTI_ABSTRACT_VOLUME_EXTENSION

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

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
    ExtensionType getType() const
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
