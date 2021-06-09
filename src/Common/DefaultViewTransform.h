#ifndef __DEFAULT_VIEW_TRANSFORM_H__
#define __DEFAULT_VIEW_TRANSFORM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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


#include <array>
#include <memory>

#include "CaretObject.h"



namespace caret {

    class DefaultViewTransform : public CaretObject {
        
    public:
        DefaultViewTransform();
        
        virtual ~DefaultViewTransform();
        
        DefaultViewTransform(const DefaultViewTransform& obj);

        DefaultViewTransform& operator=(const DefaultViewTransform& obj);

        void reset();
        
        float getScaling() const;
        
        void setScaling(const float scaling);
        
        std::array<float, 3> getTranslation() const;
        
        void setTranslation(const float x,
                            const float y,
                            const float z = 0.0);
        
        void setTranslation(const std::array<float, 3>& translation);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperDefaultViewTransform(const DefaultViewTransform& obj);

        float m_scaling = 1.0;
        
        std::array<float, 3> m_translation = { 0.0, 0.0, 0.0 };
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DEFAULT_VIEW_TRANSFORM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DEFAULT_VIEW_TRANSFORM_DECLARE__

} // namespace
#endif  //__DEFAULT_VIEW_TRANSFORM_H__
