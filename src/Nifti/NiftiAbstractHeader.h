#ifndef NIFTI_ABSTRACT_HEADER
#define NIFTI_ABSTRACT_HEADER

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
   
   HeaderType getType() const {
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
