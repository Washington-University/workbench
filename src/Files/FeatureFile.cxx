
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

#define __FEATURE_FILE_DECLARE__
#include "FeatureFile.h"
#undef __FEATURE_FILE_DECLARE__

#include <cmath>
#include <limits>

#include <zlib.h>

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QStandardItemModel>

#include "AStringNaturalComparison.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "DataCompressZLib.h"
#include "DataFileException.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "FeatureItem.h"
#include "FeatureLabel.h"
#include "FeatureLabelModel.h"
#include "FeatureItemModel.h"
#include "FeaturePropertyValue.h"
#include "NeuroglancerAnnotationFileImporter.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::FeatureFile 
 * \brief Featues.  Initially "points" but could add "linear" and other types
 * \ingroup Files
 */

/**
 * Constructor.
 */
FeatureFile::FeatureFile()
: CaretDataFile(DataFileTypeEnum::FEATURE)
{
    m_fileMetaData.reset(new GiftiMetaData());
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_featureModel.reset(new FeatureItemModel());
    
    m_volumeFileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes( { DataFileTypeEnum::VOLUME },
                                                                                                   { SubvolumeAttributes::VolumeType::ANATOMY } ));
    
    m_sceneAssistant->add("m_volumeFileSelectionModel",
                          "CaretDataFileSelectionModel",
                          m_volumeFileSelectionModel.get());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
FeatureFile::~FeatureFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return File casted to features file (avoids use of dynamic_cast that can be slow)
 * Overidden in FeatureFile
 */
FeatureFile*
FeatureFile::castToFeatureFile()
{
    return this;
}

/**
 * @return File casted to features file (avoids use of dynamic_cast that can be slow)
 * Overidden in FeatureFile
 */
const FeatureFile*
FeatureFile::castToFeatureFile() const
{
    return this;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
FeatureFile::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}
 
/**
 * @return True if this file is empty
 */
bool
FeatureFile::isEmpty() const
{
    return (m_featureModel->rowCount() == 0);
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
FeatureFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
FeatureFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* structure not supported */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
FeatureFile::getFileMetaData()
{
    return m_fileMetaData.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
FeatureFile::getFileMetaData() const
{
    return m_fileMetaData.get();
}

/**
 * @return True if file supports file metadata, else false.
 */

bool
FeatureFile::supportsFileMetaData() const
{
    return false;
}

/**
 * @return Selection model for volume that maps voxel indices to stereotaxic coordinates
 */
CaretDataFileSelectionModel*
FeatureFile::getVolumeFileSelectionModel()
{
    return m_volumeFileSelectionModel.get();
}

/**
 * @return Selection model for volume that maps voxel indices to stereotaxic coordinates (const method)
 */
const CaretDataFileSelectionModel*
FeatureFile::getVolumeFileSelectionModel() const
{
    return m_volumeFileSelectionModel.get();
}

/**
 * Add file info to data file content information
 * @param dataFileInformation
 *   The data file info structure
 */
void
FeatureFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    /*
     * Limit number of features to show.  If count is large it will be very
     * slow and use a large amount of memory.
     */
    int32_t numRows(m_featureModel->rowCount());
    AString numberOfPointsText(AString::number(numRows));
    const int maxFeaturesToShowCount(50);
    if (maxFeaturesToShowCount < numRows) {
        numRows = maxFeaturesToShowCount;
        numberOfPointsText = ("Showing "
                              + AString::number(maxFeaturesToShowCount)
                              + " of "
                              + AString::number(m_featureModel->rowCount()));
    }
    
    dataFileInformation.addNameAndValue("Number of Points",
                                        numberOfPointsText);
    const int32_t numCols(m_featureModel->columnCount());
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        for (int32_t jCol = 0; jCol < numCols; jCol++) {
            const QStandardItem* item(m_featureModel->item(iRow, jCol));
            const FeatureBase* nab(dynamic_cast<const FeatureBase*>(item));
            if (nab != NULL) {
                const AString indent((jCol == 0)
                                     ? AString::number(iRow)
                                     : "   ");
                dataFileInformation.addNameAndValue(indent,
                                                    nab->toString());
            }
        }
    }
    
    if (m_neuroglancerAnnotationFileImporter) {
        m_neuroglancerAnnotationFileImporter->addToDataFileContentInformation(dataFileInformation);
    }
}

/**
 * @return True if file supports writing, else false.
 */
