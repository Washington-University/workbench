
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __NEUROGLANCER_ANNOTATION_FILE_DECLARE__
#include "NeuroglancerAnnotationFile.h"
#undef __NEUROGLANCER_ANNOTATION_FILE_DECLARE__

#include "CaretAssert.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "FeatureLabel.h"
#include "FeatureLabelModel.h"
#include "GiftiMetaData.h"
#include "NeuroglancerAnnotation.h"
#include "NeuroglancerAnnotationFileImporter.h"
#include "NeuroglancerPropertyInfo.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;
    
/**
 * \class caret::NeuroglancerAnnotationFile 
 * \brief Neuroglancer Annotations File
 * \ingroup Files
 */

/**
 * Constructor.
 */
NeuroglancerAnnotationFile::NeuroglancerAnnotationFile()
: CaretDataFile(DataFileTypeEnum::NEUROGLANCER_ANNOTATION)
{
    m_fileMetaData.reset(new GiftiMetaData());
    initializeIjkToXyzTransformationMatrix();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_leftNegxPosteriorPoszInferiorPosYOrientationFlag",
                          &m_leftNegxPosteriorPoszInferiorPosYOrientationFlag);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationFile::~NeuroglancerAnnotationFile()
{
}

/**
 * @return File casted to neuroglancer annotation file (avoids use of dynamic_cast that can be slow)
 * Overidden in NeuroglancerAnnotationFile
 */
NeuroglancerAnnotationFile*
NeuroglancerAnnotationFile::castToNeuroglancerAnnotationFile()
{
    return this;
}

/**
 * @return File casted to neuroglancer annotation file (avoids use of dynamic_cast that can be slow)
 * Overidden in NeuroglancerAnnotationFile
 */
const NeuroglancerAnnotationFile*
NeuroglancerAnnotationFile::castToNeuroglancerAnnotationFile() const
{
    return this;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
NeuroglancerAnnotationFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Number of Annotations", getNumberOfAnnotations());
    
    for (auto& npi : m_propertyInfos) {
        npi->addToDataFileContentInformation(dataFileInformation);
    }
}

/**
 * @return Is this foci file empty (contains zero focuss)?
 */
