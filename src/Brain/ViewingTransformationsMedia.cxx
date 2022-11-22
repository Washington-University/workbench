
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
#include "CaretUndoStack.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologySlice.h"
#include "Vector3D.h"
#include "ViewingTransformationsUndoCommand.h"

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
 * Copy the transformations for yoking
 * @param otherTransformation
 *    Instance whose transforms are copied to "this"
 * @param objImageWidthHeight
 *    Width / height of "obj" underlay image
 * @param myImageWidthHeight
 *    Width / height of "my" underlay image
 */
void
ViewingTransformationsMedia::copyTransformsForYoking(const ViewingTransformationsMedia& otherTransformation,
                                                     const float otherImageWidthHeight[2],
                                                     const float myImageWidthHeight[2])
{
    /*
     * Note: "This" image and the "other" image may be different sizes.  If we simply
     * copy the "other" transformation to "this", the translation will be too small/big
     * when the images are different sizes.  So, convert the translation to a percentage
     * of the "other" image's width/height and use it.
     */
    if (this != &otherTransformation) {
        /*
         * Save "my" translation
         */
        float oldTranslation[3];
        getTranslation(oldTranslation);
        
        /*
         * Copy the "other" viewing transformations to "this"
         */
        *this = otherTransformation;
        
        /*
         * Get the "other" translation
         */
        float otherTranslation[2];
        otherTransformation.getTranslation(otherTranslation);
        
        if ((otherImageWidthHeight[0] > 0)
            && (otherImageWidthHeight[1] > 0)
            && (myImageWidthHeight[0] > 0)
            && (myImageWidthHeight[1] > 0)) {
            /*
             * Convert the "other" translation to a percentage of the "other" image size
             */
            const float objTransPercentX(otherTranslation[0] / otherImageWidthHeight[0]);
            const float objTransPercentY(otherTranslation[1] / otherImageWidthHeight[1]);
            
            /*
             * Apply the percentage translation to "this"
             */
            float transX(myImageWidthHeight[0] * objTransPercentX);
            float transY(myImageWidthHeight[1] * objTransPercentY);
            
            setTranslation(transX, transY, 0.0);
        }
    }
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
                                             const float mouseDY,
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
    
    float mousePressXYZ[3] { static_cast<float>(mousePressX), static_cast<float>(mousePressY), 0.0f };

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
        setTranslation(tx, -ty, 0.0);
    }
}

/**
 * Set scaling for media
 *
 * @param transform
 *    Graphics object to window transform
 * @param scaling
 *    New value for scaling
 */
void
ViewingTransformationsMedia::setMediaScaling(const GraphicsObjectToWindowTransform* transform,
                                             const float scaling)
{
    const std::array<float, 4> viewport = transform->getViewport();
    const float vpCenter[3] {
        viewport[0] + (viewport[2] / 2.0f),
        viewport[1] + (viewport[3] / 2.0f),
        0.0f
    };
    
    if ( ! transform->isValid()) {
        return;
    }
    
    /*
     * Pixel at center of screen
     */
    float dataXYZ[3];
    transform->inverseTransformPoint(vpCenter, dataXYZ);

    /*
     * Center of orthographic projection is at center of screen.
     */
    const std::array<float, 4> orthoBounds(transform->getOrthoLRBT());
    const float orthoCenterXYZ[] {
        (orthoBounds[0] + orthoBounds[1]) / 2.0f,
        (orthoBounds[2] + orthoBounds[3]) / 2.0f,
        0.0
    };
    
    setScaling(scaling);
    
    /*
     * The origin is in the bottom left and scaling causes the image
     * to expand to the right and top.  Translate the image so that
     * the pixel at the center of the screen before scaling remains
     * at the center of the screen after scaling.
     */
    float tx = -((dataXYZ[0] * scaling) - orthoCenterXYZ[0]);
    float ty = -((dataXYZ[1] * scaling) - orthoCenterXYZ[1]);
    setTranslation(tx, -ty, 0.0);
}

/**
 * Set the bounds of the view to the given selection bounds.
 * @param transform
 *    Graphics object to window transform
 * @param selectionBounds
 *    Box containing bounds of selection
 * @param histologySlice
 *    histology slice on which bounds are set
 * @param stereotaxicCenterXyzOut
 *    Output with center of selection box in stereotaxic coordinates
 * @param stereotaxicWidthOut
 *    Output with width of selection box in stereotaxic space
 * @param stereotaxicHeightOut
 *    Output with height of selection box in stereotaxic space
 * @return
 *    True if outputs are valid, else false.
 */
