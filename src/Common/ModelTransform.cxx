
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

#define __MODEL_TRANSFORM_DECLARE__
#include "ModelTransform.h"
#undef __MODEL_TRANSFORM_DECLARE__

#include <QStringList>

#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::ModelTransform 
 * \brief Translation, Rotation, and Scaling for a model.
 *
 * Translation, Rotation, and Scaling for a model.
 */
/**
 * Constructor.
 */
ModelTransform::ModelTransform()
: CaretObject()
{
    this->name = "";
    this->comment = "";
    
    setToIdentity();
}

/**
 * Destructor.
 */
ModelTransform::~ModelTransform()
{
    
}

/**
 * Set the view to the identity matrix.
 */
void
ModelTransform::setToIdentity()
{
    this->translation[0] = 0.0;
    this->translation[1] = 0.0;
    this->translation[2] = 0.0;
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            if (i == j) {
                this->rotation[i][j]        = 1.0;
                this->obliqueRotation[i][j] = 1.0;
                this->flatRotation[i][j]    = 1.0;
            }
            else {
                this->rotation[i][j]        = 0.0;
                this->obliqueRotation[i][j] = 0.0;
                this->flatRotation[i][j]    = 0.0;
            }
        }
    }
    
    this->rightCortexFlatMapOffsetXY[0] = 0.0;
    this->rightCortexFlatMapOffsetXY[1] = 0.0;
    this->rightCortexFlatMapZoomFactor  = 1.0;
    
    this->scaling = 1.0;    
}

/**
 * Copy constructor.
 * @param ModelTransform
 *   View that is copied.
 */
ModelTransform::ModelTransform(const ModelTransform& ModelTransform)
: CaretObject(ModelTransform)
{
    this->copyHelper(ModelTransform);
}

/**
 * Assignment operator.
 * @param ModelTransform
 *    View that is copied to this view.
 * @return 
 *    Reference to this object.
 */
ModelTransform& 
ModelTransform::operator=(const ModelTransform& ModelTransform)
{
    if (this != &ModelTransform) {
        CaretObject::operator=(ModelTransform);
        this->copyHelper(ModelTransform);
    }
    
    return *this;
}

/**
 * Less than operator.
 * @param view
 *    View compared to this view.
 * @return
 *    Returns result of a name comparison.
 */
bool 
ModelTransform::operator<(const ModelTransform& view) const
{
    return (this->name < view.name);
}

/**
 * Equality operator.
 * @param view
 *    View compared to this view.
 * @return
 *    Returns true if views have same name.
 */
bool
ModelTransform::operator==(const ModelTransform& view) const
{
    return (this->name == view.name);
}

/**
 * Get the translation
 * @param translation
 *   Output translation.
 */
void 
ModelTransform::getTranslation(float translation[3]) const
{
    translation[0] = this->translation[0];
    translation[1] = this->translation[1];
    translation[2] = this->translation[2];
}

/**
 * Get the rotation matrix.
 * @param rotation
 *   Output rotation matrix.
 */
void 
ModelTransform::getRotation(float rotation[4][4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            rotation[i][j] = this->rotation[i][j];
        }
    }
}

/**
 * Get the offset for drawing the right cortex flat map.
 *
 * @param rightCortexFlatMapOffsetX
 *     Output contining offset X.
 * @param rightCortexFlatMapOffsetX
 *     Output contining offset Y.
 */
void
ModelTransform::getRightCortexFlatMapOffset(float& rightCortexFlatMapOffsetX,
                                            float& rightCortexFlatMapOffsetY) const
{
    rightCortexFlatMapOffsetX = this->rightCortexFlatMapOffsetXY[0];
    rightCortexFlatMapOffsetY = this->rightCortexFlatMapOffsetXY[1];
}

/**
 * @return Zoom factor for right cortex flat map.
 */
float
ModelTransform::getRightCortexFlatMapZoomFactor() const
{
    return this->rightCortexFlatMapZoomFactor;
}


/**
 * Get the oblique rotation matrix.
 * @param obliqueRotation
 *   Output oblique rotation matrix.
 */
void
ModelTransform::getObliqueRotation(float obliqueRotation[4][4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            obliqueRotation[i][j] = this->obliqueRotation[i][j];
        }
    }
}

/**
 * Get the flat rotation matrix.
 * @param flatRotation
 *   Flat rotation matrix.
 */
void
ModelTransform::getFlatRotation(float flatRotation[4][4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            flatRotation[i][j] = this->flatRotation[i][j];
        }
    }
}

