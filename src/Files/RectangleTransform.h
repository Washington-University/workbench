#ifndef __RECTANGLE_TRANSFORM_H__
#define __RECTANGLE_TRANSFORM_H__

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


#include <QRectF>

#include <memory>
#include "AString.h"
#include "CaretObject.h"

namespace caret {

    class LinearEquationTransform;
    class PixelIndex;
    
    class RectangleTransform : public CaretObject {
        
    public:
        enum class Origin {
            BOTTOM_LEFT,
            TOP_LEFT
        };
        RectangleTransform(const QRectF& sourceRectangle,
                           const Origin sourceRectangleOrigin,
                           const QRectF& targetRectangle,
                           const Origin targetRectangleOrigin);
        
        virtual ~RectangleTransform();
        
        RectangleTransform(const RectangleTransform& obj) = delete;

        RectangleTransform& operator=(const RectangleTransform& obj) = delete;
        
        bool isValid() const;
        
        AString getErrorMessage() const;
        
        PixelIndex transformSourceToTarget(const PixelIndex& sourcePixelIndex) const;
        
        void transformSourceToTarget(const float sourceX,
                                     const float sourceY,
                                     float& targetX,
                                     float& targetY) const;

        void transformTargetToSource(const float targetX,
                                     const float targetY,
                                     float& sourceX,
                                     float& sourceY) const;

        static void testTransforms(RectangleTransform& transform,
                                   const QRectF& sourceRect,
                                   const QRectF& targetRect);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void convertRectangleToBottomLeftOrigin(QRectF& rect,
                                                const Origin origin) const;
        
        QRectF m_sourceRectangle;
        
        const Origin m_sourceRectangleOrigin;
        
        QRectF m_targetRectangle;
        
        const Origin m_targetRectangleOrigin;
        
        std::unique_ptr<LinearEquationTransform> m_xTransform;
        
        std::unique_ptr<LinearEquationTransform> m_xInverseTransform;
        
        std::unique_ptr<LinearEquationTransform> m_yTransform;
        
        std::unique_ptr<LinearEquationTransform> m_yInverseTransform;
        
        bool m_valid = false;
        
        AString m_errorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECTANGLE_TRANSFORM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECTANGLE_TRANSFORM_DECLARE__

} // namespace
#endif  //__RECTANGLE_TRANSFORM_H__
