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


#include "ModelController.h"

#include "Brain.h"
#include "CaretWindow.h"
#include "Matrix4x4.h"

using namespace caret;

/**
 * Constructor.
 * @param brain Brain to which this controller belongs.
 * @param allowsYokingFlag  This controller can be yoked.
 * @param allowsRotationFlag This controller can be rotated.
 *
 */
ModelController::ModelController(Brain* brain,
               const bool allowsYokingFlag,
               const bool allowsRotationFlag)
    : CaretObject()
{
    this->initializeMembersModelController();
    this->brain = brain;
    this->initializeTransformations();
    this->allowsRotationFlag = allowsRotationFlag;
    this->allowsYokingFlag   = allowsYokingFlag;
}

/**
 * Destructor
 */
ModelController::~ModelController()
{
    for (uint64_t i = 0; i < this->viewingRotationMatrix.size(); i++) {
        delete this->viewingRotationMatrix[i];
    }
}

void
ModelController::initializeMembersModelController()
{
    this->defaultModelScaling = 1.0f;
    this->allowsYokingFlag = false;
    this->allowsRotationFlag = false;
    this->brain = NULL;
}
/**
 * Get the brain to which this controller belongs.
 * @return Brain of this controller.
 *
 */
Brain*
ModelController::getBrain() const
{
    return this->brain;
}

/**
 * Initialize the transformation matrices.
 *
 */
void
ModelController::initializeTransformations()
{
    this->viewingRotationMatrix.reserve(CaretWindow::NUMBER_OF_WINDOWS);
    this->translation.reserve(CaretWindow::NUMBER_OF_WINDOWS * 3);
    this->scaling.reserve(CaretWindow::NUMBER_OF_WINDOWS);
    
    for (int32_t i = 0; i < CaretWindow::NUMBER_OF_WINDOWS; i++) {
        this->viewingRotationMatrix.push_back(new Matrix4x4());
        this->scaling.push_back(this->defaultModelScaling);
    }
    for (int32_t i = 0; i < (CaretWindow::NUMBER_OF_WINDOWS * 3); i++) {
        this->translation.push_back(0.0f);
    }
    
    for (int32_t i = 0; i < CaretWindow::NUMBER_OF_WINDOWS; i++) {
        this->resetView(*CaretWindow::indexToWindow(i));
    }
}



/**
 * See if this controller allows rotation.
 * 
 * @return true if this controller allows rotation, else false.
 *
 */
bool
ModelController::isRotationAllowed() const
{
    return this->allowsRotationFlag;
}

/**
 * See if this controller allows yoking.
 * When Yoked, this controller will maintain the same viewpoint
 * as the user moves the mouse in any other yoked controller.
 * @return  true if this controller supports yoking, else false.
 *
 */
bool
ModelController::isYokeable() const
{
    return this->allowsYokingFlag;
}

/**
 * For a structure model, copy the transformations from one window of
 * the structure model to another window.
 *
 * @param controller           Source structure model
 * @param windowSourceID   WindowID of source transformation.
 * @param windowTargetID   WindowID of target transformation.
 *
 */
void
ModelController::copyTransformations(
                   const ModelController& controller,
                   const CaretWindow& windowSourceID,
                   const CaretWindow& windowTargetID)
{
    if (windowSourceID.getEnum() == windowTargetID.getEnum()) {
        return;
    }
    
    const int32_t sourceWindowIndex = windowSourceID.getWindowIndex();
    const int32_t targetWindowIndex = windowTargetID.getWindowIndex();
    
    const int32_t it3 = targetWindowIndex * 3;
    const int32_t is3 = sourceWindowIndex * 3;
    translation[it3] = controller.translation[is3];
    translation[it3+1] = controller.translation[is3+1];
    translation[it3+2] = controller.translation[is3+2];
    scaling[targetWindowIndex] = controller.scaling[sourceWindowIndex];
    
    viewingRotationMatrix[targetWindowIndex]->setMatrix(*controller.getViewingRotationMatrix(windowSourceID));
}

/**
 * the viewing rotation matrix.
 *
 * @param  windowID  Window for which rotation is requested
 * @return Reference to the viewing rotation matrix.
 *
 */
Matrix4x4*
ModelController::getViewingRotationMatrix(const CaretWindow& windowID) const
{
    return this->viewingRotationMatrix[windowID.getWindowIndex()];
}

/**
 * get the translation.
 *
 * @param  windowID  Window for which translation is requested
 * @return  The translation, an array of three floats.
 *
 */
const float*
ModelController::getTranslation(const CaretWindow& windowID) const
{
    const int32_t i3 = windowID.getWindowIndex() * 3;
    return &this->translation[i3];
}