/**
 * @return The scaling.
 */
float 
ModelTransform::getScaling() const
{
    return this->scaling;
}

/**
 * @return Name of the view.
 */
AString 
ModelTransform::getName() const
{
    return this->name;
}

/**
 * Set the name of the view.
 * @param name
 *   New name for view.
 */
void 
ModelTransform::setName(const AString& name)
{
    this->name = name;
}

/**
 * @return Comment of the view.
 */
AString
ModelTransform::getComment() const
{
    return this->comment;
}

/**
 * Set the comment of the view.
 * @param comment
 *   New comment for view.
 */
void
ModelTransform::setComment(const AString& comment)
{
    this->comment = comment;
}

/**
 * Set the translation
 * @param translation
 *   New translation.
 */
void 
ModelTransform::setTranslation(const float translation[3])
{
    this->translation[0] = translation[0];
    this->translation[1] = translation[1];
    this->translation[2] = translation[2];
}

/**
 * Set the translation
 * @param translationX
 *   New translation X-value.
 * @param translationY
 *   New translation Y-value.
 * @param translationZ
 *   New translation Z-value.
 */
void
ModelTransform::setTranslation(const float translationX,
                         const float translationY,
                         const float translationZ)
{
    this->translation[0] = translationX;
    this->translation[1] = translationY;
    this->translation[2] = translationZ;
}


/**
 * Set the rotation matrix.
 * @param rotation
 *   New rotation matrix.
 */
void 
ModelTransform::setRotation(const float rotation[4][4])
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j] = rotation[i][j];
        }
    }
}

/**
 * Set the oblique rotation matrix.
 * @param obliqueRotation
 *   New oblique rotation matrix.
 */
void
ModelTransform::setObliqueRotation(const float obliqueRotation[4][4])
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->obliqueRotation[i][j] = obliqueRotation[i][j];
        }
    }
}

/**
 * Set the flat rotation
 * @param flatRotation
 *    New value for flat rotation
 */
void
ModelTransform::setFlatRotation(const float flatRotation[4][4])
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->flatRotation[i][j] = flatRotation[i][j];
        }
    }
}


/**
 * Set the offset for drawing the right cortex flat map.
 *
 * @param rightCortexFlatMapOffsetX
 *     Input contining offset X.
 * @param rightCortexFlatMapOffsetY
 *     Input contining offset Y.
 */
void
ModelTransform::setRightCortexFlatMapOffset(const float rightCortexFlatMapOffsetX,
                                            const float rightCortexFlatMapOffsetY)
{
    this->rightCortexFlatMapOffsetXY[0] = rightCortexFlatMapOffsetX;
    this->rightCortexFlatMapOffsetXY[1] = rightCortexFlatMapOffsetY;
}

/**
 * Set the right cortex flat map zoom factor.
 * 
 * @param rightCortexFlatMapZoomFactor
 *    Zoom factor for right cortex flat map.
 */
void
ModelTransform::setRightCortexFlatMapZoomFactor(const float rightCortexFlatMapZoomFactor)
{
    this->rightCortexFlatMapZoomFactor = rightCortexFlatMapZoomFactor;
}

/**
 * Set the scaling
 * @param scaling
 *    New value for scaling.
 */
void 
ModelTransform::setScaling(const float scaling)
{
    this->scaling = scaling;
}

/**
 * Returns the user view in a string that contains,
 * separated by commas: View Name, comment, translation[3],
 * rotation[4][4], scaling, obliqueRotation[4][4],
 * flatRotation,
 * and rightCortexFlatMapOffset[2], rightCortextFlatMapZoom
 */
AString 
ModelTransform::getAsString() const
{
    AString s = (this->name
                 + s_separatorInPreferences + this->comment
                 + s_separatorInPreferences + AString::number(this->translation[0])
                 + s_separatorInPreferences + AString::number(this->translation[1])
                 + s_separatorInPreferences + AString::number(this->translation[2]));
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->rotation[i][j]));
        }
    }
    
    s += (s_separatorInPreferences + AString::number(this->scaling));
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->obliqueRotation[i][j]));
        }
    }
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->flatRotation[i][j]));
        }
    }

    s += (s_separatorInPreferences + AString::number(this->rightCortexFlatMapOffsetXY[0]));
    s += (s_separatorInPreferences + AString::number(this->rightCortexFlatMapOffsetXY[1]));
    s += (s_separatorInPreferences + AString::number(this->rightCortexFlatMapZoomFactor));
    
    return s;
}

