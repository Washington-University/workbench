#ifndef __ANNOTATION_COLOR_BAR_SECTION_H__
#define __ANNOTATION_COLOR_BAR_SECTION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "CaretObject.h"



namespace caret {

    class AnnotationColorBarSection : public CaretObject {
        
    public:
        AnnotationColorBarSection(const float startScalar,
                                  const float endScalar,
                                  const float startRGBA[4],
                                  const float endRGBA[4]);
        
        virtual ~AnnotationColorBarSection();
        
        AnnotationColorBarSection(const AnnotationColorBarSection& obj);

        AnnotationColorBarSection& operator=(const AnnotationColorBarSection& obj);

        /** 
         * @return The starting scalar 
         */
        float getStartScalar() const { return m_startScalar; }
        
        /** 
         * @return The ending scalar 
         */
        float getEndScalar() const { return m_endScalar; }
        
        /**
         * @return Pointer to starting RGBA
         */
        const float* getStartRGBA() const { return m_startRGBA; }
        
        /** 
         * @return Pointer to ending RGBA 
         */
        const float* getEndRGBA() const { return m_endRGBA; }
        
    private:
        void copyHelperAnnotationColorBarSection(const AnnotationColorBarSection& obj);

        float m_startScalar;
        
        float m_endScalar;
        
        float m_startRGBA[4];
        
        float m_endRGBA[4];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COLOR_BAR_SECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COLOR_BAR_SECTION_DECLARE__

} // namespace
#endif  //__ANNOTATION_COLOR_BAR_SECTION_H__
