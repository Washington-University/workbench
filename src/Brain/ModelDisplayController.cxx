/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "ModelDisplayController.h"

#include "CaretAssert.h"
#include "Matrix4x4.h"

using namespace caret;

/**
 * Constructor.
 * @param controllerType Type of this controller.
 * @param allowsYokingFlag  This controller can be yoked.
 * @param allowsRotationFlag This controller can be rotated.
 *
 */
ModelDisplayController::ModelDisplayController(const ModelDisplayControllerTypeEnum::Enum controllerType,
                                               const YokingAllowedType allowsYokingStatus,
                                               const RotationAllowedType allowsRotationStatus)
    : CaretObject()
{
    this->initializeMembersModelDisplayController();
    this->controllerType = controllerType;
    this->allowsYokingStatus = allowsYokingStatus;
    this->allowsRotationStatus   = allowsRotationStatus;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->resetViewPrivate(i);
    }
}

/**
 * Destructor
 */
ModelDisplayController::~ModelDisplayController()
{
}

void
ModelDisplayController::initializeMembersModelDisplayController()
{
    this->defaultModelScaling = 1.0f;
}

/**
 * @return The type of model controller.
 */
ModelDisplayControllerTypeEnum::Enum 
ModelDisplayController::getControllerType() const
{
    return this->controllerType; 
}

/**
 * See if this controller allows rotation.
 * 
 * @return true if this controller allows rotation, else false.
 *
 */
bool
ModelDisplayController::isRotationAllowed() const
{
    return (this->allowsRotationStatus == ROTATION_ALLOWED_YES);
}

/**
 * See if this controller allows yoking.
 * When Yoked, this controller will maintain the same viewpoint
 * as the user moves the mouse in any other yoked controller.
 * @return  true if this controller supports yoking, else false.
 *
 */
bool
ModelDisplayController::isYokeable() const
{
    return (this->allowsYokingStatus == YOKING_ALLOWED_YES);
}

/**
 * For a structure model, copy the transformations from one window of
 * the structure model to another window.
 *
 * @param controllerSource        Source structure model
 * @param windowTabNumberSource   windowTabNumber of source transformation.
 * @param windowTabNumberTarget   windowTabNumber of target transformation.
 *
 */
void
ModelDisplayController::copyTransformations(
                   const ModelDisplayController& controllerSource,
                   const int32_t windowTabNumberSource,
                   const int32_t windowTabNumberTarget)
{
    if (windowTabNumberSource == windowTabNumberTarget) {
        return;
    }
    
    CaretAssertArrayIndex(this->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberTarget);
    CaretAssertArrayIndex(controllerSource->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberSource);
    
    this->translation[windowTabNumberTarget][0] = controllerSource.translation[windowTabNumberSource][0];
    this->translation[windowTabNumberTarget][1] = controllerSource.translation[windowTabNumberSource][1];
    this->translation[windowTabNumberTarget][2] = controllerSource.translation[windowTabNumberSource][2];
    this->scaling[windowTabNumberTarget] = controllerSource.scaling[windowTabNumberSource];
    
    this->viewingRotationMatrix[windowTabNumberTarget].setMatrix(*controllerSource.getViewingRotationMatrix(windowTabNumberSource));
}

/**
 * the viewing rotation matrix.
 *
 * @param  windowTabNumber  Window for which rotation is requested
 * @return Reference to the viewing rotation matrix.
 *
 */
const Matrix4x4*
ModelDisplayController::getViewingRotationMatrix(const int32_t windowTabNumber) const
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    return &this->viewingRotationMatrix[windowTabNumber];
}

/**
 * the viewing rotation matrix.
 *
 * @param  windowTabNumber  Window for which rotation is requested
 * @return Reference to the viewing rotation matrix.
 *
 */
Matrix4x4*
ModelDisplayController::getViewingRotationMatrix(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    return &this->viewingRotationMatrix[windowTabNumber];
}

/**
 * get the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @return  The translation, an array of three floats.
 *
 */