/**
 * Set the user view from a string that contains,
 * separated by commas: View Name, comment, translation[3],
 * rotation[4][4], scaling, obliqueRotation[4][4],
 * flatRotation,
 * and rightCortexFlatMapOffset[2], rightCortextFlatMapZoom
 */
bool 
ModelTransform::setFromString(const AString& s)
{
    bool hasComment = false;
    bool hasObliqueRotation = false;
    bool hasFlatRotation = false;
    bool hasRightFlatMapOffset = false;
    bool hasRightFlatMapZoomFactor = false;
    
    QStringList sl;
    if (s.contains(s_separatorInPreferences)) {
        sl = s.split(s_separatorInPreferences,
                                 Qt::KeepEmptyParts);
        const int numElements = sl.count();
        
        if (numElements == 57) {
            hasComment = true;
            hasObliqueRotation = true;
            hasFlatRotation = true;
            hasRightFlatMapOffset = true;
            hasRightFlatMapZoomFactor = true;
        }
        else if (numElements == 41) {
            hasComment = true;
            hasObliqueRotation = true;
            hasRightFlatMapOffset = true;
            hasRightFlatMapZoomFactor = true;
        }
        else if (numElements == 40) {
            hasComment = true;
            hasObliqueRotation = true;
            hasRightFlatMapOffset = true;
        }
        else if (numElements == 38) {
            hasComment = true;
            hasObliqueRotation = true;
        }
        else if (numElements == 22) {
            hasComment = true;
        }
        else {
            CaretLogSevere("User view string does not contain 22, 38, 40, 41, or 42 elements");
            return false;
        }
    }
    else {
        sl = s.split(",", Qt::KeepEmptyParts);
        const int numElements = sl.count();
        if (numElements != 21) {
            CaretLogSevere("User view string does not contain 21 elements");
            return false;
        }
    }
    
    int ctr = 0;
    this->name = sl.at(ctr++);
    if (hasComment) {
        this->comment = sl.at(ctr++);
    }
    else {
        this->comment = "";
    }
    this->translation[0] = sl.at(ctr++).toFloat();
    this->translation[1] = sl.at(ctr++).toFloat();
    this->translation[2] = sl.at(ctr++).toFloat();
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j] = sl.at(ctr++).toFloat();
        }
    }
    
    this->scaling = sl.at(ctr++).toFloat();

    if (hasObliqueRotation) {
        for (int32_t i = 0; i < 4; i++) {
            for (int32_t j = 0; j < 4; j++) {
                this->obliqueRotation[i][j] = sl.at(ctr++).toFloat();
            }
        }
    }
    else {
        for (int32_t i = 0; i < 4; i++) {
            for (int32_t j = 0; j < 4; j++) {
                if (i == j) {
                    this->obliqueRotation[i][j] = 1.0;
                }
                else {
                    this->obliqueRotation[i][j] = 0.0;
                }
            }
        }
    }
    
    if (hasFlatRotation) {
        for (int32_t i = 0; i < 4; i++) {
            for (int32_t j = 0; j < 4; j++) {
                this->flatRotation[i][j] = sl.at(ctr++).toFloat();
            }
        }
    }
    else {
        for (int32_t i = 0; i < 4; i++) {
            for (int32_t j = 0; j < 4; j++) {
                if (i == j) {
                    this->flatRotation[i][j] = 1.0;
                }
                else {
                    this->flatRotation[i][j] = 0.0;
                }
            }
        }
    }
    
    if (hasRightFlatMapOffset) {
        this->rightCortexFlatMapOffsetXY[0] = sl.at(ctr++).toFloat();
        this->rightCortexFlatMapOffsetXY[1] = sl.at(ctr++).toFloat();
    }
    else {
        this->rightCortexFlatMapOffsetXY[0] = 0;
        this->rightCortexFlatMapOffsetXY[1] = 0;
    }
    
    if (hasRightFlatMapZoomFactor) {
        this->rightCortexFlatMapZoomFactor = sl.at(ctr++).toFloat();
    }
    else {
        this->rightCortexFlatMapZoomFactor = 1.0;
    }
    return true;
}

/**
 * Copy all data from the given user view to this user view.
 * @param ModelTransform
 *    View from which data is copied.
 */