bool
FeatureFile::supportsWriting() const
{
    return false;
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
FeatureFile::readFile(const AString& filename)
{
    clear();
    
    setFileName(filename);
    
    m_neuroglancerAnnotationFileImporter.reset(new NeuroglancerAnnotationFileImporter(this));
    m_neuroglancerAnnotationFileImporter->readFile(filename);
    
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
FeatureFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Features File is not supported.");
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
FeatureFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                       sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    SceneClass* annModelScene(m_featureModel->saveToScene(sceneAttributes,
                                                             "m_featureModel"));
    sceneClass->addClass(annModelScene);
    
    for (int32_t i = 0; i < getNumberOfLabelModels(); i++) {
        FeatureLabelModel* labelModel(getLabelModel(i));
        sceneClass->addClass(labelModel->saveToScene(sceneAttributes,
                                                     labelModel->getDescription()));
    }
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
FeatureFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                            sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Default all features and labels on
     */
    m_featureModel->setCheckedStatusOfAllItems(true);
    for (auto& lm : m_labelModels) {
        lm->setAllLabelsDisplayed(true);
    }
    
    const SceneClass* annModelScene(sceneClass->getClass("m_featureModel"));
    if (annModelScene != NULL) {
        m_featureModel->restoreFromScene(sceneAttributes,
                                            annModelScene);
    }
    
    for (int32_t i = 0; i < getNumberOfLabelModels(); i++) {
        FeatureLabelModel* labelModel(getLabelModel(i));
        const SceneClass* labelClass(sceneClass->getClass(labelModel->getDescription()));
        if (labelClass != NULL) {
            labelModel->restoreFromScene(sceneAttributes, labelClass);
        }
    }
}

/**
 * Add a feature to this file in the matching relationship id.  If there is
 * a feature already in the model with the matching unique ID, the
 * featureAndProperties are NOT added and are destroyed.
 *
 * @param featureAndProperties
 *    The feature is first and then the properties.  Caller MUST NOT
 *    reference featureAndProperties after calling this function as
 *    they could be destroyed immediately or at a later time.
 * @return
 *   A FunctionResult with success or failure.
 */
FunctionResult
FeatureFile::addFeature(QList<QStandardItem*>& featureAndProperties)
{
    CaretAssertVectorIndex(featureAndProperties, 0);
    const FeatureItem* featureItem(dynamic_cast<const FeatureItem*>(featureAndProperties[0]));
    CaretAssert(featureItem);
    
    if (featureItem->getGroupID() > std::numeric_limits<int32_t>::max()) {
        CaretLogWarning("Group ID is greater than 32-bit integer maximum.  OpenGL ID will fail.");
    }
    if (featureItem->getUniqueID() > std::numeric_limits<int32_t>::max()) {
        CaretLogWarning("Unique ID is greater than 32-bit integer maximum.  OpenGL ID will fail.");
    }
    
    return m_featureModel->addFeature(featureAndProperties);
}

/**
 * @return XYZ of an feature's coordinate
 * @param featureItem
 *    The feature item
 * @param coordinateIndex
 *    Index of the feature's coordinate
 */
Vector3D
FeatureFile::featureIJKtoXYZ(const FeatureItem* featureItem,
                             const int32_t coordinateIndex) const
{
    Vector3D xyz(0.0, 0.0, 0.0);
    
    CaretAssert(featureItem);
    if (featureItem != NULL) {
        Vector3D ijk(featureItem->getIJK(coordinateIndex));
        xyz = ijk;
        const CaretDataFile* cdf(m_volumeFileSelectionModel->getSelectedFile());
        if (cdf != NULL) {
            const VolumeFile* volumeFile(cdf->castToVolumeFile());
            if (volumeFile != NULL) {
                volumeFile->indexToSpace(ijk, xyz);
            }
        }
    }
    
    return xyz;
}

/**
 * @return The model containing the features
 */
FeatureItemModel*
FeatureFile::getFeatureItemModel()
{
    return m_featureModel.get();
}

/**
 * @return The model containing the features
 */
const FeatureItemModel*
FeatureFile::getFeatureItemModel() const
{
    return m_featureModel.get();
}

/**
 * @return Number of label models
 */
int32_t
FeatureFile::getNumberOfLabelModels() const
{
    return m_labelModels.size();
}

/**
 * @return Label model at the given index
 * @param index
 *   Index of the label model
 */
FeatureLabelModel*
FeatureFile::getLabelModel(const int32_t index)
{
    CaretAssertVectorIndex(m_labelModels, index);
    return m_labelModels[index].get();
}

/**
 * @return Label model at the given index (const method)
 * @param index
 *   Index of the label model
 */
const FeatureLabelModel*
FeatureFile::getLabelModel(const int32_t index) const
{
    CaretAssertVectorIndex(m_labelModels, index);
    return m_labelModels[index].get();
}

