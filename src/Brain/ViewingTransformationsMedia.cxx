
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

#define __VIEWING_TRANSFORMATIONS_MEDIA_DECLARE__
#include "ViewingTransformationsMedia.h"
#undef __VIEWING_TRANSFORMATIONS_MEDIA_DECLARE__

#include <algorithm>

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"

using namespace caret;

/**
 * \class caret::ViewingTransformationsMedia
 * \brief Viewing transformations (pan/rotate/zoom) for media.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for media viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsMedia::ViewingTransformationsMedia()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsMedia::~ViewingTransformationsMedia()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsMedia::ViewingTransformationsMedia(const ViewingTransformationsMedia& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsMedia(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsMedia&
ViewingTransformationsMedia::operator=(const ViewingTransformationsMedia& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsMedia(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsMedia::copyHelperViewingTransformationsMedia(const ViewingTransformationsMedia& /*obj*/)
{
    
}

/**
 * Reset the view to the default view for media.
 */
void
ViewingTransformationsMedia::resetView()
{
    ViewingTransformations::resetView();
    m_rotationMatrix->identity();
}

/**
 * Scale about the position of the mouse
 * @param transform
 *    Graphics object to window transform
 * @param mousePressX
 *    X-Location of where mouse was pressed
 * @param mousePressX
 *    Y-Location of where mouse was pressed
 * @param mouseDY
 *    Change in mouse Y
 * @param dataX
 *    X-coordinate of data where mouse was pressed
 * @param dataY
 *    Y-coordinate of data where mouse was pressed
 * @param dataXYValidFlag
 *    True if dataX and dataY are valid
 */
void
ViewingTransformationsMedia::scaleAboutMouse(const GraphicsObjectToWindowTransform* transform,
                                             const int32_t mousePressX,
                                             const int32_t mousePressY,
                                             const int32_t mouseDY,
                                             const float dataX,
                                             const float dataY,
                                             const bool dataXYValidFlag)
{
    if (mouseDY == 0) {
        return;
    }
    
    if (transform == NULL) {
        CaretLogSevere("Object to window transform is NULL");
        return;
    }
    if ( ! transform->isValid()) {
        CaretLogSevere("Object to window transform is INVALID");
        return;
    }
    
    const bool debugFlag(false);
    const float mousePressXYZ[3] { static_cast<float>(mousePressX), static_cast<float>(mousePressY), 0.0f };
    if (debugFlag) {
        std::cout << "Window X Y: " << mousePressXYZ[0] << ", " << mousePressXYZ[1] << std::endl;
        std::cout << "   Data X Y: " << dataX << ", " << dataY << std::endl;
    }
    
    
    /*
     * Update the scaling but don't let it get to zero or negative
     */
    const float minimumValidScaleValue(0.001);
    const float newScale((1.0f + (mouseDY * 0.01)));
    const float oldScale(getScaling());
    const float totalScale(std::max((newScale * oldScale),
                                    minimumValidScaleValue));
    setScaling(totalScale);
    
    /*
     * If mouse is over the image, use the data XY to translate image
     * so that image pixel under mouse press location on screen remains
     * at that location on the screen
     */
    if (dataXYValidFlag) {
        /*
         * Get viewing coordinate at the location of mouse when mouse was first pressed
         */
        Matrix4x4 identityMatrix;
        auto newXform = transform->cloneWithNewModelViewMatrix(identityMatrix);
        float identityXYZ[3];
        newXform->inverseTransformPoint(mousePressXYZ, identityXYZ);
        if (debugFlag) {
            std::cout << "   Identity X Y: " << identityXYZ[0] << ", " << identityXYZ[1] << std::endl;
        }
        
        /*
         * Need to offset model origin from where the mouse was pressed
         * so that the image pixel under mouse press stays at the same
         * location on the screen
         */
        float tx = -((dataX * totalScale) - identityXYZ[0]);
        float ty = -((dataY * totalScale) - identityXYZ[1]);
        if (debugFlag) {
            std::cout << "   Ident Trans XY: " << tx << ", " << ty << std::endl;
        }
        
        setTranslation(tx, ty, 0.0);
    }
}

/**
 * Set the bounds of the view to the given selection bounds.
 * @param windowBounds
 *    Box containing bounds of window
 * @param selectionBounds
 *    Box containing bounds of selection
 */
void
ViewingTransformationsMedia::setViewToBounds(const BoundingBox* windowBounds,
                                             const GraphicsRegionSelectionBox* selectionBounds)
{
    return;  /* disable temporarily */
    CaretAssert(windowBounds);
    CaretAssert(selectionBounds);
    
    {
        float bb[6];
        windowBounds->getBounds(bb);
        std::cout << "Window BottomLeft: " << bb[0] << ", " << bb[2] << " Top Right: " << bb[1] << ", " << bb[3] << std::endl;
    }
    
    float regionMinX, regionMaxX, regionMinY, regionMaxY;
    selectionBounds->getBounds(regionMinX, regionMinY, regionMaxX, regionMaxY);
    std::cout << "   Selection BottomLeft: " << regionMinX << ", " << regionMinY << " Top Right: " << regionMaxX << ", " << regionMaxY << std::endl;

    const float windowWidth(windowBounds->getMaxX() - windowBounds->getMinX());
    const float windowHeight(windowBounds->getMaxY() - windowBounds->getMinY());
    const float regionWidth(regionMaxX - regionMinX);
    const float regionHeight(regionMaxY - regionMinY);
    std::cout << "   Window Width: " << windowWidth << std::endl;
    std::cout << "   Region Width: " << regionWidth << std::endl;
    if ((windowWidth > 0.0)
        && (regionWidth > 0.0)
        && (windowHeight > 0.0)
        && (regionHeight > 0.0)) {
        float widthScale(windowWidth / regionWidth);
        float heightScale(windowHeight / regionHeight);
        
        /*
         * Translate to center of region
         */
        const float translateX(-(regionMinX + regionMaxX) / 2.0);
        const float translateY(-(regionMinY + regionMaxY) / 2.0);
        
        std::cout << "   Tx = " << translateX << ", Ty = " << translateY
        << " scale-width = " << widthScale << " scale-height = " << heightScale << std::endl;
        
        setTranslation(translateX, translateY, 0.0);
        
        const float scale(std::min(widthScale, heightScale));
        
        setScaling(scale);
        
//        Matrix4x4 m;
//        m.translate(translateX, translateY, 0.0);
//        m.scale(scale, scale, 1.0);
//        m.translate(-translateX, -translateY, 0.0);
//
//        float t[3];
//        m.getTranslation(t);
//        double sx, sy, sz;
//        m.getScale(sx, sy, sz);
        
//        std::cout << "  Matrix Tx=" << t[0] << " Ty=" << t[1] << " scale=" << sx << std::endl;
//        setTranslation(t);
//        setScaling(sx);
        
    }
}