void 
ModelTransform::copyHelper(const ModelTransform& modelTransform)
{
    this->name = modelTransform.name;
    this->comment = modelTransform.comment;
    
    this->translation[0] = modelTransform.translation[0];
    this->translation[1] = modelTransform.translation[1];
    this->translation[2] = modelTransform.translation[2];
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j]        = modelTransform.rotation[i][j];
            this->obliqueRotation[i][j] = modelTransform.obliqueRotation[i][j];
            this->flatRotation[i][j]    = modelTransform.flatRotation[i][j];
        }
    }
    
    this->scaling = modelTransform.scaling;
    
    this->rightCortexFlatMapOffsetXY[0] = modelTransform.rightCortexFlatMapOffsetXY[0];
    this->rightCortexFlatMapOffsetXY[1] = modelTransform.rightCortexFlatMapOffsetXY[1];
    
    this->rightCortexFlatMapZoomFactor = modelTransform.rightCortexFlatMapZoomFactor;
}

/**
 * Set panning, rotation, oblique rotation, and zoom.
 *
 * @param panX
 *    X-Panning.
 * @param panY
 *    Y-Panning.
 * @param panZ
 *    Z-Panning.
 * @param rotationMatrix
 *    4x4 rotation matrix.
 * @param obliqueRotationMatrix
 *    4x4 oblique rotation matrix.
 * @param floatRotationMatrixArray
 *    The flat rotation matrix
 * @param zoom
 *    Zooming.
 * @param rightCortexFlatMapOffsetX
 *    Offset X for right cortex flat map.
 * @param rightCortexFlatMapOffsetY
 *    Offset Y for right cortex flat map.
 * @param rightCortexFlatMapZoomFactor
 *    Zoom factor for right cortex flat map.
 */
void
ModelTransform::setPanningRotationMatrixAndZoom(const float panX,
                                                const float panY,
                                                const float panZ,
                                                const float rotationMatrix[4][4],
                                                const float obliqueRotationMatrix[4][4],
                                                const float floatRotationMatrixArray[4][4],
                                                const float zoom,
                                                const float rightCortexFlatMapOffsetX,
                                                const float rightCortexFlatMapOffsetY,
                                                const float rightCortexFlatMapZoomFactor)
{
    this->setTranslation(panX, panY, panZ);

    setRotation(rotationMatrix);
    
    setObliqueRotation(obliqueRotationMatrix);
    
    this->setFlatRotation(floatRotationMatrixArray);
    
    this->setScaling(zoom);
    
    this->setRightCortexFlatMapOffset(rightCortexFlatMapOffsetX,
                                      rightCortexFlatMapOffsetY);
    
    this->setRightCortexFlatMapZoomFactor(rightCortexFlatMapZoomFactor);
}

/**
 * Get pan, rotation, oblique rotation, and zoom.
 *
 * @param panX
 *    X-Panning.
 * @param panY
 *    Y-Panning.
 * @param rotationMatrix
 *    4x4 rotation matrix.
 * @param obliqueRotationMatrix
 *    4x4 oblique rotation matrix.
 * @param floatRotationMatrixArray
 *    The flat rotation matrix
 * @param zoom
 *    Zooming.
 * @param rightCortexFlatMapOffsetX
 *    Offset X for right cortex flat map.
 * @param rightCortexFlatMapOffsetY
 *    Offset Y for right cortex flat map.
 * @param rightCortexFlatMapZoomFactor
 *    Zoom factor for right cortex flat map.
 */
void
ModelTransform::getPanningRotationMatrixAndZoom(float& panX,
                                                float& panY,
                                                float& panZ,
                                                float rotationMatrix[4][4],
                                                float obliqueRotationMatrix[4][4],
                                                float floatRotationMatrixArray[4][4],
                                                float& zoom,
                                                float& rightCortexFlatMapOffsetX,
                                                float& rightCortexFlatMapOffsetY,
                                                float& rightCortexFlatMapZoomFactor) const
{
    panX = this->translation[0];
    panY = this->translation[1];
    panZ = this->translation[2];
    
    getRotation(rotationMatrix);
    
    getObliqueRotation(obliqueRotationMatrix);
    
    getFlatRotation(floatRotationMatrixArray);
    
    zoom = getScaling();
    
    getRightCortexFlatMapOffset(rightCortexFlatMapOffsetX,
                                rightCortexFlatMapOffsetY);
    
    rightCortexFlatMapZoomFactor = this->getRightCortexFlatMapZoomFactor();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ModelTransform::toString() const
{
    return ("ModelTransform: " + this->getAsString());
}
