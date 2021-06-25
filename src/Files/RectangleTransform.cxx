
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

#define __RECTANGLE_TRANSFORM_DECLARE__
#include "RectangleTransform.h"
#undef __RECTANGLE_TRANSFORM_DECLARE__

#include <cmath>

#include <QRectF>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "LinearEquationTransform.h"

using namespace caret;
    
/**
 * \class caret::RectangleTransform 
 * \brief Transforms points from one rectangle to another
 * \ingroup File
 */

/**
 * Constructor for transformation between source and target rectangle
 * @param sourceRectangle
 *    The source rectangle
 * @param sourceRectangleOrigin
 *    The location of the origin in the source rectangle
 * @param targetRectangle
 *    The target rectangle
 * @param targetRectangleOrigin
 *    The location of the origin in the target rectangle
 */
RectangleTransform::RectangleTransform(const QRectF& sourceRectangle,
                                       const Origin sourceRectangleOrigin,
                                       const QRectF& targetRectangle,
                                       const Origin targetRectangleOrigin)
: CaretObject(),
m_sourceRectangle(sourceRectangle),
m_sourceRectangleOrigin(sourceRectangleOrigin),
m_targetRectangle(targetRectangle),
m_targetRectangleOrigin(targetRectangleOrigin)
{
    m_valid = false;
    
    if ( ! m_sourceRectangle.isValid()) {
        m_errorMessage.appendWithNewLine("Source rectangle is invalid.");
    }
    if ( ! m_targetRectangle.isValid()) {
        m_errorMessage.appendWithNewLine("Destination rectangle is invalid.");
    }
    if ( ! m_errorMessage.isEmpty()) {
        return;
    }
    
    {
        const float sx0(m_sourceRectangle.x());
        float sx1(m_sourceRectangle.x());
        float sx2(m_sourceRectangle.x() + m_sourceRectangle.width());
        
        const float sy0(m_sourceRectangle.y());
        float sy1(m_sourceRectangle.y());
        float sy2(m_sourceRectangle.y() + m_sourceRectangle.height());
        switch (m_sourceRectangleOrigin) {
            case Origin::BOTTOM_LEFT:
                break;
            case Origin::TOP_LEFT:
                std::swap(sy1,
                          sy2);
                break;
        }
        
        const float tx0(m_targetRectangle.x());
        float tx1(m_targetRectangle.x());
        float tx2(m_targetRectangle.x() + m_targetRectangle.width());
        
        const float ty0(m_targetRectangle.y());
        float ty1(m_targetRectangle.y());
        float ty2(m_targetRectangle.y() + m_targetRectangle.height());
        switch (m_targetRectangleOrigin) {
            case Origin::BOTTOM_LEFT:
                break;
            case Origin::TOP_LEFT:
                std::swap(ty1,
                          ty2);
                break;
        }
        
        AString errorMessage;
        m_xTransform = LinearEquationTransform::newInstance(sx1, sx2,
                                                            tx1, tx2,
                                                            sx0, tx0, errorMessage);
        if ( ! m_xTransform) {
            m_errorMessage.appendWithNewLine(errorMessage);
        }
        errorMessage.clear();
        
        m_xInverseTransform = LinearEquationTransform::newInstance(tx1, tx2,
                                                                   sx1, sx2,
                                                                   tx0, sx0,
                                                                   errorMessage);
        if ( ! m_xInverseTransform) {
            errorMessage.appendWithNewLine(errorMessage);
        }
        errorMessage.clear();
        
        m_yTransform = LinearEquationTransform::newInstance(sy1, sy2,
                                                            ty1, ty2,
                                                            sy0, ty0,
                                                            errorMessage);
        if ( ! m_yTransform) {
            m_errorMessage.appendWithNewLine(errorMessage);
        }
        errorMessage.clear();
        
        m_yInverseTransform = LinearEquationTransform::newInstance(ty1, ty2,
                                                                   sy1, sy2,
                                                                   ty0, sy0,
                                                                   errorMessage);
        if ( ! m_yInverseTransform) {
            m_errorMessage.appendWithNewLine(errorMessage);
        }
    }
    if ( ! m_errorMessage.isEmpty()) {
        return;
    }

    m_valid = true;
}

/**
 * Destructor.
 */
RectangleTransform::~RectangleTransform()
{
}


/**
 * @return True if the transform is valid
 */
bool
RectangleTransform::isValid() const
{
    return m_valid;
}


/**
 * Convert the rectangle (if needed) for origin at the bottom left
 * @param rect
 *    The rectangle
 * @param origin
 *    Location of the origin
 */
