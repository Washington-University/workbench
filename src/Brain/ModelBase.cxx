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


#include "ModelBase.h"

#include "Brain.h"
#include "CaretConstants.h"
#include "CaretAssert.h"
#include "Matrix4x4.h"

using namespace caret;

/**
 * Constructor.
 * @param brain Brain to which this controller belongs.
 * @param allowsYokingFlag  This controller can be yoked.
 * @param allowsRotationFlag This controller can be rotated.
 *
 */
ModelBase::ModelBase(Brain* brain,
               const bool allowsYokingFlag,
               const bool allowsRotationFlag)
    : CaretObject()
{
    this->initializeMembersModelBase();
    this->brain = brain;
    this->initializeTransformations();
    this->allowsRotationFlag = allowsRotationFlag;
    this->allowsYokingFlag   = allowsYokingFlag;
}

/**
 * Destructor
 */
ModelBase::~ModelBase()
{
    for (uint64_t i = 0; i < this->viewingRotationMatrix.size(); i++) {
        delete this->viewingRotationMatrix[i];
    }
}

void
ModelBase::initializeMembersModelBase()
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
ModelBase::getBrain() const
{
    return this->brain;
}

/**
 * Initialize the transformation matrices.
 *
 */
void
ModelBase::initializeTransformations()
{
    this->viewingRotationMatrix.reserve(CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    this->translation.reserve(CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS * 3);
    this->scaling.reserve(CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    for (int32_t i = 0; i < CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->viewingRotationMatrix.push_back(new Matrix4x4());
        this->scaling.push_back(this->defaultModelScaling);
    }
    for (int32_t i = 0; i < (CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS * 3); i++) {
        this->translation.push_back(0.0f);
    }
    
    for (int32_t i = 0; i < CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->resetView(i);
    }
}



/**
 * See if this controller allows rotation.
 * 
 * @return true if this controller allows rotation, else false.
 *
 */
bool
ModelBase::isRotationAllowed() const
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
ModelBase::isYokeable() const
{
    return this->allowsYokingFlag;
}

/**
 * For a structure model, copy the transformations from one window of
 * the structure model to another window.
 *
 * @param controller           Source structure model
 * @param windowTabNumberSource   windowTabNumber of source transformation.
 * @param windowTabNumberTarget   windowTabNumber of target transformation.
 *
 */
void
ModelBase::copyTransformations(
                   const ModelBase& controller,
                   const int32_t windowTabNumberSource,
                   const int32_t windowTabNumberTarget)
{
    if (windowTabNumberSource == windowTabNumberTarget) {
        return;
    }
    
    const int32_t it3 = windowTabNumberTarget * 3;
    const int32_t is3 = windowTabNumberSource * 3;
    
    CaretAssertVectorIndex(controller.translation, is3+2);
    CaretAssertVectorIndex(this->translation, it3 + 2);
    
    this->translation[it3] = controller.translation[is3];
    this->translation[it3+1] = controller.translation[is3+1];
    this->translation[it3+2] = controller.translation[is3+2];
    this->scaling[windowTabNumberTarget] = controller.scaling[windowTabNumberSource];
    
    this->viewingRotationMatrix[windowTabNumberTarget]->setMatrix(*controller.getViewingRotationMatrix(windowTabNumberSource));
}

/**
 * the viewing rotation matrix.
 *
 * @param  windowTabNumber  Window for which rotation is requested
 * @return Reference to the viewing rotation matrix.
 *
 */
Matrix4x4*
ModelBase::getViewingRotationMatrix(const int32_t windowTabNumber) const
{
    return this->viewingRotationMatrix[windowTabNumber];
}

/**
 * get the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @return  The translation, an array of three floats.
 *
 */
const float*
ModelBase::getTranslation(const int32_t windowTabNumber) const
{
    const int32_t i3 = windowTabNumber * 3;
    return &this->translation[i3];
}

/**
 * set the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  t  The translation, an array of three floats.
 *
 */
void
ModelBase::setTranslation(
                   const int32_t windowTabNumber,
                   const float t[])
{
    const int32_t i3 = windowTabNumber * 3;
    this->translation[i3]   = t[0];
    this->translation[i3+1] = t[1];
    this->translation[i3+2] = t[2];
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
ModelBase::setTranslation(
                   const int32_t windowTabNumber,
                   const float tx,
                   const float ty,
                   const float tz)
{
    const int32_t i3 = windowTabNumber * 3;
    this->translation[i3]   = tx;
    this->translation[i3+1] = ty;
    this->translation[i3+2] = tz;
}

/**
 * get the scaling.
 *
 * @param  windowTabNumber  Window for which scaling is requested
 * @return  Scaling value.
 *
 */
float
ModelBase::getScaling(const int32_t windowTabNumber) const
{
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
ModelBase::setScaling(
                   const int32_t windowTabNumber,
                   const float s)
{
    this->scaling[windowTabNumber] = s;
}

/**
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 *
 */
void
ModelBase::resetView(const int32_t windowTabNumber)
{
    this->setTranslation(windowTabNumber, 0.0f, 0.0f, 0.0f);
    this->viewingRotationMatrix[windowTabNumber]->identity();
    this->setScaling(windowTabNumber, this->defaultModelScaling);
}

/**
 * @param  windowTabNumber  Window for which view is requested
 * set to a right side view.
 *
 */
void
ModelBase::rightView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
    viewingRotationMatrix[windowTabNumber]->rotateY(-90.0);
    viewingRotationMatrix[windowTabNumber]->rotateZ(-90.0);
}

/**
 * set to a left side view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelBase::leftView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
    viewingRotationMatrix[windowTabNumber]->rotateY(90.0);
    viewingRotationMatrix[windowTabNumber]->rotateZ(90.0);
}

/**
 * set to a anterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelBase::anteriorView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
    viewingRotationMatrix[windowTabNumber]->rotateX(-90.0);
    viewingRotationMatrix[windowTabNumber]->rotateY(180.0);}

/**
 * set to a posterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelBase::posteriorView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
    viewingRotationMatrix[windowTabNumber]->rotateX(-90.0);
}

/**
 * set to a dorsal view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelBase::dorsalView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
}

/**
 * set to a ventral view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
ModelBase::ventralView(const int32_t windowTabNumber)
{
    viewingRotationMatrix[windowTabNumber]->identity();
    viewingRotationMatrix[windowTabNumber]->rotateY(-180.0);
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
ModelBase::setTransformation(
                   const int32_t windowTabNumber,
                   const std::vector<float>& transformationData)
{
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
                this->viewingRotationMatrix[windowTabNumber]->setMatrixElement(
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
ModelBase::toString() const
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
ModelBase::toDescriptiveString() const
{
    AString s = CaretObject::toString();
    
    return s;
}

