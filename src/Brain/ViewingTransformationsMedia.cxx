
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
    
    const float mousePressXYZ[3] { static_cast<float>(mousePressX), static_cast<float>(mousePressY), 0.0f };

    /*
     * Scaling equations are set up so that:
     *   MouseDY =  100 results in "newScale" 2.0 (doubles current scale)
     *   MouseDY = -100 results in "newScale" 0.5 (halves current scale)
     */
    const float absMouseDY(mouseDY >= 0
                           ? mouseDY
                           : -mouseDY);
    const float minimumValidScaleValue(0.001);
    const float absDeltaScale((1.0f + (absMouseDY * 0.01)));
    CaretAssert(absDeltaScale > 0.0);
    const float deltaScale((mouseDY >= 0)
                         ? absDeltaScale
                         : (1.0 / absDeltaScale));
    const float oldScale(getScaling());
    const float totalScale(std::max((deltaScale * oldScale),
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
        
        /*
         * Need to offset model origin from where the mouse was pressed
         * so that the image pixel under mouse press stays at the same
         * location on the screen
         */
        float tx = -((dataX * totalScale) - identityXYZ[0]);
        float ty = -((dataY * totalScale) - identityXYZ[1]);
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
    CaretAssert(windowBounds);
    CaretAssert(selectionBounds);
    
    float regionMinX, regionMaxX, regionMinY, regionMaxY;
    selectionBounds->getBounds(regionMinX, regionMinY, regionMaxX, regionMaxY);
    
    /*
     * Ensure window and selection region are valid
     */
    const float windowWidth(windowBounds->getMaxX() - windowBounds->getMinX());
    const float windowHeight(windowBounds->getMaxY() - windowBounds->getMinY());
    const float regionWidth(regionMaxX - regionMinX);
    const float regionHeight(regionMaxY - regionMinY);
    if ((windowWidth > 0.0)
        && (regionWidth > 0.0)
        && (windowHeight > 0.0)
        && (regionHeight > 0.0)) {
        /*
         * Scale using width or height to best fit region into window.
         */
        const float widthScale(windowWidth / regionWidth);
        const float heightScale(windowHeight / regionHeight);
        const float scale(std::min(widthScale, heightScale));
        setScaling(scale);
        
        float centerXYZ[3] { 0.0, 0.0, 0.0 };
        selectionBounds->getCenter(centerXYZ[0], centerXYZ[1]);
        
        /*
         * Translate so that center of selection box is moved
         * to the center of the screen
         */
        float tx = -((centerXYZ[0] * scale));
        float ty = -((centerXYZ[1] * scale));
        setTranslation(tx, ty, 0.0);
    }
}