bool
ViewingTransformationsMedia::setViewToBounds(const GraphicsObjectToWindowTransform* transform,
                                             const GraphicsRegionSelectionBox* selectionBounds,
                                             const HistologySlice* histologySlice,
                                             Vector3D& stereotaxicCenterXyzOut,
                                             float& stereotaxicWidthOut,
                                             float& stereotaxicHeightOut)
{
    CaretAssert(transform);
    CaretAssert(selectionBounds);
    
    stereotaxicCenterXyzOut = Vector3D();
    stereotaxicWidthOut  = 0.0;
    stereotaxicHeightOut = 0.0;
    
    float selectionBoxCenterX(0.0), selectionBoxCenterY(0.0);
    selectionBounds->getCenter(selectionBoxCenterX,
                               selectionBoxCenterY);
    
    /*
     * Ortho is in plane coordinates
     */
    const std::array<float, 4> orthoLRBT(transform->getOrthoLRBT());
    const float windowWidth(orthoLRBT[1] - orthoLRBT[0]);
    const float windowHeight(orthoLRBT[2] - orthoLRBT[3]); /* for images positive Y is down */

    const float selectionWidth(selectionBounds->getWidth());
    const float selectionHeight(selectionBounds->getHeight());

    bool resultValidFlag(false);
    
    /*
     * Ensure window and selection region are valid
     */
    if ((windowWidth > 0.0)
        && (selectionWidth > 0.0)
        && (windowHeight > 0.0)
        && (selectionHeight > 0.0)) {
        ViewingTransformations undoViewTrans;
        undoViewTrans.copyFromOther(*this);
        
        /*
         * Scale using width or height to best fit region into window.
         */
        const float widthScale(windowWidth / selectionWidth);
        const float heightScale(windowHeight / selectionHeight);
        const float scale(std::min(widthScale, heightScale));
        setScaling(scale);
        
        const Vector3D orthoCenterXYZ((orthoLRBT[0] + orthoLRBT[1]) / 2.0f,
                                      (orthoLRBT[2] + orthoLRBT[3]) / 2.0f,
                                      0.0);
        
        /*
         * Translate so that center of selection box is moved
         * to the center of the screen
         * NOTE: Origin is at top so invert Y-translation
         */
        float tx =  (orthoCenterXYZ[0] - (selectionBoxCenterX * scale));
        float ty = -(orthoCenterXYZ[1] - (selectionBoxCenterY * scale));
        setTranslation(tx, ty, 0.0);
        
        ViewingTransformations redoViewTrans;
        redoViewTrans.copyFromOther(*this);
        
        ViewingTransformationsUndoCommand* undoCommand = new ViewingTransformationsUndoCommand(this);
        undoCommand->setDescription("select image region");
        if (histologySlice != NULL) {
            undoCommand->setDescription("select histology region");
        }
        undoCommand->setRedoUndoValues(redoViewTrans,
                                       undoViewTrans);
        CaretUndoStack* undoStack = getRedoUndoStack();
        undoStack->push(undoCommand);
        
        if (histologySlice != NULL) {
            const Vector3D selectionBoundCenterXYZ(selectionBoxCenterX,
                                                   selectionBoxCenterY,
                                                   0.0);
            if (histologySlice->planeXyzToStereotaxicXyz(selectionBoundCenterXYZ,
                                                         stereotaxicCenterXyzOut)) {
                float minX(0.0) ,maxX(0.0), minY(0.0), maxY(0.0);
                if (selectionBounds->getBounds(minX, minY, maxX, maxY)) {
//                    std::cout << "   Min/Max X: " << minX << ", " << maxX << std::endl;
//                    std::cout << "   Min/Max Y: " << minY << ", " << maxY << std::endl;
                    
                    Vector3D planeTopLeftXYZ(minX, minY, 0.0);
                    Vector3D planeBottomLeftXYZ(minX, maxY, 0.0);
                    Vector3D planeTopRightXYZ(maxX, minY, 0.0);
                    Vector3D stereotaxicBottomLeftXYZ;
                    Vector3D stereotaxicTopLeftXYZ;
                    Vector3D stereotaxicTopRightXYZ;
                    if (histologySlice->planeXyzToStereotaxicXyz(planeBottomLeftXYZ, stereotaxicBottomLeftXYZ)
                        && histologySlice->planeXyzToStereotaxicXyz(planeTopLeftXYZ, stereotaxicTopLeftXYZ)
                        && histologySlice->planeXyzToStereotaxicXyz(planeTopRightXYZ, stereotaxicTopRightXYZ)) {
                        stereotaxicWidthOut  = (stereotaxicTopLeftXYZ - stereotaxicTopRightXYZ).length();
                        stereotaxicHeightOut = (stereotaxicTopLeftXYZ - stereotaxicBottomLeftXYZ).length();
                        resultValidFlag = true;
                    }
                }
            }
        }
    }
    
    return resultValidFlag;
}



