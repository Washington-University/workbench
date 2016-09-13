#ifndef __SELECTION_ITEM_IMAGE_CONTROL_POINT_H_
#define __SELECTION_ITEM_IMAGE_CONTROL_POINT_H_

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


#include "SelectionItem.h"

namespace caret {

    class ControlPointFile;
    class ControlPoint3D;
    class ImageFile;
    
    class SelectionItemImageControlPoint : public SelectionItem {
        
    public:
        SelectionItemImageControlPoint();
        
        virtual ~SelectionItemImageControlPoint();
        
        virtual bool isValid() const;
        
        ImageFile* getImageFile();
        
        const ImageFile* getImageFile() const;

        void setImageFile(ImageFile* imageFile);
        
        ControlPointFile* getControlPointFile();
        
        const ControlPointFile* getControlPointFile() const;
        
        void setControlPointFile(ControlPointFile* imageFile);
        
        int32_t getControlPointIndexInFile() const;
        
        void setControlPointIndexInFile(const int32_t controlPointIndexInFile);
        
        ControlPoint3D* getControlPoint();
        
        const ControlPoint3D* getControlPoint() const;
        
        void setControlPoint(ControlPoint3D* controlPoint);
        
        virtual void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemImageControlPoint(const SelectionItemImageControlPoint&);

        SelectionItemImageControlPoint& operator=(const SelectionItemImageControlPoint&);

        ImageFile* m_imageFile;
        
        ControlPointFile* m_controlPointFile;
        
        ControlPoint3D* m_controlPoint;
        
        int32_t m_controlPointIndexInFile;
    };
    
#ifdef __SELECTION_ITEM_IMAGE_CONTROL_POINT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_IMAGE_CONTROL_POINT_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_IMAGE_CONTROL_POINT_H_
