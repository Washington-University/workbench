
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

#define __CARET_MAPPABLE_DATA_FILE_DECLARE__
#include "CaretMappableDataFile.h"
#undef __CARET_MAPPABLE_DATA_FILE_DECLARE__

#include <limits>

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiXML.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "FastStatistics.h"
#include "FileIdentificationAttributes.h"
#include "FileInformation.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaDataXmlElements.h"
#include "Histogram.h"
#include "LabelDrawingProperties.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneAttributes.h"
#include "ScenePrimitive.h"
#include "StringTableModel.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * Constructor.
 */
CaretMappableDataFile::CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    initializeCaretMappableDataFileInstance(dataFileType);
}

/**
 * Destructor.
 */
CaretMappableDataFile::~CaretMappableDataFile()
{
    
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is copied.
 */
CaretMappableDataFile::CaretMappableDataFile(const CaretMappableDataFile& cmdf)
: CaretDataFile(cmdf)
{
    initializeCaretMappableDataFileInstance(cmdf.getDataFileType());
    
    this->copyCaretMappableDataFile(cmdf);
}

/**
 * Assignment operator.
 * @param cmdf
 *   Instance that is assigned to this.
 * @return
 *   Reference to this instance.
 */
CaretMappableDataFile& 
CaretMappableDataFile::operator=(const CaretMappableDataFile& cmdf)
{
    if (this != &cmdf) {
        CaretDataFile::operator=(cmdf);
        this->copyCaretMappableDataFile(cmdf);
    }
    return *this;
}

/**
 * Initialize a new instance.
 *
 * @param dataFileType
 *     Type of data file.
 */
void
CaretMappableDataFile::initializeCaretMappableDataFileInstance(const DataFileTypeEnum::Enum /*dataFileType*/)
{
    m_labelDrawingProperties = std::unique_ptr<LabelDrawingProperties>(new LabelDrawingProperties());
    m_fileIdentificationAttributes.reset(new FileIdentificationAttributes());
    m_applyToAllMapsSelected = false;
}


/**
 * Assists with copying instances of this class.
 */
void 
CaretMappableDataFile::copyCaretMappableDataFile(const CaretMappableDataFile& cmdf)
{
    *m_labelDrawingProperties = *cmdf.m_labelDrawingProperties;
    m_mapThresholdFileSelectionModels.clear();
}

// note: method is documented in header file
bool
CaretMappableDataFile::hasMapAttributes() const
{
    return true;
}

/**
 * Is this file able to map to the given structure?  Some data files, such
 * as CIFTI files, are able to map to multiple surface structure.  The default
 * implementation of this method simply compares the given structure to
 * getStructure() and returns true if they are the same value, else false.
 *
 * @param structure
 *   Structure for testing mappability status.
 * @return True if this file is able to map to the given structure, else false.
 */
bool
CaretMappableDataFile::isMappableToSurfaceStructure(const StructureEnum::Enum structure) const
{
    if (getStructure() == StructureEnum::ALL) {
        return true;
    }
    
    if (structure == getStructure()) {
        return true;
    }
    
    return false;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromNameOrNumber(const AString& mapName) const
{
    bool ok = false;
    int32_t ret = mapName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)
    {
        if (ret < 0 || ret >= getNumberOfMaps())
        {
            ret = -1;
        }
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        ret = getMapIndexFromName(mapName);
    }
    return ret;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromName(const AString& mapName) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (mapName == getMapName(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (uniqueID == getMapUniqueID(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
void
CaretMappableDataFile::updateScalarColoringForAllMaps()
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        updateScalarColoringForMap(iMap);
    }
    
    invalidateHistogramChartColoring();
}

/**
 * @return True if this file is mapped using a palette and all palettes
 * are equal, otherwise false.
 */
bool
CaretMappableDataFile::isPaletteColorMappingEqualForAllMaps() const
{
    if ( ! isMappedWithPalette()) {
        return false;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    if (numMaps <= 0) {
        return false;
    }
    
    /*
     * Some files use one palette color mapping for all maps
     * and this can be detected if the pointer to palette
     * color mapping is the same for all maps.
     */
    bool pointerTheSameFlag = true;
    const PaletteColorMapping* firstPCM = getMapPaletteColorMapping(0);
    for (int32_t iMap = 1; iMap < numMaps; iMap++) {
        if (firstPCM != getMapPaletteColorMapping(iMap)) {
            pointerTheSameFlag = false;
            break;
        }
    }
    if (pointerTheSameFlag) {
        return true;
    }
    
    /*
     * Compare each palette color mapping to the first palette color mapping
     */
    for (int32_t iMap = 1; iMap < numMaps; iMap++) {
        if (*firstPCM != *getMapPaletteColorMapping(iMap)) {
            return false;
        }
    }

    return true;
}

/**
 * Apply palette coloring from the given map to all other maps in the file.
 *
 * @param mapIndex
 *     Index of the map.
 */
void
CaretMappableDataFile::applyPaletteColorMappingToAllMaps(const int32_t mapIndex)
{
    if ( ! isMappedWithPalette()) {
        return;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    if (numMaps <= 1) {
        return;
    }
    
    const PaletteColorMapping* mapColoring = getMapPaletteColorMapping(mapIndex);
    for (int32_t i = 0; i < numMaps; i++) {
        if (i != mapIndex) {
            PaletteColorMapping* pcm = getMapPaletteColorMapping(i);
                pcm->copy(*mapColoring,
                          false);
        }
        
        /*
         * Want all palette color mappings modified.
         * Otherwise, this index might remain unmodified and the
         * auto palette fixing when scenes are loaded may
         */
        getMapPaletteColorMapping(mapIndex)->setModified();
    }
}

/**
 * Invalidate all histogram coloring for this file.
 */
void
CaretMappableDataFile::invalidateHistogramChartColoring()
{
    getChartingDelegate()->getHistogramCharting()->invalidateAllColoring();
}

// note: method is documented in header file
NiftiTimeUnitsEnum::Enum
CaretMappableDataFile::getMapIntervalUnits() const
{
    return NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
}

// note: method is documented in header file
void
CaretMappableDataFile::getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                        float& mapIntervalStepValueOut) const
{
    firstMapUnitsValueOut   = 1.0;
    mapIntervalStepValueOut = 1.0;
}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
CaretMappableDataFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    dataRangeMaximumOut = std::numeric_limits<float>::max();
    dataRangeMinimumOut = -dataRangeMaximumOut;
    
    return false;
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CaretMappableDataFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                           SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    

    sceneClass->addClass(m_labelDrawingProperties->saveToScene(sceneAttributes,
                                                               "m_labelDrawingProperties"));
    
    sceneClass->addClass(m_fileIdentificationAttributes->saveToScene(sceneAttributes,
                                                                     "m_fileIdentificationAttributes"));

    if (m_chartingDelegate != NULL) {
        SceneClass* chartDelegateScene = m_chartingDelegate->saveToScene(sceneAttributes,
                                                                         "m_chartingDelegate");
        if (chartDelegateScene != NULL) {
            sceneClass->addClass(chartDelegateScene);
        }
    }
    
    if (isMappedWithPalette()) {
        /*
         * 03 March 2017
         * Note: Palette (m_paletteNormalizationMode) normalization is no 
         * longer added to scenes since the palette normalization is a file 
         * property stored in the file's metadata.
         */
        /*
         * WB-690 request normalization mode be saved to scenes
         * so adding it back in.
         */
        sceneClass->addEnumeratedType<PaletteNormalizationModeEnum,
            PaletteNormalizationModeEnum::Enum>("m_paletteNormalizationMode",
                                                getPaletteNormalizationMode());
        
        sceneClass->addBoolean("m_applyToAllMapsSelected",
                               m_applyToAllMapsSelected);
        
        if (sceneAttributes->isModifiedPaletteSettingsSavedToScene()) {
            /*
             * WB-916 Scene files very large for series type files with
             * modified palette color mapping
             *
             * When one palette coloring mapping is used for a file,
             * we only need to save the palette color mapping for the
             * first map.  Only some CIFTI files, particularly
             * series type files, use one palette color mapping for all
             * maps.   Until 12 Oct 2020, palette color mapping was
             * always written for all maps which greatly bloated the
             * scene file for scalar data-series files with many rows.
             */
            int32_t numMaps = getNumberOfMaps();
            if (numMaps > 0) {
                if (isOnePaletteUsedForAllMaps()) {
                    numMaps = 1;
                }
            }

            std::vector<SceneClass*> pcmClassVector;
            for (int32_t i = 0; i < numMaps; i++) {
                const PaletteColorMapping* pcmConst = getMapPaletteColorMapping(i);
                bool savePaletteFlag = false;
                switch (pcmConst->getModifiedStatus()) {
                    case PaletteModifiedStatusEnum::MODIFIED:
                        savePaletteFlag = true;
                        break;
                    case PaletteModifiedStatusEnum::MODIFIED_BY_SHOW_SCENE:
                        savePaletteFlag = true;
                        break;
                    case PaletteModifiedStatusEnum::UNMODIFIED:
                        break;
                }
                if (savePaletteFlag) {
                    PaletteColorMapping* pcm = const_cast<PaletteColorMapping*>(pcmConst);
                    
                    try {
                        const AString xml = pcm->encodeInXML();

                        
                        SceneClass* pcmClass = new SceneClass("savedPaletteColorMapping",
                                                              "SavedPaletteColorMapping",
                                                              1);
                        pcmClass->addString("mapName",
                                            getMapName(i));
                        pcmClass->addInteger("mapIndex", i);
                        pcmClass->addInteger("mapCount", numMaps);
                        pcmClass->addString("mapColorMapping", xml);
                        
                        pcmClassVector.push_back(pcmClass);
                    }
                    catch (const XmlException& e) {
                        sceneAttributes->addToErrorMessage("Failed to encode palette color mapping for file: "
                                                           + getFileNameNoPath()
                                                           + "  Map Name: "
                                                           + getMapName(i)
                                                           + "  Map Index: "
                                                           + AString::number(i)
                                                           + ".  "
                                                           + e.whatString());
                    }
                }
            }
            
            if ( ! pcmClassVector.empty()) {
                SceneClassArray* pcmArray = new SceneClassArray("savedPaletteColorMappingArray",
                                                                pcmClassVector);
                sceneClass->addChild(pcmArray);
            }
        }
        
        {
            /*
             * Save thresholds for each map
             */
            SceneObjectMapIntegerKey* sceneThreshMap = new SceneObjectMapIntegerKey("m_mapThresholdFileSelectionModels",
                                                                                    SceneObjectDataTypeEnum::SCENE_CLASS);
            const int32_t numMaps = getNumberOfMaps();
            for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                CaretMappableDataFileAndMapSelectionModel* threshSel = getMapThresholdFileSelectionModel(iMap);
                if ((threshSel->getSelectedFile() != this)
                    || (threshSel->getSelectedMapIndex() != iMap)) {
                    sceneThreshMap->addClass(iMap, threshSel->saveToScene(sceneAttributes, "threshSelElement"));
                }
            }
            
            if (sceneThreshMap->isEmpty()) {
                delete sceneThreshMap;
            }
            else {
                sceneClass->addChild(sceneThreshMap);
            }
        }
    }
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
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
CaretMappableDataFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    m_labelDrawingProperties->restoreFromScene(sceneAttributes,
                                               sceneClass->getClass("m_labelDrawingProperties"));
    m_fileIdentificationAttributes->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_fileIdentificationAttributes"));
    
    const SceneClass* chartingDelegateClass = sceneClass->getClass("m_chartingDelegate");
    ChartableTwoFileDelegate* chartDelegate = getChartingDelegate();
    chartDelegate->updateAfterFileChanged();
    if (chartingDelegateClass != NULL) {
        CaretAssert(chartDelegate);
        chartDelegate->restoreFromScene(sceneAttributes,
                                             chartingDelegateClass);
    }
    
    if (isMappedWithPalette()) {
        /*
         * Palette normalization was no longer saved to scenes after 
         * palette normalization was saved in mappable files's metadata.
         */
        std::vector<PaletteNormalizationModeEnum::Enum> paletteNormalizationModes;
        getPaletteNormalizationModesSupported(paletteNormalizationModes);
        if ( ! paletteNormalizationModes.empty()) {
            const AString paletteNormStringValue = sceneClass->getEnumeratedTypeValueAsString("m_paletteNormalizationMode");
            if ( ! paletteNormStringValue.isEmpty()) {
                bool validFlag = false;
                const PaletteNormalizationModeEnum::Enum palNormValue = PaletteNormalizationModeEnum::fromName(paletteNormStringValue,
                                                                                                           &validFlag);
                if (validFlag) {
                    if (std::find(paletteNormalizationModes.begin(),
                                  paletteNormalizationModes.end(),
                                  palNormValue) != paletteNormalizationModes.end()) {
                        /*
                         * Do not allow the metadata's modification status
                         * to change or else many files will have a modified
                         * status after restoration of the scene.
                         */
                        const bool metadataModFlag = getFileMetaData()->isModified();
                        setPaletteNormalizationMode(palNormValue);
                        if ( ! metadataModFlag) {
                            getFileMetaData()->clearModified();
                        }
                    }
                }
            }
        }
        
        /*
         * This enables fixing of palettes for files that have palette settings
         * for each map; apply to all maps is enabled; there is at least one
         * palettes settings in the scene for the file; and the number of maps
         * in the file does not match the number of palette settings in the scene
         * for the file
         */
        const bool enablePaletteFixingFlag(true);
        
        /*
         * Will be set to true if we find that the file's palette
         * settings need to be updated.
         */
        bool fixFilesPalettesFlag(false);

        std::vector<AString> paletteErrorMessages;
        int32_t numberOfMapPaletteSettings(-1);
        
        const int32_t numMaps = getNumberOfMaps();
        const SceneClassArray* pcmArray = sceneClass->getClassArray("savedPaletteColorMappingArray");
        if (pcmArray != NULL) {
            const int32_t numElements = pcmArray->getNumberOfArrayElements();
            if (enablePaletteFixingFlag) {
                if ( ! isOnePaletteUsedForAllMaps()) {
                    if ((numElements > 0)
                        && (numMaps > 0)
                        && (numMaps != numElements)) {
                        
                        //std::cout << "*** Maps=" << numMaps << ", palettes=" << numElements << " file: " << getFileNameNoPath() << std::endl;
                        
                        numberOfMapPaletteSettings = numElements;
                        
                        if (sceneAttributes->isLogFilesWithPaletteSettingsErrors()) {
                            
                            
                            /*
                             * Number of maps in file is different than number of palette settings
                             * in the scene for the file.  This is used by the
                             * scene file update command that will fix the palette errors itself.
                             */
                            sceneAttributes->addToMapFilesWithPaletteSettingsErrors(this,
                                                                                    getFileName());
                        }
                        else {
                            fixFilesPalettesFlag = true;
                        }
                    }
                }
            }

            for (int32_t i = 0; i < numElements; i++) {
                const SceneClass* pcmClass = pcmArray->getClassAtIndex(i);
                
                const AString mapName = pcmClass->getStringValue("mapName");
                const int32_t mapIndex = pcmClass->getIntegerValue("mapIndex", -1);
                const int32_t mapCount = pcmClass->getIntegerValue("mapCount", -1);
                const AString pcmString = pcmClass->getStringValue("mapColorMapping");
                
                int32_t restoreMapIndex = -1;
                
                /*
                 * Try to find map that has the saved name AND index.
                 */
                if (restoreMapIndex < 0) {
                    if ((mapIndex >= 0)
                        && (mapIndex < numMaps)) {
                        if (getMapName(mapIndex) == mapName) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                }
                
                /*
                 * If map count has not changed, give preference to
                 * map index over map name
                 */
                if (mapCount == numMaps) {
                    /*
                     * Try to find map that has the saved map index.
                     */
                    if (restoreMapIndex < 0) {
                        if ((mapIndex >= 0)
                            && (mapIndex < numMaps)) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                    
                    /*
                     * Try to find map that has the saved map name.
                     */
                    if (restoreMapIndex < 0) {
                        if ( ! mapName.isEmpty()) {
                            restoreMapIndex = getMapIndexFromName(mapName);
                        }
                    }
                    
                }
                else {
                    /*
                     * Try to find map that has the saved map name.
                     */
                    if (restoreMapIndex < 0) {
                        if ( ! mapName.isEmpty()) {
                            restoreMapIndex = getMapIndexFromName(mapName);
                        }
                    }
                    
                    /*
                     * Try to find map that has the saved map index.
                     */
                    if (restoreMapIndex < 0) {
                        if ((mapIndex >= 0)
                            && (mapIndex < numMaps)) {
                            restoreMapIndex = mapIndex;
                        }
                    }
                }
                
                
                if (restoreMapIndex >= 0) {
                    try {
                        PaletteColorMapping pcm;
                        pcm.decodeFromStringXML(pcmString);
                        
                        PaletteColorMapping* pcmMap = getMapPaletteColorMapping(restoreMapIndex);
                        pcmMap->copy(pcm,
                                     true);
                        pcmMap->clearModified();
                        
                        /*
                         * WB-522 When palette loaded from scene,
                         * mark it as modified.
                         */
                        pcmMap->setSceneModified();
                        
                        /*
                         * Volume file needs it's map coloring updated since
                         * palette has changed.
                         */
                        VolumeFile* volumeFile = dynamic_cast<VolumeFile*>(this);
                        if (volumeFile != NULL) {
                            volumeFile->updateScalarColoringForMap(restoreMapIndex);
                        }
                    }
                    catch (const XmlException& e) {
                        sceneAttributes->addToErrorMessage("Failed to decode palette color mapping for file: "
                                                           + getFileNameNoPath()
                                                           + "  Map Name: "
                                                           + getMapName(i)
                                                           + "  Map Index: "
                                                           + AString::number(i)
                                                           + ".  "
                                                           + e.whatString());
                    }
                }
                else {
                    if (sceneAttributes->isLogFilesWithPaletteSettingsErrors()) {
                        /* Prevent logging of error message */
                    }
                    else {
                        const AString msg = ("Unable to find map for restoring palette settings for file: "
                                             + getFileNameNoPath()
                                             + "  Map Name: "
                                             + mapName
                                             + "  Map Index: "
                                             + AString::number(mapIndex));
                        if (fixFilesPalettesFlag) {
                            /*
                             * Message may be displayed later
                             */
                            paletteErrorMessages.push_back(msg);
                        }
                        else {
                            sceneAttributes->addToErrorMessage(msg);
                        }
                    }
                }
            }
        }
        
        {
            m_mapThresholdFileSelectionModels.clear();
            
            const SceneObjectMapIntegerKey* sceneThreshMap = sceneClass->getMapIntegerKey("m_mapThresholdFileSelectionModels");
            if (sceneThreshMap != NULL) {
                const std::vector<int32_t> keys = sceneThreshMap->getKeys();
                for (auto mapIndex : keys) {
                    CaretAssert(mapIndex < getNumberOfMaps());
                    const SceneClass* threshSel = dynamic_cast<const SceneClass*>(sceneThreshMap->getObject(mapIndex));
                    CaretAssert(threshSel);
                    getMapThresholdFileSelectionModel(mapIndex)->restoreFromScene(sceneAttributes,
                                                                                  threshSel);
                }
            }
        }
        
        updateAfterFileDataChanges();
        
        /*
         * Must restore after call to updateAfterFileDataChanges() to since
         * that method initializes 'm_applyToAllMapsSelected'.
         *
         * This was added by WB-781 Apply to All Maps for ColorBar so that 
         * the 'apply to all maps' status is saved to and restored from scenes.
         */
        const ScenePrimitive* applyToAllMapsPrimitive = sceneClass->getPrimitive("m_applyToAllMapsSelected");
        if (applyToAllMapsPrimitive != NULL) {
            m_applyToAllMapsSelected = applyToAllMapsPrimitive->booleanValue();
        }
        
        /*
         * Need to fix here since we need to know if Apply to All Maps is enabled
         */
        if (fixFilesPalettesFlag
            && m_applyToAllMapsSelected) {
            /*
             * Only need to set palettes if the number of maps has increased
             */
            if (numMaps != numberOfMapPaletteSettings) {
                if (numMaps > numberOfMapPaletteSettings) {
                    /*
                     * Apply the first map to all maps
                     */
                    const int32_t mapIndex(0);
                    applyPaletteColorMappingToAllMaps(mapIndex);
                }
                CaretLogInfo("Fixed incorrect palette settings count (maps="
                             + AString::number(numMaps)
                             + ", paletteSettings="
                             + AString::number(numberOfMapPaletteSettings)
                             + ") for "
                             + getFileNameNoPath()
                             + ".  Replacing the scene will eliminate this message.");
            }
        }
        else {
            /*
             * Cannot fix palette errors so keep any error messages
             */
            for (const auto& msg : paletteErrorMessages) {
                sceneAttributes->addToErrorMessage(msg);
            }
        }
        /*
         * README ABOUT IMPORTANCE OF MODIFIED COLOR PALLETTE MAPPING STATUS MUST REMAIN ON
         *
         * (1) The user may modify the palette color mapping for a map/file.  This modified palette
         * color mapping is saved to the scene so that the user does not need to save the actual
         * data file.  When the scene is restored, the palette color mapping is restored from the
         * scene and applied to the file.  As a result, the file will contain a 'modified palette
         * color mapping status'.
         *
         * (2) This 'modified palette color mapping status' must remain ON so that if the user
         * saves a scene, the modified status is added to the scene and will be restored from
         * the scene at a later time.  As a result, the scene will display correctly.
         *
         * (3) If the 'modified palette color mapping status' was NOT left on and the user 
         * saved a scene, the scene would not display correctly due to the palette color mapping
         * no longer being added to the scene.
         */
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CaretMappableDataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    const int64_t dataSizeInBytes = getDataSizeUncompressedInBytes();
    if (dataSizeInBytes >= 0) {
        dataFileInformation.addNameAndValue("Data Size",
                                            FileInformation::fileSizeToStandardUnits(dataSizeInBytes));
    }
    
    dataFileInformation.addNameAndValue("Maps to Surface",
                                        isSurfaceMappable());
    dataFileInformation.addNameAndValue("Maps to Volume",
                                        isVolumeMappable());
    dataFileInformation.addNameAndValue("Maps with LabelTable",
                                        isMappedWithLabelTable());
    dataFileInformation.addNameAndValue("Maps with Palette",
                                        isMappedWithPalette());
    
    if (isMappedWithPalette()) {
        dataFileInformation.addNameAndValue("All Map Palettes Equal",
                                            isPaletteColorMappingEqualForAllMaps());
        
        NiftiTimeUnitsEnum::Enum timeUnits = getMapIntervalUnits();
        switch (timeUnits) {
            case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                break;
            case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                break;
        }
        dataFileInformation.addNameAndValue("Map Interval Units", NiftiTimeUnitsEnum::toName(timeUnits));
        if (timeUnits != NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN) {
            float mapIntervalStart, mapIntervalStep;
            getMapIntervalStartAndStep(mapIntervalStart, mapIntervalStep);
            dataFileInformation.addNameAndValue("Map Interval Start", mapIntervalStart);
            dataFileInformation.addNameAndValue("Map Interval Step", mapIntervalStep);
        }
    }
    
    bool showMapFlag = (isMappedWithLabelTable() || isMappedWithPalette());
   
    /*
     * Do not show maps on CIFTI connectivity matrix data because
     * they do not have maps, they have a matrix.
     */
    const bool ciftiMatrixFlag = (dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(this)
                                  != NULL);
    if (ciftiMatrixFlag) {
        showMapFlag = false;
    }
    
    /*
     * Did user override display of map information?
     */
    if ( ! dataFileInformation.isOptionFlag(DataFileContentInformation::OPTION_SHOW_MAP_INFORMATION)) {
        showMapFlag = false;
    }
    
    if (showMapFlag) {
        const int32_t numMaps = getNumberOfMaps();
        dataFileInformation.addNameAndValue("Number of Maps",
                                                numMaps);
        
        if (numMaps > 0) {
            int columnCount = 0;
            const int COL_INDEX   = columnCount++;
            
            int32_t COL_MIN     = -1;
            int32_t COL_MAX     = -1;
            int32_t COL_MEAN    = -1;
            int32_t COL_DEV     = -1;
            int32_t COL_PCT_POS = -1;
            int32_t COL_PCT_NEG = -1;
            int32_t COL_INF_NAN = -1;
            
            if (isMappedWithPalette()) {
                COL_MIN = columnCount++;
                COL_MAX = columnCount++;
                COL_MEAN = columnCount++;
                COL_DEV = columnCount++;
                COL_PCT_POS = columnCount++;
                COL_PCT_NEG = columnCount++;
                COL_INF_NAN = columnCount++;
            }
            const int COL_NAME = columnCount++;
            
            /*
             * Include a row for the column titles
             */
            const int32_t tableRowCount = numMaps + 1;
            StringTableModel stringTable(tableRowCount,
                                         columnCount);
            
            stringTable.setElement(0, COL_INDEX, "Map");
            if (COL_MIN >= 0) {
                stringTable.setElement(0, COL_MIN, "Minimum");
            }
            if (COL_MAX >= 0) {
                stringTable.setElement(0, COL_MAX, "Maximum");
            }
            if (COL_MEAN >= 0) {
                stringTable.setElement(0, COL_MEAN, "Mean");
            }
            if (COL_DEV >= 0) {
                stringTable.setElement(0, COL_DEV, "Sample Dev");
            }
            if (COL_PCT_POS >= 0) {
                stringTable.setElement(0, COL_PCT_POS, "% Positive");
            }
            if (COL_PCT_NEG >= 0) {
                stringTable.setElement(0, COL_PCT_NEG, "% Negative");
            }
            if (COL_INF_NAN >= 0) {
                stringTable.setElement(0, COL_INF_NAN, "Inf/NaN");
            }
            
            stringTable.setElement(0, COL_NAME, "Map Name");
            stringTable.setColumnAlignment(COL_NAME, StringTableModel::ALIGN_LEFT);
            
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                
                const int32_t tableRow = mapIndex + 1;
                
                CaretAssert(COL_INDEX >= 0);
                CaretAssert(COL_NAME >= 0);
                stringTable.setElement(tableRow, COL_INDEX, (mapIndex + 1));
                stringTable.setElement(tableRow, COL_NAME, getMapName(mapIndex));
                
                const FastStatistics* stats = const_cast<CaretMappableDataFile*>(this)->getMapFastStatistics(mapIndex);
                if (isMappedWithPalette()
                    && (stats != NULL)) {
                    
                    const Histogram* histogram = getMapHistogram(mapIndex);
                    int64_t posCount = 0;
                    int64_t zeroCount = 0;
                    int64_t negCount = 0;
                    int64_t infCount = 0;
                    int64_t negInfCount = 0;
                    int64_t nanCount = 0;
                    histogram->getCounts(posCount,
                                         zeroCount,
                                         negCount,
                                         infCount,
                                         negInfCount,
                                         nanCount);
                    const int64_t numInfinityAndNotANumber = (infCount
                                                + negInfCount
                                                + nanCount);
                    const double totalCount = (posCount
                                                + zeroCount
                                                + negCount
                                                + numInfinityAndNotANumber);
                    const double pctPositive = (posCount / totalCount) * 100.0;
                    const double pctNegative = (negCount / totalCount) * 100.0;
                    
                    if (COL_MIN >= 0) {
                        stringTable.setElement(tableRow, COL_MIN, stats->getMin());
                    }
                    
                    if (COL_MAX >= 0) {
                        stringTable.setElement(tableRow, COL_MAX, stats->getMax());
                    }
                    
                    if (COL_MEAN >= 0) {
                        stringTable.setElement(tableRow, COL_MEAN, stats->getMean());
                    }
                    
                    if (COL_DEV >= 0) {
                        stringTable.setElement(tableRow, COL_DEV, stats->getSampleStdDev());
                    }
                    if (COL_PCT_POS >= 0) {
                        stringTable.setElement(tableRow, COL_PCT_POS, pctPositive);
                    }
                    
                    if (COL_PCT_NEG >= 0) {
                        stringTable.setElement(tableRow, COL_PCT_NEG, pctNegative);
                    }
                    
                    if (COL_INF_NAN >= 0) {
                        stringTable.setElement(tableRow, COL_INF_NAN, numInfinityAndNotANumber);
                    }
                }
            }
            
            dataFileInformation.addText("\n"
                                        + stringTable.getInString()
                                        + "\n");
            
        }
    }
    
    if (showMapFlag) {
        if (isMappedWithLabelTable()) {
            /*
             * Show label table for each map.
             * However, some files contain only a single label table used
             * for all maps and this condition is detected if the first
             * two label tables use the same pointer.
             */
            const int32_t numMaps = getNumberOfMaps();
            bool haveLabelTableForEachMap = false;
            if (numMaps > 1) {
                if (getMapLabelTable(0) != getMapLabelTable(1)) {
                    haveLabelTableForEachMap = true;
                }
            }
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                const AString labelTableName = ("Label table for "
                                                + (haveLabelTableForEachMap
                                                   ? ("map " + AString::number(mapIndex + 1) + ": " + getMapName(mapIndex))
                                                   : ("ALL maps"))
                                                + "\n");
                
                dataFileInformation.addText(labelTableName
                                            + getMapLabelTable(mapIndex)->toFormattedString("    ")
                                            + "\n");
                
                if ( ! haveLabelTableForEachMap) {
                    break;
                }
            }
        }
    }
}

/**
 * @return File histogram number of buckets.
 */
int32_t
CaretMappableDataFile::getFileHistogramNumberOfBuckets() const
{
    /*
     * Metadata returns zero if integer value not found
     */
    const GiftiMetaData* metadata = getFileMetaData();
    CaretAssert(metadata);
    int32_t numBuckets = metadata->getInt(GiftiMetaDataXmlElements::HISTOGRAM_NUMBER_OF_BUCKETS);
    if (numBuckets <= 0) {
        numBuckets = 100;
    }
    return numBuckets;
}

/**
 * Set the file histogram number of buckets.
 *
 * @param numberOfBuckets
 *     Number of buckets.
 */
void
CaretMappableDataFile::setFileHistogramNumberOfBuckets(const int32_t numberOfBuckets)
{
    GiftiMetaData* metadata = getFileMetaData();
    CaretAssert(metadata);
    metadata->setInt(GiftiMetaDataXmlElements::HISTOGRAM_NUMBER_OF_BUCKETS,
                     numberOfBuckets);
}


/**
 * @return True if any of the maps in this file contain a
 * color mapping that possesses a modified status.
 */
bool
CaretMappableDataFile::isModifiedPaletteColorMapping() const
{
    if (isMappedWithPalette()) {
        const int32_t numMaps = getNumberOfMaps();
        
        for (int32_t i = 0; i < numMaps; i++) {
            if (getMapPaletteColorMapping(i)->isModified()) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @return The modified status for aall palettes in this file.
 * Note that 'modified' overrides any 'modified by show scene'.
 */
PaletteModifiedStatusEnum::Enum
CaretMappableDataFile::getPaletteColorMappingModifiedStatus() const
{
    PaletteModifiedStatusEnum::Enum modStatus = PaletteModifiedStatusEnum::UNMODIFIED;
    
    if (isMappedWithPalette()) {
        const int32_t numMaps = getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            switch (getMapPaletteColorMapping(i)->getModifiedStatus()) {
                case PaletteModifiedStatusEnum::MODIFIED:
                    modStatus = PaletteModifiedStatusEnum::MODIFIED;
                    break;
                case PaletteModifiedStatusEnum::MODIFIED_BY_SHOW_SCENE:
                    modStatus = PaletteModifiedStatusEnum::MODIFIED_BY_SHOW_SCENE;
                    break;
                case PaletteModifiedStatusEnum::UNMODIFIED:
                    break;
            }
            
            if (modStatus == PaletteModifiedStatusEnum::MODIFIED) {
                /*
                 * 'MODIFIED' overrides 'MODIFIED_BY_SHOW_SCENE'
                 * so no need to continue loop
                 */
                break;
            }
        }
    }
    
    return modStatus;
}

/**
 * @return True if the file is modified in any way EXCEPT for
 * the palette color mapping.  Also see isModified().
 */
bool
CaretMappableDataFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    return false;
}


/**
 * @return True if the file is modified in any way including
 * the palette color mapping.
 *
 * NOTE: While this method overrides that in the super class,
 * it is NOT virtual here.  Thus subclasses cannot override
 * this method and instead, subclasses should overrride
 * isModifiedExcludingPaletteColorMapping().
 */
bool
CaretMappableDataFile::isModified() const
{
    if (isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    
    if (isModifiedPaletteColorMapping()) {
        return true;
    }
    
    return false;
}

/**
 * Clear data in this file.
 */
void
CaretMappableDataFile::clear()
{
    CaretDataFile::clear();
    
    m_chartingDelegate.reset();
    
    m_mapThresholdFileSelectionModels.clear();
}

/**
 * Clear the modified status of this file.
 */
void
CaretMappableDataFile::clearModified()
{
    CaretDataFile::clearModified();
    
    if (m_chartingDelegate != NULL) {
        m_chartingDelegate->clearModified();
    }
}

/**
 * Create cartesian chart data from the given data.
 *
 * @param
 *     Data for the Y-axis.
 * @return 
 *     Pointer to the ChartDataCartesian instance.
 */
ChartDataCartesian*
CaretMappableDataFile::helpCreateCartesianChartData(const std::vector<float>& data)
{
    const int64_t numData = static_cast<int64_t>(data.size());
    
    /*
     * Some files may have time data but initially assume data-series
     */
    bool timeSeriesFlag = false;
    
    float convertTimeToSeconds = 1.0;
    switch (getMapIntervalUnits()) {
        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
            timeSeriesFlag = true;
            convertTimeToSeconds = 1000.0;
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
            convertTimeToSeconds = 1.0;
            timeSeriesFlag = true;
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
            convertTimeToSeconds = 1000000.0;
            timeSeriesFlag = true;
            break;
    }
    
    ChartDataCartesian* chartData = NULL;
    
    if (timeSeriesFlag) {
        chartData = new ChartDataCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
    }
    else {
        chartData = new ChartDataCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
    }

    if (chartData != NULL) {
        float timeStart = 0.0;
        float timeStep  = 1.0;
        if (timeSeriesFlag) {
            getMapIntervalStartAndStep(timeStart,
                                       timeStep);
            timeStart *= convertTimeToSeconds;
            timeStep  *= convertTimeToSeconds;
            chartData->setTimeStartInSecondsAxisX(timeStart);
            chartData->setTimeStepInSecondsAxisX(timeStep);
        }
        
        for (int64_t i = 0; i < numData; i++) {
            float xValue = i;
            
            if (timeSeriesFlag) {
                /*
                 * X-Value is "time"
                 */
                xValue = timeStart + (i * timeStep);
            }
            else {
                /*
                 * X-Value is the map index and map indices start at one
                 */
                xValue = i + 1;
            }
            
            chartData->addPoint(xValue,
                                data[i]);
        }
    }
    
    return chartData;
}

/**
 * Helper for getting chart data types supported by files that create
 * charts from brainordinates (multi-map files).
 * The chart data types are a function of the map interval units.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CaretMappableDataFile::helpGetSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    switch (getMapIntervalUnits()) {
        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
            chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
            chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
            CaretLogSevere("Units - PPM not supported");
            CaretAssertMessage(0, "Units - PPM not supported");
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
            chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
            chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES);
            break;
        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
            chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES);
            break;
    }
}

/**
 * Is a medial wall label in the label table for the given map index?
 *
 * NOTE: This does not test to see if a data element in
 *     the map is set to the key of the medial wall label.  So, it is
 *     possible that true is returned but no vertices or voxels are
 *     assigned to the medial wall.
 *
 * @param mapIndex
 *     Index of the map.
 * @return
 *     True if map the map's label table contains a medial wall label,
 *     else false.
 */
bool
CaretMappableDataFile::isMedialWallLabelInMapLabelTable(const int32_t mapIndex) const
{
    if (isMappedWithLabelTable()) {
        const GiftiLabelTable* labelTable = getMapLabelTable(mapIndex);
        return labelTable->hasMedialWallLabel();
    }
    
    return false;
}


/**
 * @return The label drawing properties for this file.  A valid pointer
 * will always be returned even if the file does not provide label data.
 */
LabelDrawingProperties*
CaretMappableDataFile::getLabelDrawingProperties()
{
    return m_labelDrawingProperties.get();
}

/**
 * @return The label drawing properties for this file.  A valid pointer
 * will always be returned even if the file does not provide label data.
 */
const LabelDrawingProperties*
CaretMappableDataFile::getLabelDrawingProperties() const
{
    return m_labelDrawingProperties.get();
}

/**
 * @return The charting delegate for this file.  Pointer
 * will never be NULL, even if file does not support charting.
 */
ChartableTwoFileDelegate*
CaretMappableDataFile::getChartingDelegate()
{
    if (m_chartingDelegate == NULL) {
        m_chartingDelegate = std::unique_ptr<ChartableTwoFileDelegate>(new ChartableTwoFileDelegate(this));
        
        m_chartingDelegate->updateAfterFileChanged();
    }
    return m_chartingDelegate.get();
}

/*
 * @return The charting delegate for this file.  Pointer
 * will never be NULL, even if file does not support charting.
 */
const ChartableTwoFileDelegate*
CaretMappableDataFile::getChartingDelegate() const
{
    if (m_chartingDelegate == NULL) {
        CaretMappableDataFile* thisFile = const_cast<CaretMappableDataFile*>(this);
        m_chartingDelegate = std::unique_ptr<ChartableTwoFileDelegate>(new ChartableTwoFileDelegate(thisFile));

        m_chartingDelegate->updateAfterFileChanged();
    }
    return m_chartingDelegate.get();
}

/**
 * Update the map threshold file selection models.
 */
void
CaretMappableDataFile::updateMapThresholdFileSelectionModels()
{
    const int32_t numMaps = getNumberOfMaps();
    const int32_t numThresh = static_cast<int32_t>(m_mapThresholdFileSelectionModels.size());
    if (numMaps > numThresh) {
        for (int32_t i = numThresh; i < numMaps; i++) {
            std::unique_ptr<CaretMappableDataFileAndMapSelectionModel> threshSel(new CaretMappableDataFileAndMapSelectionModel(this));
            threshSel->setSelectedFile(this);
            threshSel->setSelectedMapIndex(i);
            m_mapThresholdFileSelectionModels.push_back(std::move(threshSel));
        }
    }
    else if (numThresh > numMaps) {
        m_mapThresholdFileSelectionModels.resize(numMaps);
    }
}

/**
 * @return The thresholding file selection model for the given map index.
 */
CaretMappableDataFileAndMapSelectionModel*
CaretMappableDataFile::getMapThresholdFileSelectionModel(const int32_t mapIndex)
{
    updateMapThresholdFileSelectionModels();
    
    CaretAssertVectorIndex(m_mapThresholdFileSelectionModels, mapIndex);
    return m_mapThresholdFileSelectionModels[mapIndex].get();
}

/**
 * Update the charting delegate after changes (add a row/column, etc.)
 * are made to the data file.
 */
void
CaretMappableDataFile::updateAfterFileDataChanges()
{
    if (m_chartingDelegate) {
        m_chartingDelegate->updateAfterFileChanged();
    }

    m_applyToAllMapsSelected = isPaletteColorMappingEqualForAllMaps();
}

/**
 * @return Is apply palette color mapping to all maps selected.
 */
bool
CaretMappableDataFile::isApplyPaletteColorMappingToAllMaps() const
{
    return m_applyToAllMapsSelected;
}

/**
 * Set apply palette color mapping to all maps.  Only sets the status,
 * it does not change any palette color mapping.
 *
 * @param selected
 *     New selected status.
 */
void
CaretMappableDataFile::setApplyPaletteColorMappingToAllMaps(const bool selected)
{
    m_applyToAllMapsSelected = selected;
}

/**
 * @return True if file is mapped with a palette and one
 * palette is used for all maps.
 */
bool
CaretMappableDataFile::isOnePaletteUsedForAllMaps() const
{
    return false;
}

/**
 * @return The palette normalization mode for the file.
 * The default is NORMALIZATION_SELECTED_MAP_DATA.
 */
PaletteNormalizationModeEnum::Enum
CaretMappableDataFile::getPaletteNormalizationMode() const
{
    const AString textValue = getFileMetaData()->get(GiftiMetaDataXmlElements::METADATA_PALETTE_NORMALIZATION_MODE);
    bool validFlag = false;
    PaletteNormalizationModeEnum::Enum modeValue = PaletteNormalizationModeEnum::fromName(textValue, &validFlag);
    
    if ( ! validFlag) {
        modeValue = getDefaultPaletteNormalizationMode();
    }
    
    ensurePaletteNormalizationModeIsSupported(modeValue);
    
    return modeValue;
}

/**
 * Set the palette normalization mode for the file.  If the mode is not supported by the
 * file, the mode is not changed.
 *
 * @param mode
 *     New value for palette normalization mode.
 */
void
CaretMappableDataFile::setPaletteNormalizationMode(const PaletteNormalizationModeEnum::Enum modeIn)
{
    PaletteNormalizationModeEnum::Enum mode = modeIn;
    ensurePaletteNormalizationModeIsSupported(mode);
    getFileMetaData()->set(GiftiMetaDataXmlElements::METADATA_PALETTE_NORMALIZATION_MODE,
                           PaletteNormalizationModeEnum::toName(mode));
}

/**
 * Ensure that the given palette normalization mode is supported by the instance of this file.
 * If not supported, it is changed to that returned by getDefaultPaletteNormalizationMode().
 *
 * @param modeInOut
 *     Normalization mode that may be changed to a supported mode.
 */
void
CaretMappableDataFile::ensurePaletteNormalizationModeIsSupported(PaletteNormalizationModeEnum::Enum& modeInOut) const
{
    std::vector<PaletteNormalizationModeEnum::Enum> validModes;
    getPaletteNormalizationModesSupported(validModes);
    
    if (std::find(validModes.begin(),
                  validModes.end(),
                  modeInOut) == validModes.end()) {
        modeInOut = getDefaultPaletteNormalizationMode();
    }
}

/**
 * @return The default palette normalization mode for this file which is always
 * the first mode in the supported modes from getPaletteNormalizationModesSupported().
 */
PaletteNormalizationModeEnum::Enum
CaretMappableDataFile::getDefaultPaletteNormalizationMode() const
{
    PaletteNormalizationModeEnum::Enum mode = PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA;
    
    std::vector<PaletteNormalizationModeEnum::Enum> validModes;
    getPaletteNormalizationModesSupported(validModes);
    if ( ! validModes.empty()) {
        CaretAssertVectorIndex(validModes, 0);
        mode = validModes[0];
    }
    
    return mode;
}

/**
 * @return Is the data in the file mapped to colors using
 * Red, Green, Blue, Alpha values.
 */
bool
CaretMappableDataFile::isMappedWithRGBA() const
{
    return false;
}

bool CaretMappableDataFile::hasCiftiXML() const
{
    return false;
}

const CiftiXML CaretMappableDataFile::getCiftiXML() const
{
    return CiftiXML();
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param structure
 *    Structure of the surface.
 * @param nodeIndex
 *    Index of the node.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @param dataValueSeparator
 *    Separator between multiple data values
 * @param textOut
 *    Output containing identification information.
 */
bool
CaretMappableDataFile::getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& /*mapIndices*/,
                                                           const StructureEnum::Enum /*structure*/,
                                                           const int /*nodeIndex*/,
                                                           const int32_t /*numberOfNodes*/,
                                                           const AString& /*dataValueSeparator*/,
                                                           AString& textOut) const
{
    textOut.clear();
    return false;
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param dataValueSeparator
 *    Separator between multiple data values
 * @param textOut
 *    Output containing identification information.
 */
bool
CaretMappableDataFile::getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& /*mapIndices*/,
                                                           const float* /*xyz[3]*/,
                                                           const AString& /*dataValueSeparator*/,
                                                           int64_t* /*ijkOut[3]*/,
                                                           AString& textOut) const
{
    textOut.clear();
    return false;
}

/**
 * @return The file identification attributes
 */
FileIdentificationAttributes*
CaretMappableDataFile::getFileIdentificationAttributes()
{
    return m_fileIdentificationAttributes.get();
}

/**
 * @return The file identification attributes (const method)
 */
const FileIdentificationAttributes*
CaretMappableDataFile::getFileIdentificationAttributes() const
{
    return m_fileIdentificationAttributes.get();
}