/**
 * set the translation.
 *
 * @param  windowID  Window for which translation is requested
 * @param  t  The translation, an array of three floats.
 *
 */
void
ModelController::setTranslation(
                   const CaretWindow& windowID,
                   const float t[])
{
    const int32_t i3 = windowID.getWindowIndex() * 3;
    this->translation[i3]   = t[0];
    this->translation[i3+1] = t[1];
    this->translation[i3+2] = t[2];
}

/**
 * set the translation.
 *
 * @param  windowID  Window for which translation is requested
 * @param  tx - The x-coordinate of the translation.
 * @param  ty - The y-coordinate of the translation.
 * @param  tz - The z-coordinate of the translation.
 *
 */
void
ModelController::setTranslation(
                   const CaretWindow& windowID,
                   const float tx,
                   const float ty,
                   const float tz)
{
    const int32_t i3 = windowID.getWindowIndex() * 3;
    this->translation[i3]   = tx;
    this->translation[i3+1] = ty;
    this->translation[i3+2] = tz;
}

/**
 * get the scaling.
 *
 * @param  windowID  Window for which scaling is requested
 * @return  Scaling value.
 *
 */
float
ModelController::getScaling(const CaretWindow& windowID) const
{
    return this->scaling[windowID.getWindowIndex()];
}

/**
 * set the scaling.
 *
 * @param  windowID  Window for which scaling is requested
 * @param  s  The scaling value.
 *
 */
void
ModelController::setScaling(
                   const CaretWindow& windowID,
                   const float s)
{
    this->scaling[windowID.getWindowIndex()] = s;
}

/**
 * @param  windowID  Window for which view is requested
 * reset the view.
 *
 */
void
ModelController::resetView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    this->setTranslation(windowID, 0.0f, 0.0f, 0.0f);
    this->viewingRotationMatrix[windowIndex]->identity();
    this->setScaling(windowID, this->defaultModelScaling);
}

/**
 * @param  windowID  Window for which view is requested
 * set to a right side view.
 *
 */
void
ModelController::rightView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
    viewingRotationMatrix[windowIndex]->rotateY(-90.0);
    viewingRotationMatrix[windowIndex]->rotateZ(-90.0);
}

/**
 * set to a left side view.
 * @param  windowID  Window for which view is requested
 *
 */
void
ModelController::leftView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
    viewingRotationMatrix[windowIndex]->rotateY(90.0);
    viewingRotationMatrix[windowIndex]->rotateZ(90.0);
}

/**
 * set to a anterior view.
 * @param  windowID  Window for which view is requested
 *
 */
void
ModelController::anteriorView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
    viewingRotationMatrix[windowIndex]->rotateX(-90.0);
    viewingRotationMatrix[windowIndex]->rotateY(180.0);}

/**
 * set to a posterior view.
 * @param  windowID  Window for which view is requested
 *
 */
void
ModelController::posteriorView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
    viewingRotationMatrix[windowIndex]->rotateX(-90.0);
}

/**
 * set to a dorsal view.
 * @param  windowID  Window for which view is requested
 *
 */
void
ModelController::dorsalView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
}

/**
 * set to a ventral view.
 * @param  windowID  Window for which view is requested
 *
 */
void
ModelController::ventralView(const CaretWindow& windowID)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    viewingRotationMatrix[windowIndex]->identity();
    viewingRotationMatrix[windowIndex]->rotateY(-180.0);
}

/**
 * Set the transformation.
 *
 * @param windowID  Window of this view.
 * @param transformationData - the transformation data:
 *          translation(3)
 *          viewing matrix[4][4],
 *          scaling(1)
 *
 */
void
ModelController::setTransformation(
                   const CaretWindow& windowID,
                   const std::vector<float>& transformationData)
{
    const int32_t windowIndex = windowID.getWindowIndex();
    uint32_t ctr = 0;
    
    if (transformationData.size() >= (ctr + 3)) {
        this->setTranslation(windowID,
                            transformationData[ctr + 0],
                            transformationData[ctr + 1],
                            transformationData[ctr + 2]);
        ctr += 3;
    }
    
    if (transformationData.size() >= (ctr + 16)) {
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {
                this->viewingRotationMatrix[windowIndex]->setMatrixElement(
                        i, j, transformationData[ctr]);
                ctr++;
            }
        }
    }
    
    if (transformationData.size() >= (ctr + 17)) {
        this->setScaling(windowID, transformationData[ctr]);
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
std::string
ModelController::toString() const
{
       return this->getNameForGUI(true);
}

/**
 * Returns a descriptive string containing info about this instance.
 *
 * @return  String describing contents of this instance.
 *
 */
std::string
ModelController::toDescriptiveString() const
{
    std::string s = CaretObject::toString();
    
    return s;
}

