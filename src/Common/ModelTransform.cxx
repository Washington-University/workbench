
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "Matrix4x4Interface.h"

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
    
    this->mprTwoRotationAngles[0] = 0.0;
    this->mprTwoRotationAngles[1] = 0.0;
    this->mprTwoRotationAngles[2] = 0.0;

    this->mprThreeRotationAngles[0] = 0.0;
    this->mprThreeRotationAngles[1] = 0.0;
    this->mprThreeRotationAngles[2] = 0.0;
    
    this->mprThreeSingleSliceRotationAngle = 0.0;
    
    this->rightCortexFlatMapOffsetXY[0] = 0.0;
    this->rightCortexFlatMapOffsetXY[1] = 0.0;
    this->rightCortexFlatMapZoomFactor  = 1.0;
    
    this->scaling = 1.0;    
}

/**
 * Load an array with the identity quaternion (scalar, x, y, z)
 * @param quaternionSXYZ
 *    Array loaded with identity quaterion (1, 0, 0, 0)
 */
void
ModelTransform::getIdentityQuaternion(float quaternionSXYZ[4])
{
    quaternionSXYZ[0] = 1.0;
    quaternionSXYZ[1] = 0.0;
    quaternionSXYZ[2] = 0.0;
    quaternionSXYZ[3] = 0.0;
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
 * Get the MPR  Two rotation angles
 * @param mprTwoRotationAngles
 *   Output mpr two rotatation angles
 */
void
ModelTransform::getMprTwoRotationAngles(float mprTwoRotationAngles[3]) const
{
    mprTwoRotationAngles[0] = this->mprTwoRotationAngles[0];
    mprTwoRotationAngles[1] = this->mprTwoRotationAngles[1];
    mprTwoRotationAngles[2] = this->mprTwoRotationAngles[2];
}

/**
 * Get the MPR Three rotation angles
 * @param mprThreeRotationAngles
 *   Output mpr three rotatation angles
 */
void
ModelTransform::getMprThreeRotationAngles(float mprThreeRotationAngles[3]) const
{
    mprThreeRotationAngles[0] = this->mprThreeRotationAngles[0];
    mprThreeRotationAngles[1] = this->mprThreeRotationAngles[1];
    mprThreeRotationAngles[2] = this->mprThreeRotationAngles[2];
}

/**
 * @return The MPR three single slice rotation angle
 */
float
ModelTransform::getMprThreeSingleSliceRotationAngle() const
{
    return this->mprThreeSingleSliceRotationAngle;
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
 * Set the MPR Two rotation angles
 * @param mprTwoRotationAngles
 *   New MPR two rotation angles
 */
void
ModelTransform::setMprTwoRotationAngles(const float mprTwoRotationAngles[3])
{
    this->mprTwoRotationAngles[0] = mprTwoRotationAngles[0];
    this->mprTwoRotationAngles[1] = mprTwoRotationAngles[1];
    this->mprTwoRotationAngles[2] = mprTwoRotationAngles[2];
}

/**
 * Set the MPR Three rotation angles
 * @param mprThreeRotationAngles
 *   New MPR three rotation angles
 */
void
ModelTransform::setMprThreeRotationAngles(const float mprThreeRotationAngles[3])
{
    this->mprThreeRotationAngles[0] = mprThreeRotationAngles[0];
    this->mprThreeRotationAngles[1] = mprThreeRotationAngles[1];
    this->mprThreeRotationAngles[2] = mprThreeRotationAngles[2];
}

/**
 * Set the MPR Three single slice rotation angle
 * @param angle
 *   New angle
 */
void
ModelTransform::setMprThreeSingleSliceRotationAngle(const float angle)
{
    this->mprThreeSingleSliceRotationAngle = angle;
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
 * @param Matrix4x4Interface& matrixForCalculations
 *    A matrix that is used for calculations (matrix to rotation angles).  Calling function
 *    must provide an instance of Matrix4x4.
 * @param leftRotationMatrix
 *    The left rotation matrix
 * @return String with each transform values notated for human viewing
 */
AString
ModelTransform::getAsPrettyString(Matrix4x4Interface& matrixForCalculations,
                                  const float leftRotationMatrix[4][4]) const
{
    AString s;
    
    s.appendWithNewLine("Translation: " + AString::fromNumbers(this->translation, 3));
    s.appendWithNewLine(" ");
    
    matrixForCalculations.setMatrix(this->rotation);
    double rotXYZ[3];
    matrixForCalculations.getRotation(rotXYZ[0], rotXYZ[1], rotXYZ[2]);
    
    s.appendWithNewLine("Rotation Angles: "
                        + AString::fromNumbers(rotXYZ, 3));
    s.appendWithNewLine(" " );
    
    s.appendWithNewLine("Right Rotation Matrix:");
    for (int32_t i = 0; i < 4; i++) {
        s.appendWithNewLine(AString::fromNumbers(this->rotation[i], 4));
    }
    s.appendWithNewLine(" ");

    s.appendWithNewLine("Left Rotation Matrix:");
    for (int32_t i = 0; i < 4; i++) {
        s.appendWithNewLine(AString::fromNumbers(leftRotationMatrix[i], 4));
    }
    s.appendWithNewLine(" ");
    
    s.appendWithNewLine("Scaling: " + AString::number(this->scaling));
    s.appendWithNewLine(" ");

    matrixForCalculations.setMatrix(this->obliqueRotation);
    matrixForCalculations.getRotation(rotXYZ[0], rotXYZ[1], rotXYZ[2]);
    
    s.appendWithNewLine("Oblique Rotation Angles: "
                        + AString::fromNumbers(rotXYZ, 3));
    s.appendWithNewLine(" " );
    
    s.appendWithNewLine("Oblique Rotation Matrix:");
    for (int32_t i = 0; i < 4; i++) {
        s.appendWithNewLine(AString::fromNumbers(this->obliqueRotation[i], 4));
    }
    s.appendWithNewLine(" ");

    s.appendWithNewLine("MPR Two Rotation Angles: "
                        + AString::fromNumbers(this->mprTwoRotationAngles, 3));
    s.appendWithNewLine(" ");

    s.appendWithNewLine("MPR Three Rotation Angles: "
                        + AString::fromNumbers(this->mprThreeRotationAngles, 3));
    s.appendWithNewLine(" ");
    
    matrixForCalculations.identity();
    matrixForCalculations.setRotation(this->mprThreeRotationAngles[0],
                                      this->mprThreeRotationAngles[1],
                                      this->mprThreeRotationAngles[2]);
    float mprThreeElements[4][4];
    matrixForCalculations.getMatrix(mprThreeElements);
    s.appendWithNewLine("MPR Three Rotation Matrix:");
    for (int32_t i = 0; i < 4; i++) {
        s.appendWithNewLine(AString::fromNumbers(mprThreeElements[i], 4));
    }
    s.appendWithNewLine(" ");

    s.appendWithNewLine("MPR Three Single Slice Rotation Angle: "
                        + AString::number(this->mprThreeSingleSliceRotationAngle));
    s.appendWithNewLine(" ");

    matrixForCalculations.setMatrix(this->flatRotation);
    matrixForCalculations.getRotation(rotXYZ[0], rotXYZ[1], rotXYZ[2]);
    
    s.appendWithNewLine("Flat Rotation Angles: "
                        + AString::fromNumbers(rotXYZ, 3));
    s.appendWithNewLine(" " );

    s.appendWithNewLine("Flat Rotation Matrix:");
    for (int32_t i = 0; i < 4; i++) {
        s.appendWithNewLine(AString::fromNumbers(this->flatRotation[i], 4));
    }
    s.appendWithNewLine(" ");

    s.appendWithNewLine("Right Cortex Flat Map Offset: "
                        + AString::fromNumbers(this->rightCortexFlatMapOffsetXY, 2));
    s.appendWithNewLine(" ");

    s.appendWithNewLine("Right Cortex Flat Map Zoom: "
                        + AString::number(this->rightCortexFlatMapZoomFactor));
    s.appendWithNewLine(" ");

    return s;
}

/**
 * Returns the user view in a string that contains,
 * separated by commas: View Name, comment, translation[3],
 * rotation[4][4], scaling, obliqueRotation[4][4],
 * mprTwoRotation[3], mprThreeRotation[3]
 * flatRotation,
 * and rightCortexFlatMapOffset[2], rightCortextFlatMapZoom
 */
AString 
ModelTransform::getAsString() const
{
    /* 5 */
    AString s = (this->name
                 + s_separatorInPreferences + this->comment
                 + s_separatorInPreferences + AString::number(this->translation[0])
                 + s_separatorInPreferences + AString::number(this->translation[1])
                 + s_separatorInPreferences + AString::number(this->translation[2]));
    
    /* 5 + 16 = 21 */
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->rotation[i][j]));
        }
    }
    
    /* 21 + 1 = 22 */
    s += (s_separatorInPreferences + AString::number(this->scaling));
    
    /* 22 + 16 = 38 */
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->obliqueRotation[i][j]));
        }
    }
    
    /* 38 + 3 = 41 */
    for (int32_t i = 0; i < 3; i++) {
        s += (s_separatorInPreferences + AString::number(this->mprTwoRotationAngles[i]));
    }
    
    /* 41 + 3 = 44 */
    for (int32_t i = 0; i < 3; i++) {
        s += (s_separatorInPreferences + AString::number(this->mprThreeRotationAngles[i]));
    }
    
    /* 44 + 1 = 45 */
    s += (s_separatorInPreferences + AString::number(this->mprThreeSingleSliceRotationAngle));
    
    /* 45 + 16 = 61 */
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += (s_separatorInPreferences + AString::number(this->flatRotation[i][j]));
        }
    }

    /* 61 + 3 = 64 */
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
    bool hasMprAngles = false;
    bool hasMprThreeAngles = false;
    bool hasMprThreeSingleSliceAngle = false;
    bool hasFlatRotation = false;
    bool hasRightFlatMapOffset = false;
    bool hasRightFlatMapZoomFactor = false;
    
    QStringList sl;
    if (s.contains(s_separatorInPreferences)) {
#if QT_VERSION >= 0x060000
        sl = s.split(s_separatorInPreferences,
                     Qt::KeepEmptyParts);
#else
        sl = s.split(s_separatorInPreferences,
                                 QString::KeepEmptyParts);
#endif
        const int numElements = sl.count();
        
        if (numElements == 64) {
            hasComment = true;
            hasObliqueRotation = true;
            hasMprAngles = true;
            hasMprThreeAngles = true;
            hasMprThreeSingleSliceAngle;
            hasFlatRotation = true;
            hasRightFlatMapOffset = true;
            hasRightFlatMapZoomFactor = true;
        }
        else if (numElements == 63) {
            hasComment = true;
            hasObliqueRotation = true;
            hasMprAngles = true;
            hasMprThreeAngles = true;
            hasFlatRotation = true;
            hasRightFlatMapOffset = true;
            hasRightFlatMapZoomFactor = true;
        }
        else if (numElements == 60) {
            hasComment = true;
            hasObliqueRotation = true;
            hasMprAngles = true;
            hasFlatRotation = true;
            hasRightFlatMapOffset = true;
            hasRightFlatMapZoomFactor = true;
        }
        else if (numElements == 57) {
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
            CaretLogSevere("User view string does not contain 22, 38, 40, 41, ,57, or 60 elements");
            return false;
        }
    }
    else {
#if QT_VERSION >= 0x060000
        sl = s.split(",", Qt::KeepEmptyParts);
#else
        sl = s.split(",", QString::KeepEmptyParts);
#endif
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
    
    if (hasMprAngles) {
        for (int32_t j = 0; j < 3; j++) {
            this->mprTwoRotationAngles[j] = sl.at(ctr++).toFloat();
        }
    }
    else {
        for (int32_t i = 0; i < 3; i++) {
            this->mprTwoRotationAngles[i] = 0.0;
        }
    }
    
    if (hasMprThreeAngles) {
        for (int32_t j = 0; j < 3; j++) {
            this->mprThreeRotationAngles[j] = sl.at(ctr++).toFloat();
        }
    }
    else {
        /*
         * Before MPR three angles were added, the MPR angles were
         * used for both MPR Two and MPR Three Angles
         */
        for (int32_t j = 0; j < 3; j++) {
            this->mprThreeRotationAngles[j] = mprTwoRotationAngles[j];
        }
    }

    if (hasMprThreeSingleSliceAngle) {
        this->mprThreeSingleSliceRotationAngle = sl.at(ctr++).toFloat();
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
    
    this->mprTwoRotationAngles[0] = modelTransform.mprTwoRotationAngles[0];
    this->mprTwoRotationAngles[1] = modelTransform.mprTwoRotationAngles[1];
    this->mprTwoRotationAngles[2] = modelTransform.mprTwoRotationAngles[2];

    this->mprThreeRotationAngles[0] = modelTransform.mprThreeRotationAngles[0];
    this->mprThreeRotationAngles[1] = modelTransform.mprThreeRotationAngles[1];
    this->mprThreeRotationAngles[2] = modelTransform.mprThreeRotationAngles[2];
    
    this->mprThreeSingleSliceRotationAngle = modelTransform.mprThreeSingleSliceRotationAngle;
    
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
 * @param mprTwoRotationAngles
 *    The MPR two rotation angles
 * @param mprThreeRotationAngles
 *    The MPR three rotation angles
 * @param mprThreeSingleSliceRotationAngle
 *    The MPR three single slice rotation angle
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
                                                const float mprTwoRotationAngles[3],
                                                const float mprThreeRotationAngles[3],
                                                const float mprThreeSingleSliceRotationAngle,
                                                const float floatRotationMatrixArray[4][4],
                                                const float zoom,
                                                const float rightCortexFlatMapOffsetX,
                                                const float rightCortexFlatMapOffsetY,
                                                const float rightCortexFlatMapZoomFactor)
{
    this->setTranslation(panX, panY, panZ);

    setRotation(rotationMatrix);
    
    setObliqueRotation(obliqueRotationMatrix);
    
    setMprTwoRotationAngles(mprTwoRotationAngles);
    
    setMprThreeRotationAngles(mprThreeRotationAngles);
    
    setMprThreeSingleSliceRotationAngle(mprThreeSingleSliceRotationAngle);
    
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
 * @param mprTwoRotationAngles
 *    The MPR two rotation angles
 * @param mprThreeRotationAngles
 *    The MPR three rotation angles
 * @param mprThreeSingleSliceRotationAngle
 *    The MPR three single slice rotation angle
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
                                                float mprTwoRotationAngles[3],
                                                float mprThreeRotationAngles[3],
                                                float& mprThreeSingleSliceRotationAngle,
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
    
    getMprTwoRotationAngles(mprTwoRotationAngles);
    
    getMprThreeRotationAngles(mprThreeRotationAngles);
    
    mprThreeSingleSliceRotationAngle = getMprThreeSingleSliceRotationAngle();
    
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
