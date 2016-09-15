
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CONTROL_POINT_FILE_DECLARE__
#include "ControlPointFile.h"
#undef __CONTROL_POINT_FILE_DECLARE__

#include "CaretAssert.h"
#include "ControlPoint3D.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"
#include "Matrix4x4.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ControlPointFile 
 * \brief File containing control points used for data transformations.
 * \ingroup Files
 */

/**
 * Constructor.
 */
ControlPointFile::ControlPointFile()
: CaretDataFile(DataFileTypeEnum::UNKNOWN)
{
    m_metadata.grabNew(new GiftiMetaData());
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_landmarkTransformationMatrix.grabNew(new Matrix4x4());

//    addControlPoint(ControlPoint3D(151, 147, 0, -42, -81, 6));
//    addControlPoint(ControlPoint3D(263, 126, 0, -10, -87, 6));
//    addControlPoint(ControlPoint3D(250, 491, 0, -13, 20, 6));

//    addControlPoint(ControlPoint3D(48, 105, 0, -58, -13, 11));
//    addControlPoint(ControlPoint3D(140, 106, 0, -12, -13, 13));
//    addControlPoint(ControlPoint3D(133, 196, 0, -17, -13, 58));
}

/**
 * Destructor.
 */
ControlPointFile::~ControlPointFile()
{
    clearPrivate();
}

/**
 * @return True if this file is empty, else false.
 */
bool
ControlPointFile::isEmpty() const
{
    return m_controlPoints.empty();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
ControlPointFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
ControlPointFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
ControlPointFile::getFileMetaData()
{
    return m_metadata.getPointer();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
ControlPointFile::getFileMetaData() const
{
    return m_metadata.getPointer();
}

/**
 * Clear the content of this file.
 * This method is virtual so do not call from constructor/destructor.
 */
void
ControlPointFile::clear()
{
    const AString nameOfFile = getFileName();
    
    CaretDataFile::clear();
    
    clearPrivate();
}

/**
 * Clear the content of this file.
 */
void
ControlPointFile::clearPrivate()
{
    m_metadata->clear();

    removeAllControlPoints();
    
    m_landmarkTransformationMatrix->identity();
}


/**
 * Add information about the content of this file.
 *
 * @param dataFileInformation
 *     Will contain information about this file.
 */
void
ControlPointFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
}

/**
 * @return true if file is modified, else false.
 */
bool
ControlPointFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    if (m_metadata->isModified()) {
        return true;
    }
    
    for (std::vector<ControlPoint3D*>::const_iterator iter = m_controlPoints.begin();
         iter != m_controlPoints.end();
         iter++) {
        if ((*iter)->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status of this file.
 */
void
ControlPointFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_metadata->clearModified();
    
    for (std::vector<ControlPoint3D*>::const_iterator iter = m_controlPoints.begin();
         iter != m_controlPoints.end();
         iter++) {
        (*iter)->clearModified();
    }
    
    m_landmarkTransformationMatrix->clearModified();
}

/**
 * @return The number of control points.
 */
int32_t
ControlPointFile::getNumberOfControlPoints() const
{
    return m_controlPoints.size();
}

/**
 * Add a control point.
 *
 * @param controlPoint
 *     Control point to add.
 */
void
ControlPointFile::addControlPoint(const ControlPoint3D& controlPoint)
{
    m_controlPoints.push_back(new ControlPoint3D(controlPoint));
    setModified();
}

/**
 * @return Control point at the given index (const method).
 *
 * @param index
 *     Index of the control point.
 */
const ControlPoint3D*
ControlPointFile::getControlPointAtIndex(const int32_t index) const
{
    CaretAssertVectorIndex(m_controlPoints, index);
    return m_controlPoints[index];
}

/**
 * @return Control point at the given index.
 *
 * @param index
 *     Index of the control point.
 */
ControlPoint3D*
ControlPointFile::getControlPointAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_controlPoints, index);
    return m_controlPoints[index];
}

