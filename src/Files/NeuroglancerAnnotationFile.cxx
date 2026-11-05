
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
        std::cout << "Coloring with property index=" << propertyIndex
        << ", name: " << colorProperty->getDescription() << std::endl;
        
        CaretAssert(propertyIndex >= 0);
        for (auto& ann : m_annotations) {
            ann->setRgbaFromPropertyAtIndex(propertyIndex);
        }
        
        std::set<uint64_t> debugIDs(NeuroglancerAnnotationFileImporter::getAnnotionsUniqueIDsForDebugging());
        for (uint64_t id : debugIDs) {
            const NeuroglancerAnnotation* ann(getAnnotationWithUniqueID(id));
            if (ann != NULL) {
                const uint8_t* rgba(ann->getRgba());
                AString rHex(AString::number(rgba[0], 16));
                AString gHex(AString::number(rgba[1], 16));
                AString bHex(AString::number(rgba[2], 16));
                AString aHex(AString::number(rgba[3], 16));
                std::cout << "Annotation UniqueID=" << ann->getUniqueID()
                << ", XYZ=" << ann->getXYZ().toString()
                << ", RGBA=" << AString::fromNumbers(rgba, 4)
                << ", RGBA Hex=" << rHex << ", " << gHex << ", " << bHex << ", " << aHex << std::endl;
            }
        }
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
    
    std::cout << "After reading " << filename << std::endl;
    std::cout << "   Property Count: " << m_propertyInfos.size() << std::endl;
    std::cout << "   Annotations Count: " << m_annotations.size() << std::endl;
    
    setFileName(filename);
    
    updateColoring();
    
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
