#ifndef __OME_COORDINATE_TRANSFORMATIONS_H__
#define __OME_COORDINATE_TRANSFORMATIONS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <vector>

#include "CaretObject.h"
#include "OmeCoordinateTransformationTypeEnum.h"


namespace caret {

    class OmeCoordinateTransformations : public CaretObject {
        
    public:
        OmeCoordinateTransformations();
        
        OmeCoordinateTransformations(const OmeCoordinateTransformationTypeEnum::Enum type,
                                     const std::vector<float>& values);
        
        virtual ~OmeCoordinateTransformations();
        
        OmeCoordinateTransformations(const OmeCoordinateTransformations& obj);

        OmeCoordinateTransformations& operator=(const OmeCoordinateTransformations& obj);
        
        OmeCoordinateTransformationTypeEnum::Enum getType() const;
        
        std::vector<float> getTransformValues() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeCoordinateTransformations(const OmeCoordinateTransformations& obj);

        OmeCoordinateTransformationTypeEnum::Enum m_type = OmeCoordinateTransformationTypeEnum::INVALID;
        
        std::vector<float> m_transformValues;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_COORDINATE_TRANSFORMATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_COORDINATE_TRANSFORMATIONS_DECLARE__

} // namespace
#endif  //__OME_COORDINATE_TRANSFORMATIONS_H__
