
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

#define __VIEWING_TRANSFORMATIONS_HISTOLOGY_DECLARE__
#include "ViewingTransformationsHistology.h"
#undef __VIEWING_TRANSFORMATIONS_HISTOLOGY_DECLARE__

#include <algorithm>

#include "BoundingBox.h"
#include "BrainOpenGLViewportContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologySlice.h"
#include "MouseEvent.h"
#include "Vector3D.h"
#include "ViewingTransformationToFitRegion.h"
#include "ViewingTransformationsUndoCommand.h"

using namespace caret;

/**
 * \class caret::ViewingTransformationsHistology
 * \brief Viewing transformations (pan/rotate/zoom) for histology.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for histology viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsHistology::ViewingTransformationsHistology()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsHistology::~ViewingTransformationsHistology()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsHistology::ViewingTransformationsHistology(const ViewingTransformationsHistology& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsHistology(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsHistology&
ViewingTransformationsHistology::operator=(const ViewingTransformationsHistology& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsHistology(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsHistology::copyHelperViewingTransformationsHistology(const ViewingTransformationsHistology& /*obj*/)
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
ViewingTransformationsHistology::copyTransformsForYoking(const ViewingTransformationsHistology& otherTransformation,
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
ViewingTransformationsHistology::resetView()
{
    ViewingTransformations::resetView();
    m_rotationMatrix->identity();
}

/**
 * Set the bounds of the view to the given selection bounds.
 * @param mouseEvent
 *    The mouse event that triggered this function
 * @param selectionRegion
 *    Box containing bounds of selection
 * @param regionPercentageViewportWidth
 *    Percentage of viewport width occupied by selected region
 * @param regionPercentageViewportHeight
 *    Percentage of viewport height occupied by selected region
 * @param transform
 *    Graphics object to window transform
 * @param histologySlice
 *    histology slice on which bounds are set
 * @return
 *    True if outputs are valid, else false.
 */
bool
ViewingTransformationsHistology::setHistologyViewToBounds(const MouseEvent* mouseEvent,
                                                          const GraphicsRegionSelectionBox* selectionRegion,
                                                          const GraphicsObjectToWindowTransform* transform,
                                                          const HistologySlice* histologySlice)
{
    CaretAssert(mouseEvent);
    CaretAssert(transform);
    CaretAssert(selectionRegion);
    
    const BrainOpenGLViewportContent* viewportContent(mouseEvent->getViewportContent());
    const BrowserTabContent* browserTabContent(viewportContent->getBrowserTabContent());
                                                      
    Vector3D translation;
    float zoom(0.0);
    ViewingTransformationToFitRegion transformFitToRegion(viewportContent,
                                                          selectionRegion,
                                                          browserTabContent);
    if (transformFitToRegion.applyToHistologySlice(transform,
                                                   translation,
                                                   zoom)) {
        ViewingTransformations undoViewTrans;
        undoViewTrans.copyFromOther(*this);
        setScaling(zoom);
        
        setTranslation(translation);
        
        ViewingTransformations redoViewTrans;
        redoViewTrans.copyFromOther(*this);
        
        ViewingTransformationsUndoCommand* undoCommand = new ViewingTransformationsUndoCommand(this);
        undoCommand->setDescription("selection of image region");
        if (histologySlice != NULL) {
            undoCommand->setDescription("selection of histology region");
        }
        undoCommand->setRedoUndoValues(redoViewTrans,
                                       undoViewTrans);
        CaretUndoStack* undoStack = getRedoUndoStack();
        undoStack->push(undoCommand);

        return true;
    }
    
    return false;
}