const float*
ModelDisplayController::getTranslation(const int32_t windowTabNumber) const
{
    CaretAssertArrayIndex(this->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    return &this->translation[windowTabNumber][0];
}

/**
 * set the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  t  The translation, an array of three floats.
 *
 */
void
ModelDisplayController::setTranslation(const int32_t windowTabNumber,
                          const float t[3])
{
    CaretAssertArrayIndex(this->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    this->translation[windowTabNumber][0] = t[0];
    this->translation[windowTabNumber][1] = t[1];
    this->translation[windowTabNumber][2] = t[2];
}

/**
 * set the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  tx - The x-coordinate of the translation.
 * @param  ty - The y-coordinate of the translation.
 * @param  tz - The z-coordinate of the translation.
 *
 */
void
ModelDisplayController::setTranslation(const int32_t windowTabNumber,
                          const float tx,
                          const float ty,
                          const float tz)
{
    CaretAssertArrayIndex(this->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    this->translation[windowTabNumber][0] = tx;
    this->translation[windowTabNumber][1] = ty;
    this->translation[windowTabNumber][2] = tz;
}

/**
 * get the scaling.
 *
 * @param  windowTabNumber  Window for which scaling is requested
 * @return  Scaling value.
 *
 */
float
ModelDisplayController::getScaling(const int32_t windowTabNumber) const
{
    CaretAssertArrayIndex(this->scaling,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    return this->scaling[windowTabNumber];
}

/**
 * set the scaling.
 *
 * @param  windowTabNumber  Window for which scaling is requested
 * @param  s  The scaling value.
 *
 */
void
ModelDisplayController::setScaling(
                   const int32_t windowTabNumber,
                   const float s)
{
    CaretAssertArrayIndex(this->scaling,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    this->scaling[windowTabNumber] = s;
}

/**
 * Resets the view to the default view.
 * 
 * Since is resetView() is virtual and overridden by subclasses,
 * resetView() cannot be called by the constructor of this class.
 * So this methed, resetViewPrivate() is used to reset the view and
 * to initialize the views inside of the constructor.
 *
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void 
ModelDisplayController::resetViewPrivate(const int windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    this->setTranslation(windowTabNumber, 0.0f, 0.0f, 0.0f);
    this->viewingRotationMatrix[windowTabNumber].identity();
    this->setScaling(windowTabNumber, this->defaultModelScaling);    
}

/**
 * Reset the view to the default view.
 *
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelDisplayController::resetView(const int32_t windowTabNumber)
{
    this->resetViewPrivate(windowTabNumber);
}

/**
 * @param  windowTabNumber  Window for which view is requested
 * set to a right side view.
 *
 */
void
ModelDisplayController::rightView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
    viewingRotationMatrix[windowTabNumber].rotateY(-90.0);
    viewingRotationMatrix[windowTabNumber].rotateZ(-90.0);
}

/**
 * set to a left side view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelDisplayController::leftView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
    viewingRotationMatrix[windowTabNumber].rotateY(90.0);
    viewingRotationMatrix[windowTabNumber].rotateZ(90.0);
}

/**
 * set to a anterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelDisplayController::anteriorView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
    viewingRotationMatrix[windowTabNumber].rotateX(-90.0);
    viewingRotationMatrix[windowTabNumber].rotateY(180.0);}

/**
 * set to a posterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelDisplayController::posteriorView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
    viewingRotationMatrix[windowTabNumber].rotateX(-90.0);
}

/**
 * set to a dorsal view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelDisplayController::dorsalView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
}

/**
 * set to a ventral view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelDisplayController::ventralView(const int32_t windowTabNumber)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    viewingRotationMatrix[windowTabNumber].identity();
    viewingRotationMatrix[windowTabNumber].rotateY(-180.0);
}

/**
 * Set the transformation.
 *
 * @param windowTabNumber  Window of this view.
 * @param transformationData - the transformation data:
 *          translation(3)
 *          viewing matrix[4][4],
 *          scaling(1)
 *
 */
void
ModelDisplayController::setTransformation(
                   const int32_t windowTabNumber,
                   const std::vector<float>& transformationData)
{
    CaretAssertArrayIndex(this->viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    uint32_t ctr = 0;
    
    if (transformationData.size() >= (ctr + 3)) {
        this->setTranslation(windowTabNumber,
                            transformationData[ctr + 0],
                            transformationData[ctr + 1],
                            transformationData[ctr + 2]);
        ctr += 3;
    }
    
    if (transformationData.size() >= (ctr + 16)) {
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {
                this->viewingRotationMatrix[windowTabNumber].setMatrixElement(
                        i, j, transformationData[ctr]);
                ctr++;
            }
        }
    }
    
    if (transformationData.size() >= (ctr + 17)) {
        this->setScaling(windowTabNumber, transformationData[ctr]);
        ctr++;
    }
}

/**
 * Get a String for use in the GUI.  Use toDescriptiveString() for
 * information about this controller's content.
 *
 * @return String for use in the GUI.
 *
 */
AString
ModelDisplayController::toString() const
{
       return this->getNameForGUI(true);
}

/**
 * Returns a descriptive string containing info about this instance.
 *
 * @return  String describing contents of this instance.
 *
 */
AString
ModelDisplayController::toDescriptiveString() const
{
    AString s = CaretObject::toString();
    
    return s;
}