void
RectangleTransform::convertRectangleToBottomLeftOrigin(QRectF& rect,
                                                       const Origin origin) const
{
    switch (origin) {
        case Origin::BOTTOM_LEFT:
            break;
        case Origin::TOP_LEFT:
        {
            const float bottomY(rect.y() - rect.height());
            rect.setY(bottomY);
        }
            break;
    }
}

/**
 * @return error message when transform is not valid
 */
AString
RectangleTransform::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * Transform the point from source to target rectangle
 * @param sourceX
 *    X-coordinate in source rectangle
 * @param sourceY
 *    Y-coordinate in source rectangle
 * @param targetX
 *    Output with X-coordinate in target rectangle
 * @param targetY
 *    Output with Y-coordinate in target rectangle
 */
void
RectangleTransform::transformSourceToTarget(const float sourceX,
                                            const float sourceY,
                                            float& targetX,
                                            float& targetY) const
{
    if ( ! m_valid) {
        CaretLogSevere("Attempting to use invalid RectangleTransform");
        return;
    }
    
    targetX = m_xTransform->transform(sourceX);
    targetY = m_yTransform->transform(sourceY);
}

/**
 * Transform the point from target to source rectangle
 * @param targetX
 *    X-coordinate in target rectangle
 * @param targetY
 *    Y-coordinate in target rectangle
 * @param sourceX
 *    Output with X-coordinate in source rectangle
 * @param sourceY
 *    Output with Y-coordinate in source rectangle
 */
void
RectangleTransform::transformTargetToSource(const float targetX,
                                            const float targetY,
                                            float& sourceX,
                                            float& sourceY) const
{
    if ( ! m_valid) {
        CaretLogSevere("Attempting to use invalid RectangleTransform");
        return;
    }
    
    sourceX = m_xInverseTransform->transform(targetX);
    sourceY = m_yInverseTransform->transform(targetY);
}

/**
 * Tests the given transform to verify it works correctly
 * @param transform
 *    The transform
 * @param sx
 *    The source x-value
 * @param sy
 *    The source y-value
 * @param tx
 *    The target x-value
 * @param ty
 *    The target y-value
 */
void
testSourceToTargetHelper(RectangleTransform& transform,
                         const float sx,
                         const float sy,
                         const float tx,
                         const float ty)
{
    {
        float x2(0.0), y2(0.0);
        transform.transformSourceToTarget(sx, sy, x2, y2);
        const float dx(std::fabs(x2 - tx));
        const float dy(std::fabs(y2 - ty));
        if ((dx > 0.001)
            || (dy > 0.001)) {
            std::cout << "WRONG ";
        }
        std::cout << "S -> T  x=" << sx << ", y=" << sy
        << " Result x=" << x2 << ", y=" << y2
        << " Correct x=" << tx << " y=" << ty << std::endl;
    }

    {
        float x2(0.0), y2(0.0);
        transform.transformTargetToSource(tx, ty, x2, y2);
        const float dx(std::fabs(x2 - sx));
        const float dy(std::fabs(y2 - sy));
        if ((dx > 0.001)
            || (dy > 0.001)) {
            std::cout << "WRONG ";
        }
        std::cout << "T -> S  x=" << tx << ", y=" << ty
        << " Result x=" << x2 << ", y=" << y2
        << " Correct x=" << sx << " y=" << sy << std::endl;
    }
}
/**
 * Test the given transform for transforms between the source and target rectangle
 * @param transform
 *    Transform that is tested
 * @param sourceRect
 *    The source rectangle
 * @param targetRect
 *    The target rectangle
 */
void
RectangleTransform::testTransforms(RectangleTransform& transform,
                                   const QRectF& sourceRect,
                                   const QRectF& targetRect)
{
    testSourceToTargetHelper(transform,
                             sourceRect.x(), sourceRect.y(),
                             targetRect.x(), targetRect.y());
    
    testSourceToTargetHelper(transform,
                             sourceRect.x() + sourceRect.width(), sourceRect.y(),
                             targetRect.x() + targetRect.width(), targetRect.y());
    
    testSourceToTargetHelper(transform,
                             sourceRect.x(), sourceRect.y() + sourceRect.height(),
                             targetRect.x(), targetRect.y() + targetRect.height());
    
    testSourceToTargetHelper(transform,
                             sourceRect.x() + sourceRect.width(), sourceRect.y() + sourceRect.height(),
                             targetRect.x() + targetRect.width(), targetRect.y() + targetRect.height());
    
    testSourceToTargetHelper(transform,
                             sourceRect.x() + sourceRect.width() / 2.0, sourceRect.y() + sourceRect.height() / 2.0,
                             targetRect.x() + targetRect.width() / 2.0, targetRect.y() + targetRect.height() / 2.0);
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
RectangleTransform::toString() const
{
    return "RectangleTransform";
}

