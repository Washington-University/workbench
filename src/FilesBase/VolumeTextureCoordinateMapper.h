#ifndef __VOLUME_TEXTURE_COORDINATE_MAPPER_H__
#define __VOLUME_TEXTURE_COORDINATE_MAPPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"

namespace caret {
    class LinearEquationTransform;
    class Vector3D;
    class VolumeMappableInterface;

    class VolumeTextureCoordinateMapper : public CaretObject {
        
    public:
        VolumeTextureCoordinateMapper(const VolumeMappableInterface* volumeInterface);
        
        virtual ~VolumeTextureCoordinateMapper();
        
        VolumeTextureCoordinateMapper(const VolumeTextureCoordinateMapper&) = delete;

        VolumeTextureCoordinateMapper& operator=(const VolumeTextureCoordinateMapper&) = delete;

        bool isValid() const;
        
        Vector3D mapXyzToStr(const Vector3D& xyz) const;

        void testMapXyzToStr(const AString& prefixText,
                             const Vector3D& xyz) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        const VolumeMappableInterface* m_volumeInterface;
        
        std::unique_ptr<LinearEquationTransform> m_transforms[3];
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_TEXTURE_COORDINATE_MAPPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_TEXTURE_COORDINATE_MAPPER_DECLARE__

} // namespace
#endif  //__VOLUME_TEXTURE_COORDINATE_MAPPER_H__
