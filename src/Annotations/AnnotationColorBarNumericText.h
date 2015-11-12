#ifndef __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_H__
#define __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_H__

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

#include "AnnotationTextAlignHorizontalEnum.h"
#include "CaretObject.h"

namespace caret {

    class AnnotationColorBarNumericText : public CaretObject {
        
    public:
        AnnotationColorBarNumericText(const float scalar,
                                      const AString& numericText,
                                      const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
                                      const bool drawTickMarkAtScalar);
        
        virtual ~AnnotationColorBarNumericText();
        
        AnnotationColorBarNumericText(const AnnotationColorBarNumericText& obj);

        AnnotationColorBarNumericText& operator=(const AnnotationColorBarNumericText& obj);

        /**
         * @return The scalar value.
         */
        float getScalar() const { return m_scalar; }

        /**
         * @return The numeric text.
         */
        AString getNumericText() const { return m_numericText; }
        
        /**
         * Set the numeric text
         *
         * @param text
         *     New text.
         */
        void setNumericText(const AString& text) { m_numericText = text; }
        
        /**
         * @return The horizontal alignment for the text
         */
        AnnotationTextAlignHorizontalEnum::Enum getHorizontalAlignment() const { return m_horizontalAlignment; }
        
        /**
         * @return Is a tick mark drawn at the scalar value ?
         */
        bool isDrawTickMarkAtScalar() const { return m_drawTickMarkAtScalar; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperAnnotationColorBarNumericText(const AnnotationColorBarNumericText& obj);

        float m_scalar;
        
        AString m_numericText;
        
        AnnotationTextAlignHorizontalEnum::Enum m_horizontalAlignment;
        
        bool m_drawTickMarkAtScalar;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_COLOR_BAR_NUMERIC_TEXT_H__
