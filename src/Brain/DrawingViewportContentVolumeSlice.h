#ifndef __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE__
#define __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE__

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
#include "Plane.h"
#include "Vector3D.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class DrawingViewportContentVolumeSlice : public CaretObject {
        
    public:
        DrawingViewportContentVolumeSlice();
        
        DrawingViewportContentVolumeSlice(const int32_t numberOfRows,
                                          const int32_t numberOfColumns,
                                          const int32_t rowIndex,
                                          const int32_t columnIndex,
                                          const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                          const Plane& plane,
                                          const Vector3D& sliceXYZ);

        virtual ~DrawingViewportContentVolumeSlice();
        
        DrawingViewportContentVolumeSlice(const DrawingViewportContentVolumeSlice& obj);

        DrawingViewportContentVolumeSlice& operator=(const DrawingViewportContentVolumeSlice& obj);
        
        int32_t getNumberOfRows() const;

        int32_t getNumberOfColumns() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        VolumeSliceViewPlaneEnum::Enum getVolumeSliceViewPlane() const;
        
        Plane getPlane() const;
        
        Vector3D getSliceXYZ() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperDrawingViewportContentVolumeSlice(const DrawingViewportContentVolumeSlice& obj);

        int32_t m_numberOfRows = -1;
        
        int32_t m_numberOfColumns = -1;
        
        int32_t m_rowIndex = -1;
        
        int32_t m_columnIndex = -1;
        
        VolumeSliceViewPlaneEnum::Enum m_slicePlane = VolumeSliceViewPlaneEnum::ALL;
        
        Plane m_plane;
        
        Vector3D m_sliceXYZ;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE__