/**
 * Remove all of the control points.
 */
void
ControlPointFile::removeAllControlPoints()
{
    for (std::vector<ControlPoint3D*>::iterator iter = m_controlPoints.begin();
         iter != m_controlPoints.end();
         iter++) {
        delete *iter;
    }
    m_controlPoints.clear();
}

/**
 * Remote the control point at the given index.
 *
 * @param index
 *     Index of the control point.
 */
void
ControlPointFile::removeControlPointAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_controlPoints, index);
    delete m_controlPoints[index];
    m_controlPoints.erase(m_controlPoints.begin() + index);
    setModified();
}

/**
 * Update the landmark transformation matrix from the control points.
 * The transformed position will be set in the control points if the
 * matrix is successfully created.
 *
 * @param errorMessageOutk
 *     Contains error information upon exit.
 * @return True if landmark transformation matrix was successfully update,
 *     else false.
 */
bool
ControlPointFile::updateLandmarkTransformationMatrix(AString& errorMessageOut)
{
    m_landmarkTransformationMatrix->identity();
    errorMessageOut.clear();
    
    bool successFlag = m_landmarkTransformationMatrix->createLandmarkTransformMatrix(m_controlPoints,
                                                                                     errorMessageOut);
    
    const int32_t numCP = getNumberOfControlPoints();
    for (int32_t icp = 0; icp < numCP; icp++) {
        float transformedXYZ[3] = { 0.0, 0.0, 0.0 };
        
        CaretAssertVectorIndex(m_controlPoints, icp);
        ControlPoint3D* cp = m_controlPoints[icp];
        if (successFlag) {
            cp->getSourceXYZ(transformedXYZ);
            m_landmarkTransformationMatrix->multiplyPoint3(transformedXYZ);
        }
        cp->setTransformedXYZ(transformedXYZ);
    }
    
    return successFlag;
}

/**
 * @return The landmark transformation matrix.
 */
Matrix4x4*
ControlPointFile::getLandmarkTransformationMatrix()
{
    return m_landmarkTransformationMatrix;
}

/**
 * @return The landmark transformation matrix (const method).
 */
const Matrix4x4*
ControlPointFile::getLandmarkTransformationMatrix() const
{
    return m_landmarkTransformationMatrix;
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
ControlPointFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    throw DataFileException("Reading of Control Point files not implemented");
    
//    AnnotationFileXmlReader reader;
//    reader.readFile(filename,
//                    this);
//    
//    updateUniqueKeysAfterReadingFile();
    
    setFileName(filename);
    
    clearModified();
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
ControlPointFile::writeFile(const AString& filename)
{
    checkFileWritability(filename);
    
    throw DataFileException("Writing of Control Point files not implemented");
    
    setFileName(filename);
    
//    AnnotationFileXmlWriter writer;
//    writer.writeFile(this);
    
    clearModified();
}







/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ControlPointFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    std::vector<SceneClass*> controlPointsVector;
    
    const int32_t numCP = getNumberOfControlPoints();
    for (int32_t icp = 0; icp < numCP; icp++) {
        const QString name("ControlPoint" + QString::number(icp));
        controlPointsVector.push_back(m_controlPoints[icp]->saveToScene(sceneAttributes,
                                                                        name));
    }
    SceneClassArray* classArray = new SceneClassArray("controlPointsArray",
                                                      controlPointsVector);
    sceneClass->addChild(classArray);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ControlPointFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const SceneClassArray* controlPointsArray = sceneClass->getClassArray("controlPointsArray");
    if (controlPointsArray != NULL) {
        const int32_t numElements = controlPointsArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numElements; i++) {
            const SceneClass* controlPointClass = controlPointsArray->getClassAtIndex(i);
            ControlPoint3D controlPoint(0, 0, 0, 0, 0, 0);
            controlPoint.restoreFromScene(sceneAttributes,
                                          controlPointClass);
            addControlPoint(controlPoint);
        }
    }
}