bool
NeuroglancerAnnotationFile::isEmpty() const
{
    return true;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
NeuroglancerAnnotationFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
NeuroglancerAnnotationFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* does nothing since focuss apply to all structures */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
NeuroglancerAnnotationFile::getFileMetaData()
{
    return m_fileMetaData.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
NeuroglancerAnnotationFile::getFileMetaData() const
{
    return m_fileMetaData.get();
}

/**
 * Clear the focus file.
 */
void
NeuroglancerAnnotationFile::clear()
{
    CaretDataFile::clear();
    m_fileMetaData->clear();
    initializeIjkToXyzTransformationMatrix();
}

/**
 * @return Is this foci file modified?
 */
bool
NeuroglancerAnnotationFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    if (m_fileMetaData->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * Clear the modification status of this foci file.
 */
void
NeuroglancerAnnotationFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_fileMetaData->clearModified();
}

/**
 * Add a property info to this file
 */
void
NeuroglancerAnnotationFile::addPropertyInfo(NeuroglancerPropertyInfo* propertyInfo)
{
    propertyInfo->setPropertyIndex(m_propertyInfos.size());
    m_propertyInfos.emplace_back(propertyInfo);
}

/**
 * Add an annotation to this file
 * @param annotation
 *    Annontation added to this file (file takes ownership of the annotation)
 */
void
NeuroglancerAnnotationFile::addAnnotation(NeuroglancerAnnotation* annotation)
{
    m_annotations.emplace_back(annotation);
}

/**
 * @return Number of annotations in the file
 */
int32_t
NeuroglancerAnnotationFile::getNumberOfAnnotations() const
{
    return m_annotations.size();
}

/**
 * @returns Annotation at the given index
 * @param index
 *    Index of the annotation
 */
const NeuroglancerAnnotation*
NeuroglancerAnnotationFile::getAnnotation(const int32_t index) const
{
    CaretAssertVectorIndex(m_annotations, index);
    return m_annotations[index].get();
}

/**
 * @returns Annotation with the given Unique ID or NULL if not found
 * @param uniqueID
 *    Unique of the annotation
 */
const NeuroglancerAnnotation*
NeuroglancerAnnotationFile::getAnnotationWithUniqueID(const uint64_t uniqueID) const
{
    for (const auto& ann : m_annotations) {
        if (ann->getUniqueID() == uniqueID) {
            return ann.get();
        }
    }
    return NULL;
}


/**
 * @return The label models
 */
std::vector<FeatureLabelModel*>
NeuroglancerAnnotationFile::getLabelModels() const
{
    std::vector<FeatureLabelModel*> labelModels;
    
    for (auto& pi : m_propertyInfos) {
        if (pi->isLabels()) {
            labelModels.push_back(pi->getLabelModel());
        }
    }
    
    return labelModels;
}

/**
 * @return Color properties for display in a combo box
 */
std::vector<ComboBoxItemInterface*>
NeuroglancerAnnotationFile::getColorPropertiesForComboBox() const
{
    std::vector<ComboBoxItemInterface*> colorProperties;
    
    for (auto& pi : m_propertyInfos) {
        if (pi->getDataType() == NeuroglancerPropertyDataTypeEnum::RGBA) {
            colorProperties.push_back(pi.get());
        }
    }

    return colorProperties;
}

/**
 * @return The property used to color the annotations, or nullptr if
 *    annotations are not colored by a property.
 */
NeuroglancerPropertyInfo*
NeuroglancerAnnotationFile::getSelectedColorProperty()
{
    validateSelectedColorProperty();
    return m_selectedColorProperty;
}

/**
 * @return The property used to color the annotations (const version), or
 *    nullptr if annotations are not colored by a property.
 */
const NeuroglancerPropertyInfo*
NeuroglancerAnnotationFile::getSelectedColorProperty() const
{
    validateSelectedColorProperty();
    return m_selectedColorProperty;
}

/**
 * Set the property used to color the annotations.
 *
 * The property is not owned by this file.  It must remain valid for as long
 * as it is the selected property.
 *
 * @param colorProperty
 *    The property for coloring, or nullptr to clear the selection.
 */
void
NeuroglancerAnnotationFile::setSelectedColorProperty(NeuroglancerPropertyInfo* colorProperty)
{
    if (m_selectedColorProperty != colorProperty) {
        m_annotationColoringValidFlag = false;
    }
    m_selectedColorProperty = colorProperty;
    validateSelectedColorProperty();
    updateColoring();
}

/**
 * Ensure the selected color property is valid
 */
void
NeuroglancerAnnotationFile::validateSelectedColorProperty() const
{
    if (m_selectedColorProperty != nullptr) {
        for (const auto& p : m_propertyInfos) {
            if ((p.get() == m_selectedColorProperty)
                && (p->getDataType() == NeuroglancerPropertyDataTypeEnum::RGBA)) {
                /*
                 * Selection is valid
                 */
                return;
            }
        }
    }
        
    /*
     * Select the first property that is RGBA
     */
    m_selectedColorProperty = NULL;
    for (const auto& p : m_propertyInfos) {
        if (p->getDataType() == NeuroglancerPropertyDataTypeEnum::RGBA) {
            m_selectedColorProperty = p.get();
            break;
        }
    }
    
    /*
     * If we got here, the color has changed
     */
    if (m_selectedColorProperty != NULL) {
        m_annotationColoringValidFlag = false;
    }
    else {
        /*
         * If no coloring, default coloring is used
         */
        m_annotationColoringValidFlag = true;
    }
}

/**
 * Set coloring for annotations
 */
void
NeuroglancerAnnotationFile::updateColoring()
{
    if (m_annotationColoringValidFlag) {
        return;
    }
 
    const NeuroglancerPropertyInfo* colorProperty(getSelectedColorProperty());
    if (colorProperty != NULL) {
        const int32_t propertyIndex(colorProperty->getPropertyIndex());
        
        CaretAssert(propertyIndex >= 0);
        for (auto& ann : m_annotations) {
            ann->setRgbaFromPropertyAtIndex(propertyIndex);
        }
        
        printDebugAnnotations();
    }
    m_annotationColoringValidFlag = true;
}

/**
 * @return All properties information
 */
const std::vector<const NeuroglancerPropertyInfo*>
NeuroglancerAnnotationFile::getAllPropertyInfos() const
{
    std::vector<const NeuroglancerPropertyInfo*> propInfo;
    
    for (const auto& p : m_propertyInfos) {
        propInfo.push_back(p.get());
    }
    
    return propInfo;
}

/**
 * @return Property with the given name (description)
 * @param name
 *    Name of property
 */
NeuroglancerPropertyInfo*
NeuroglancerAnnotationFile::getPropertyWithName(const AString& name)
{
    for (const auto& p : m_propertyInfos) {
        if (p->getDescription() == name) {
            return p.get();
        }
    }

    return NULL;
}

/**
 * @return Property info at the given index or NULL if invalid index
 * @param propertyIndex
 *    Index of the propertry
 */
const NeuroglancerPropertyInfo*
NeuroglancerAnnotationFile::getPropertyAtIndex(const int32_t propertyIndex) const
{
    NeuroglancerPropertyInfo* propInfoOut(NULL);
    const int32_t numProps(m_propertyInfos.size());
    if ((propertyIndex >= 0)
        && (propertyIndex < numProps)) {
        propInfoOut = m_propertyInfos[propertyIndex].get();
    }
    
    return propInfoOut;
}

/**
 * @return Matrix for transforming IJK to XYZ
 */
Matrix4x4
NeuroglancerAnnotationFile::getIjkToXyzTransformationMatrix()
{
    return m_ijkToXyzTransformationMatrix;
}

/**
 * @return Matrix for transforming IJK to XYZ
 */
const Matrix4x4
NeuroglancerAnnotationFile::getIjkToXyzTransformationMatrix() const
{
    return m_ijkToXyzTransformationMatrix;
}

/**
 * Set the transformation matrix that converts voxel indices (IJK) to
 * coordinates (XYZ).
 *
 * @param matrix
 *    The new transformation matrix.
 */
void
NeuroglancerAnnotationFile::setIjkToXyzTransformationMatrix(const Matrix4x4& matrix)
{
    if (matrix != m_ijkToXyzTransformationMatrix) {
        m_ijkToXyzTransformationMatrix = matrix;
        updateAnnotationsXYZ();
    }
}

/**
 * Set one component of the IJK to XYZ transformation matrix.
 *
 * @param row
 *    Row of the component, in [0, 3].
 * @param column
 *    Column of the component, in [0, 3].
 * @param value
 *    New value for the component.
 */
void
NeuroglancerAnnotationFile::setIjkToXyzTransformationMatrixComponent(const int32_t row,
                                                                     const int32_t column,
                                                                     const float value)
{
    CaretAssertMessage((row >= 0) && (row < 4), "Row must be in [0, 3]");
    CaretAssertMessage((column >= 0) && (column < 4), "Column must be in [0, 3]");
    if ((row < 0) || (row > 3)
        || (column < 0) || (column > 3)) {
        return;
    }
    
    if (m_ijkToXyzTransformationMatrix.getMatrixElement(row, column) != value) {
        m_ijkToXyzTransformationMatrix.setMatrixElement(row,
                                                        column,
                                                        static_cast<double>(value));
        updateAnnotationsXYZ();
    }
}

/**
 * @return Flag indicating orientation is: (Left, Negative X), (Posterior, Positive Z), (Inferior, PosY)
 * when drawing annotations.
 */
bool
NeuroglancerAnnotationFile::isLeftNegxPosteriorPoszInferiorPosYOrientation() const
{
    return m_leftNegxPosteriorPoszInferiorPosYOrientationFlag;
}

/**
 * Set flag indicating orientation is: (Left, Negative X), (Posterior, Positive Z), (Inferior, PosY)
 * when drawing annotations.
 */
void
NeuroglancerAnnotationFile::setLeftNegxPosteriorPoszInferiorPosYOrientation(const bool status)
{
    m_leftNegxPosteriorPoszInferiorPosYOrientationFlag = status;
}

/**
 * Initialize the IJK to XYZ transformation matrix.
 */
void
NeuroglancerAnnotationFile::initializeIjkToXyzTransformationMatrix()
{
    /*
     Matrix is contained in the URL for the neuroglancer web page.
     Look for the "matrix as shown below.
     
     {
     "type": "annotation",
     "name": "cells",
     "source": {
     "url": "precomputed://https://allen-hmba-releases.s3.us-west-2.amazonaws.com/neuroglancer/HMBA-MERSCOPE-QM23.50.001-BG/20250630/donor/cells",
     "transform": {
     "matrix": [
     [1, 0, 0, 0],
     [0, 0, -1, 0],
     [0, -1, 0, 0]
     ],
     "outputDimensions": {
     "x": [0.001, "m"],
     "y": [0.001, "m"],
     "z": [0.001, "m"]
     }
     }
     },
     */
    
    m_ijkToXyzTransformationMatrix.setRow(0,  1.0,  0.0,  0.0,   0.0);
    m_ijkToXyzTransformationMatrix.setRow(1,  0.0,  0.0, -1.0,   0.0);
    m_ijkToXyzTransformationMatrix.setRow(2,  0.0, -1.0,  0.0,   0.0);
    m_ijkToXyzTransformationMatrix.setRow(3,  0.0,  0.0,  0.0,   1.0);
    
    m_leftNegxPosteriorPoszInferiorPosYOrientationFlag = true;
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
NeuroglancerAnnotationFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    NeuroglancerAnnotationFileImporter importer(this);
    importer.readFile(filename);
    
    setFileName(filename);
    
    updateDisplayedAnnotations();
    
    updateColoring();
    
    updateAnnotationsXYZ();

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
NeuroglancerAnnotationFile::writeFile(const AString& filename)
{
    throw DataFileException("Writing of NeuroglancerAnnotationFile is not supported for "
                            + filename);
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
NeuroglancerAnnotationFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                       sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    const NeuroglancerPropertyInfo* colorProperty(getSelectedColorProperty());
    if (colorProperty != NULL) {
        sceneClass->addString("selectedColorProperty",
                              colorProperty->getDescription());
    }
    
    for (const auto& p : m_propertyInfos) {
        SceneClass* propInfoSceneClass(p->saveToScene(sceneAttributes,
                                                      p->getDescription()));
        if (propInfoSceneClass != NULL) {
            sceneClass->addClass(propInfoSceneClass);
        }
    }
    
    float m16[16];
    m_ijkToXyzTransformationMatrix.getMatrixForOpenGL(m16);
    sceneClass->addFloatArray("m_ijkToXyzTransformationMatrix", m16, 16);
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
NeuroglancerAnnotationFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    m_annotationColoringValidFlag = false;
    
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                            sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const AString colorPropertyName(sceneClass->getStringValue("selectedColorProperty"));
    if ( ! colorPropertyName.isEmpty()) {
        NeuroglancerPropertyInfo* propertyInfo(getPropertyWithName(colorPropertyName));
        if (propertyInfo != NULL) {
            if (propertyInfo->getDataType() == NeuroglancerPropertyDataTypeEnum::RGBA) {
                setSelectedColorProperty(propertyInfo);
            }
        }
    }
    
    for (const auto& p : m_propertyInfos) {
        const SceneClass* propInfoSceneClass(sceneClass->getClass(p->getDescription()));
        if (propInfoSceneClass != NULL) {
            p->restoreFromScene(sceneAttributes,
                                propInfoSceneClass);
        }
    }
    
    float m16[16];
    if (sceneClass->getFloatArrayValue("m_ijkToXyzTransformationMatrix", m16, 16) == 16) {
        m_ijkToXyzTransformationMatrix.setMatrixFromOpenGL(m16);
    }
    else {
        initializeIjkToXyzTransformationMatrix();
    }
    
    updateDisplayedAnnotations();
    
    updateAnnotationsXYZ();
}

/**
 * Update the displayed annotations
 */
void
NeuroglancerAnnotationFile::updateDisplayedAnnotations()
{
    const std::vector<const NeuroglancerPropertyInfo*> allPropInfo(getAllPropertyInfos());
    for (auto& ann : m_annotations) {
        ann->updateDisplayedStatus(allPropInfo);
    }
}

void
/**
 * Update the XYZ for all annotations
 */
NeuroglancerAnnotationFile::updateAnnotationsXYZ()
{
    std::set<uint64_t> debugIDs(NeuroglancerAnnotationFileImporter::getAnnotionsUniqueIDsForDebugging());
    
    for (auto& ann : m_annotations) {
        ann->updateXYZ(m_ijkToXyzTransformationMatrix);
    }
    
    printDebugAnnotations();
}

/**
 * Print info about some annotations for debugging
 */
void
NeuroglancerAnnotationFile::printDebugAnnotations()
{
    const bool printFlag(false);
    if ( ! printFlag) {
        return;
    }
    
    std::set<uint64_t> debugIDs(NeuroglancerAnnotationFileImporter::getAnnotionsUniqueIDsForDebugging());
    if ( ! debugIDs.empty()) {
        std::cout << "IJK to XYZ Matrix" << std::endl;
        std::cout << m_ijkToXyzTransformationMatrix.toFormattedString("   ") << std::endl;
    }
    for (uint64_t id : debugIDs) {
        const NeuroglancerAnnotation* ann(getAnnotationWithUniqueID(id));
        if (ann != NULL) {
            const uint8_t* rgba(ann->getRgba());
            AString rHex(AString::number(rgba[0], 16));
            AString gHex(AString::number(rgba[1], 16));
            AString bHex(AString::number(rgba[2], 16));
            AString aHex(AString::number(rgba[3], 16));
            std::cout << "Annotation UniqueID=" << ann->getUniqueID() << std::endl;
            std::cout << "   IJK=" << ann->getIJK().toString() << std::endl;
            std::cout << "   XYZ=" << ann->getXYZ().toString() << std::endl;
            std::cout << "   RGBA=" << AString::fromNumbers(rgba, 4) << std::endl;
            std::cout << "   RGBA Hex=" << rHex << ", " << gHex << ", " << bHex << ", " << aHex << std::endl;
        }
    }

}
