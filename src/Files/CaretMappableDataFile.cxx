
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
#include "ChartTwoDataCartesianHistory.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiXML.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "FastStatistics.h"
#include "FileInformation.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaDataXmlElements.h"
#include "Histogram.h"
#include "HistogramDrawingInfo.h"
#include "LabelDrawingProperties.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneAttributes.h"
#include "StringTableModel.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * Constructor.
 */
CaretMappableDataFile::CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    initializeCaretMappableDataFileInstance();
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
    initializeCaretMappableDataFileInstance();
    
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
 */
void
CaretMappableDataFile::initializeCaretMappableDataFileInstance()
{
    m_labelDrawingProperties = std::unique_ptr<LabelDrawingProperties>(new LabelDrawingProperties());
}


/**
 * Assists with copying instances of this class.
 */
void 
CaretMappableDataFile::copyCaretMappableDataFile(const CaretMappableDataFile& cmdf)
{
    setPaletteNormalizationMode(cmdf.getPaletteNormalizationMode());
    *m_labelDrawingProperties = *cmdf.m_labelDrawingProperties;
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
CaretMappableDataFile::updateScalarColoringForAllMaps(const PaletteFile* paletteFile)
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        updateScalarColoringForMap(iMap,
                             paletteFile);
    }
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
        
        if (sceneAttributes->isModifiedPaletteSettingsSavedToScene()) {
            std::vector<SceneClass*> pcmClassVector;
            
            const int32_t numMaps = getNumberOfMaps();
            for (int32_t i = 0; i < numMaps; i++) {
                const PaletteColorMapping* pcmConst = getMapPaletteColorMapping(i);
                if (pcmConst->isModified()) {
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
    const SceneClass* chartingDelegateClass = sceneClass->getClass("m_chartingDelegate");
    m_chartingDelegate.reset();
    if (chartingDelegateClass != NULL) {
        ChartableTwoFileDelegate* chartingDelegate = getChartingDelegate(); // creates charting delegate if not valid
        chartingDelegate->updateAfterFileChanged();
        chartingDelegate->restoreFromScene(sceneAttributes,
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
        
        const int32_t numMaps = getNumberOfMaps();
        const SceneClassArray* pcmArray = sceneClass->getClassArray("savedPaletteColorMappingArray");
        if (pcmArray != NULL) {
            const int32_t numElements = pcmArray->getNumberOfArrayElements();
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
                        pcmMap->copy(pcm);
                        pcmMap->clearModified();
                        
                        /*
                         * WB-522 When palette loaded from scene,
                         * mark it as modified.
                         */
                        pcmMap->setModified();
                        
                        /*
                         * Volume file needs it's map coloring updated since
                         * palette has changed.
                         */
                        VolumeFile* volumeFile = dynamic_cast<VolumeFile*>(this);
                        if (volumeFile != NULL) {
                            volumeFile->updateScalarColoringForMap(restoreMapIndex,
                                                                   NULL);
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
                    const AString msg = ("Unable to find map for restoring palette settings for file: "
                                         + getFileNameNoPath()
                                         + "  Map Name: "
                                         + mapName
                                         + "  Map Index: "
                                         + AString::number(mapIndex));
                    sceneAttributes->addToErrorMessage(msg);
                }
            }
        }
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
 * Get the histogram bounds, heights, and RGBA coloring.
 *
 * @param mapIndex
 *     Index of map.
 * @param useDataFromAllMapsFlag
 *     If true, data from ALL maps is used not just the map specified by map index.
 * @param addEndPointForQwtFlag
 *     If true, add point at end for drawing with Qwt.
 * @param histogramDrawingInfoOut
 *     Output containing histogram drawing information.
 * @return
 *     True if output data is valid, else false.
 */
bool
CaretMappableDataFile::getMapHistogramDrawingInfo(const int32_t mapIndex,
                                                  const bool useDataFromAllMapsFlag,
                                                  const bool addEndPointForQwtFlag,
                                                  HistogramDrawingInfo& histogramDrawingInfoOut,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    histogramDrawingInfoOut.reset();
    
    if ( ! isMappedWithPalette()) {
        errorMessageOut = "File is not mapped with palette!";
        return false;
    }
    if ((mapIndex < 0)
        || (mapIndex >= getNumberOfMaps())) {
        CaretAssertMessage(0, "Invalid map index.");
        errorMessageOut = "Invalid map index.";
        return false;
    }
    
    PaletteColorMapping* paletteColorMapping = getMapPaletteColorMapping(mapIndex);
    CaretAssert(paletteColorMapping);
    
    EventPaletteGetByName paletteEvent(paletteColorMapping->getSelectedPaletteName());
    EventManager::get()->sendEvent(paletteEvent.getPointer());
    const Palette* palette = paletteEvent.getPalette();
    if (palette == NULL) {
        errorMessageOut = ("Unable to find palette named: "
                           + paletteColorMapping->getSelectedPaletteName());
        return false;
    }
    
    FastStatistics* statistics = NULL;
    if (useDataFromAllMapsFlag) {
        statistics = const_cast<FastStatistics*>(getFileFastStatistics());
    }
    else {
        switch (getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getMapFastStatistics(mapIndex));
                break;
        }
    }
    
    /*
     * Statistics may be NULL for connectivity files (dense, dense dynamic)
     * that have not yet loaded any data caused by the user clicking
     * brainordinates.
     */
    if (statistics == NULL) {
        return false;
    }
    
    CaretAssert(statistics);
    float mostPos  = 0.0;
    float leastPos = 0.0;
    float leastNeg = 0.0;
    float mostNeg  = 0.0;
    bool matchFlag = false;
    
    switch (paletteColorMapping->getHistogramRangeMode()) {
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_ALL:
        {
            float dummy;
            statistics->getNonzeroRanges(mostNeg, dummy, dummy, mostPos);
        }
            break;
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_MATCH_PALETTE:
        {
            matchFlag = true;
            switch (paletteColorMapping->getScaleMode()) {
                case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                    mostPos  = statistics->getMax();
                    leastPos = 0.0;
                    leastNeg = 0.0;
                    mostNeg  = statistics->getMin();
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
                    mostPos  =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
                    leastPos =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
                    leastNeg = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
                    mostNeg  = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                    mostPos  = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMaximum());
                    leastPos = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMinimum());
                    leastNeg = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMinimum());
                    mostNeg  = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMaximum());
                    break;
                case PaletteScaleModeEnum::MODE_USER_SCALE:
                    mostPos  = paletteColorMapping->getUserScalePositiveMaximum();
                    leastPos = paletteColorMapping->getUserScalePositiveMinimum();
                    leastNeg = paletteColorMapping->getUserScaleNegativeMinimum();
                    mostNeg  = paletteColorMapping->getUserScaleNegativeMaximum();
                    break;
            }
        }
            break;
    }
    
    /*
     * Remove data that is not displayed
     */
    bool isZeroIncluded = true;
    const Histogram* histogram = NULL;
    if (matchFlag) {
        isZeroIncluded = paletteColorMapping->isDisplayZeroDataFlag();
        
        if ( ! paletteColorMapping->isDisplayNegativeDataFlag()) {
            mostNeg  = 0.0;
            leastNeg = 0.0;
        }
        if ( ! paletteColorMapping->isDisplayPositiveDataFlag()) {
            mostPos  = 0.0;
            leastPos = 0.0;
        }
        
        if (useDataFromAllMapsFlag) {
            histogram = getFileHistogram(mostPos,
                                         leastPos,
                                         leastNeg,
                                         mostNeg,
                                         isZeroIncluded);
        }
        else {
            switch (getPaletteNormalizationMode()) {
                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                    histogram = getFileHistogram(mostPos,
                                                 leastPos,
                                                 leastNeg,
                                                 mostNeg,
                                                 isZeroIncluded);
                    break;
                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                    histogram = getMapHistogram(mapIndex,
                                                mostPos,
                                                leastPos,
                                                leastNeg,
                                                mostNeg,
                                                isZeroIncluded);
                    break;
            }
        }
    }
    else {
        if (useDataFromAllMapsFlag) {
            histogram = getFileHistogram();
        }
        else {
            switch (getPaletteNormalizationMode()) {
                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                    histogram = getFileHistogram();
                    break;
                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                    histogram = getMapHistogram(mapIndex);
                    break;
            }
        }
    }
    
    CaretAssert(histogram);
    
    
    float minValueX = 0.0;
    float maxValueX = 0.0;
    histogram->getRange(minValueX,
                        maxValueX);
    const float valueRangeX = maxValueX - minValueX;
    if (valueRangeX <= 0.0) {
        errorMessageOut = "Range of histogram X-values is zero";
        return false;
    }
    
    if ((paletteColorMapping != NULL)
        && (statistics != NULL)
        && (palette != NULL)
        && (histogram != NULL)) {
        std::vector<float> histogramBuckets = histogram->getHistogramDisplay();
        const int32_t numBucketValues = static_cast<int32_t>(histogramBuckets.size());
        if (numBucketValues < 2) {
            errorMessageOut = "Histogram must contain two or more values";
            return false;
        }
        
        int32_t numberOfDataPoints = (addEndPointForQwtFlag
                                      ? (numBucketValues + 1)
                                      : numBucketValues);
        
        /*
         * Set up vectors for X/Y values and coloring
         */
        histogramDrawingInfoOut.initialize(numberOfDataPoints);
        
        const float stepX = (valueRangeX
                             / static_cast<float>(numBucketValues));
        for (int64_t ix = 0; ix < numBucketValues; ix++) {
            float valueX = (minValueX
                            + (ix * stepX));
            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, ix);
            histogramDrawingInfoOut.m_dataX[ix] = valueX;
            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataY, ix);
            CaretAssertVectorIndex(histogramBuckets, ix);
            histogramDrawingInfoOut.m_dataY[ix] = histogramBuckets[ix];
        }
        if (addEndPointForQwtFlag) {
            /*
             * Duplicate last point for Qwt or else Qwt may not draw correctly
             */
            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, numBucketValues);
            histogramDrawingInfoOut.m_dataX[numBucketValues] = histogramDrawingInfoOut.m_dataX[numBucketValues - 1];
            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataY, numBucketValues);
            histogramDrawingInfoOut.m_dataY[numBucketValues] = histogramDrawingInfoOut.m_dataY[numBucketValues - 1];
        }
        
        /*
         * Adjust X-value of first and last points
         */
        histogramDrawingInfoOut.m_dataX[0] = minValueX;
        histogramDrawingInfoOut.m_dataX[numberOfDataPoints - 1] = maxValueX;
        
        
        /*
         * Always color using palette and it will assign a zero alpha
         * value to buckets that do not fall in the palette negative
         * postive min/max ranges.
         */
        const float* xValuesArray = &histogramDrawingInfoOut.m_dataX[0];
        float* dataRGBA = &histogramDrawingInfoOut.m_dataRGBA[0];
        NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                      paletteColorMapping,
                                                      palette,
                                                      xValuesArray,
                                                      xValuesArray,
                                                      numberOfDataPoints,
                                                      dataRGBA,
                                                      true); // ignore thresholding
        
        /*
         * If bucket is not colored (zero alpha) set bucket height to zero
         */
        for (int32_t i = 0; i < numberOfDataPoints; i++) {
            const int32_t alphaIndex = i * 4 + 3;
            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataRGBA, alphaIndex);
            if (histogramDrawingInfoOut.m_dataRGBA[alphaIndex] <= 0.0) {
                CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataY, i);
                histogramDrawingInfoOut.m_dataY[i] = 0.0;
            }
        }
        
        /*
         * If selected, apply solid color to histogram
         */
        const CaretColorEnum::Enum histogramColor = paletteColorMapping->getHistogramColor();
        if (histogramColor != CaretColorEnum::CUSTOM) {
            float rgba[4];
            CaretColorEnum::toRGBAFloat(histogramColor, rgba);
            
            /*
             * Note: DO NOT override alpha value that was set 
             * using palette coloring
             */
            for (int32_t i = 0; i < numberOfDataPoints; i++) {
                const int32_t i4 = i * 4;
                CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataRGBA, i4 + 3);
                histogramDrawingInfoOut.m_dataRGBA[i4]   = rgba[0];
                histogramDrawingInfoOut.m_dataRGBA[i4+1] = rgba[1];
                histogramDrawingInfoOut.m_dataRGBA[i4+2] = rgba[2];
            }
        }
        
        bool setThresholdingFlag = false;
        switch (paletteColorMapping->getThresholdType()) {
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                setThresholdingFlag = true;
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
                break;
        }
        
        if (setThresholdingFlag) {
            float bounds[4];
            if (histogramDrawingInfoOut.getBounds(bounds)) {
                const float minX = bounds[0];
                const float maxX = bounds[1];
                const float minY = bounds[2];
                const float maxY = bounds[3];
                
                float threshMinValue = paletteColorMapping->getThresholdNormalMinimum();
                float threshMaxValue = paletteColorMapping->getThresholdNormalMaximum();
                switch (paletteColorMapping->getThresholdTest()) {
                    case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                    {
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ.resize(12);
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[0] = minX;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[1] = minY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[2] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[3] = threshMinValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[4] = minY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[5] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[6] = threshMinValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[7] = maxY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[8] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[9] = minX;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[10] = maxY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[11] = 0.0;
                        
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ.resize(12);
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[0] = threshMaxValue;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[1] = minY;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[2] = 0.0;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[3] = maxX;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[4] = minY;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[5] = 0.0;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[6] = maxX;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[7] = maxY;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[8] = 0.0;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[9] = threshMaxValue;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[10] = maxY;
                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[11] = 0.0;
                    }
                        break;
                    case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                    {
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ.resize(12);
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[0] = threshMinValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[1] = minY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[2] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[3] = threshMaxValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[4] = minY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[5] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[6] = threshMaxValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[7] = maxY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[8] = 0.0;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[9] = threshMinValue;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[10] = maxY;
                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[11] = 0.0;
                    }
                        break;
                }
                
                switch (paletteColorMapping->getThresholdRangeMode()) {
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE:
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP:
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_UNLIMITED:
                        break;
                }
            }
        }
        
        CaretAssert(histogramDrawingInfoOut.isValid());
        return histogramDrawingInfoOut.isValid();
    }
    
    return false;
}

///**
// * Get the histogram bounds, heights, and RGBA coloring.
// *
// * @param mapIndex
// *     Index of map.
// * @param useDataFromAllMapsFlag
// *     If true, data from ALL maps is used not just the map specified by map index.
// * @param addEndPointForQwtFlag
// *     If true, add point at end for drawing with Qwt.
// * @param histogramDrawingInfoOut
// *     Output containing histogram drawing information.
// * @return
// *     True if output data is valid, else false.
// */
//bool
//CaretMappableDataFile::getMapHistogramDrawingInfo(const int32_t mapIndex,
//                                                  const bool useDataFromAllMapsFlag,
//                                               const bool addEndPointForQwtFlag,
//                                               HistogramDrawingInfo& histogramDrawingInfoOut,
//                                               AString& errorMessageOut) 
//{
//    errorMessageOut.clear();
//    histogramDrawingInfoOut.reset();
//    
//    if ( ! isMappedWithPalette()) {
//        errorMessageOut = "File is not mapped with palette!";
//        return false;
//    }
//    if ((mapIndex < 0)
//        || (mapIndex >= getNumberOfMaps())) {
//        CaretAssertMessage(0, "Invalid map index.");
//        errorMessageOut = "Invalid map index.";
//        return false;
//    }
//    
//    PaletteColorMapping* paletteColorMapping = getMapPaletteColorMapping(mapIndex);
//    CaretAssert(paletteColorMapping);
//    
//    EventPaletteGetByName paletteEvent(paletteColorMapping->getSelectedPaletteName());
//    EventManager::get()->sendEvent(paletteEvent.getPointer());
//    const Palette* palette = paletteEvent.getPalette();
//    if (palette == NULL) {
//        errorMessageOut = ("Unable to find palette named: "
//                           + paletteColorMapping->getSelectedPaletteName());
//        return false;
//    }
//    
//    FastStatistics* statistics = NULL;
//    if (useDataFromAllMapsFlag) {
//        statistics = const_cast<FastStatistics*>(getFileFastStatistics());
//    }
//    else {
//        switch (getPaletteNormalizationMode()) {
//            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
//                statistics = const_cast<FastStatistics*>(getFileFastStatistics());
//                break;
//            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
//                statistics = const_cast<FastStatistics*>(getMapFastStatistics(mapIndex));
//                break;
//        }
//    }
//    
//    /*
//     * Statistics may be NULL for connectivity files (dense, dense dynamic)
//     * that have not yet loaded any data caused by the user clicking
//     * brainordinates.
//     */
//    if (statistics == NULL) {
//        return false;
//    }
//    
//    CaretAssert(statistics);
//    float mostPos  = 0.0;
//    float leastPos = 0.0;
//    float leastNeg = 0.0;
//    float mostNeg  = 0.0;
//    bool matchFlag = false;
//    
//    switch (paletteColorMapping->getHistogramRangeMode()) {
//        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_ALL:
//        {
//            float dummy;
//            statistics->getNonzeroRanges(mostNeg, dummy, dummy, mostPos);
//        }
//            break;
//        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_MATCH_PALETTE:
//        {
//            matchFlag = true;
//            switch (paletteColorMapping->getScaleMode()) {
//                case PaletteScaleModeEnum::MODE_AUTO_SCALE:
//                    mostPos  = statistics->getMax();
//                    leastPos = 0.0;
//                    leastNeg = 0.0;
//                    mostNeg  = statistics->getMin();
//                    break;
//                case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
//                    mostPos  =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
//                    leastPos =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
//                    leastNeg = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
//                    mostNeg  = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
//                    break;
//                case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
//                    mostPos  = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMaximum());
//                    leastPos = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMinimum());
//                    leastNeg = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMinimum());
//                    mostNeg  = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMaximum());
//                    break;
//                case PaletteScaleModeEnum::MODE_USER_SCALE:
//                    mostPos  = paletteColorMapping->getUserScalePositiveMaximum();
//                    leastPos = paletteColorMapping->getUserScalePositiveMinimum();
//                    leastNeg = paletteColorMapping->getUserScaleNegativeMinimum();
//                    mostNeg  = paletteColorMapping->getUserScaleNegativeMaximum();
//                    break;
//            }
//        }
//            break;
//    }
//
//    /*
//     * Remove data that is not displayed
//     */
//    bool isZeroIncluded = true;
//    const Histogram* histogram = NULL;
//    if (matchFlag) {
//        isZeroIncluded = paletteColorMapping->isDisplayZeroDataFlag();
//        
//        if ( ! paletteColorMapping->isDisplayNegativeDataFlag()) {
//            mostNeg  = 0.0;
//            leastNeg = 0.0;
//        }
//        if ( ! paletteColorMapping->isDisplayPositiveDataFlag()) {
//            mostPos  = 0.0;
//            leastPos = 0.0;
//        }
//        
//        if (useDataFromAllMapsFlag) {
//            histogram = getFileHistogram(mostPos,
//                                         leastPos,
//                                         leastNeg,
//                                         mostNeg,
//                                         isZeroIncluded);
//        }
//        else {
//            switch (getPaletteNormalizationMode()) {
//                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
//                    histogram = getFileHistogram(mostPos,
//                                                 leastPos,
//                                                 leastNeg,
//                                                 mostNeg,
//                                                 isZeroIncluded);
//                    break;
//                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
//                    histogram = getMapHistogram(mapIndex,
//                                                mostPos,
//                                                leastPos,
//                                                leastNeg,
//                                                mostNeg,
//                                                isZeroIncluded);
//                    break;
//            }
//        }
//    }
//    else {
//        if (useDataFromAllMapsFlag) {
//            histogram = getFileHistogram();
//        }
//        else {
//            switch (getPaletteNormalizationMode()) {
//                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
//                    histogram = getFileHistogram();
//                    break;
//                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
//                    histogram = getMapHistogram(mapIndex);
//                    break;
//            }
//        }
//    }
//    
//    CaretAssert(histogram);
//    
//    
//    float minValueX = 0.0;
//    float maxValueX = 0.0;
//    histogram->getRange(minValueX,
//                        maxValueX);
//    const float valueRangeX = maxValueX - minValueX;
//    if (valueRangeX <= 0.0) {
//        errorMessageOut = "Range of histogram X-values is zero";
//        return false;
//    }
//    
//    if ((paletteColorMapping != NULL)
//        && (statistics != NULL)
//        && (palette != NULL)
//        && (histogram != NULL)) {
//        std::vector<float> histogramBuckets = histogram->getHistogramDisplay();
//        const int32_t numBucketValues = static_cast<int32_t>(histogramBuckets.size());
//        if (numBucketValues < 2) {
//            errorMessageOut = "Histogram must contain two or more values";
//            return false;
//        }
//        
//        int32_t numberOfDataPoints = (addEndPointForQwtFlag
//                                         ? (numBucketValues + 1)
//                                         : numBucketValues);
//        
//        histogramDrawingInfoOut.initialize(numberOfDataPoints);
//        
//        if (addEndPointForQwtFlag) {
//            /*
//             * Duplicate last point for Qwt or else Qwt may not draw correctly
//             */
//            histogramBuckets.push_back(histogramBuckets[histogramBuckets.size() - 1]);
//        }
//        histogramDrawingInfoOut.m_dataY = histogramBuckets;
//        
//        const bool hideZerosFlag = ( ! paletteColorMapping->isDisplayZeroDataFlag());
//        const float stepX = ((valueRangeX)
//                            / static_cast<float>(numberOfDataPoints));
//    
//        /*
//         * Need to find zero (or near zero if there is not an exact zero value)
//         */
//        int32_t indexZeroX = -1;
//        float indexZeroDistance = std::numeric_limits<float>::max();
//        
//        for (int64_t ix = 0; ix < numberOfDataPoints; ix++) {
//            float valueX = (minValueX
//                           + (ix * stepX));
//            
//            if (hideZerosFlag) {
//                const float zeroDistance = ((valueX >= 0) ? valueX : -valueX);
//                if (zeroDistance < indexZeroDistance) {
//                    indexZeroX = ix;
//                    indexZeroDistance = zeroDistance;
//                }
//            }
//            
//            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, ix);
//            histogramDrawingInfoOut.m_dataX[ix] = valueX;
//        }
//        
//        if (indexZeroX >= 0) {
//            const float posZero = 0.05;
//            const float negZero = -posZero;
//            int32_t negZeroIndex = -1;
//            
//            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, indexZeroX);
//            const float xValue = histogramDrawingInfoOut.m_dataX[indexZeroX];
//            if (xValue <= 0.0) {
//                negZeroIndex = indexZeroX;
//            }
//            else {
//                negZeroIndex = indexZeroX - 1;
//            }
//            
//            if ((negZeroIndex >= 0)
//                && (negZeroIndex < (numberOfDataPoints - 1))) {
//                CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, negZeroIndex + 1);
//                CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataY, negZeroIndex + 1);
//                histogramDrawingInfoOut.m_dataX[negZeroIndex] = negZero;
//                histogramDrawingInfoOut.m_dataY[negZeroIndex] = 0.0;
//                
//                histogramDrawingInfoOut.m_dataX.insert(histogramDrawingInfoOut.m_dataX.begin() + negZeroIndex + 1, posZero);
//                histogramDrawingInfoOut.m_dataY.insert(histogramDrawingInfoOut.m_dataY.begin() + negZeroIndex + 1, 0.0);
//            }
//            numberOfDataPoints = static_cast<int32_t>(histogramDrawingInfoOut.m_dataX.size());
//            CaretAssert(histogramDrawingInfoOut.m_dataX.size() == histogramDrawingInfoOut.m_dataY.size());
//            histogramDrawingInfoOut.m_dataRGBA.resize(numberOfDataPoints * 4);
//        }
//        
////        for (int64_t ix = 0; ix < numberOfDataPoints; ix++) {
////            CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataX, ix);
////            std::cout << "value " << ix << ": " << histogramDrawingInfoOut.m_dataX[ix] << "," << histogramDrawingInfoOut.m_dataX[ix] << std::endl;
////        }
//        
//        histogramDrawingInfoOut.m_dataX[0] = minValueX;
//        histogramDrawingInfoOut.m_dataX[numberOfDataPoints - 1] = maxValueX;
//        
//        const float* xValuesArray = &histogramDrawingInfoOut.m_dataX[0];
//        
//        const CaretColorEnum::Enum histogramColor = paletteColorMapping->getHistogramColor();
//        if (histogramColor != CaretColorEnum::CUSTOM) {
//            float rgba[4];
//            CaretColorEnum::toRGBAFloat(histogramColor, rgba);
//            rgba[3] = 1.0;
//            
//            for (int32_t i = 0; i < numberOfDataPoints; i++) {
//                const int32_t i4 = i * 4;
//                CaretAssertVectorIndex(histogramDrawingInfoOut.m_dataRGBA, i4 + 3);
//                histogramDrawingInfoOut.m_dataRGBA[i4]   = rgba[0];
//                histogramDrawingInfoOut.m_dataRGBA[i4+1] = rgba[1];
//                histogramDrawingInfoOut.m_dataRGBA[i4+2] = rgba[2];
//                histogramDrawingInfoOut.m_dataRGBA[i4+3] = rgba[3];
//            }
//        }
//        else {
//            float* dataRGBA = &histogramDrawingInfoOut.m_dataRGBA[0];
//            
//            NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
//                                                          paletteColorMapping,
//                                                          palette,
//                                                          xValuesArray,
//                                                          xValuesArray,
//                                                          numberOfDataPoints,
//                                                          dataRGBA,
//                                                          true); // ignore thresholding
//        }
//        
//        bool setThresholdingFlag = false;
//        switch (paletteColorMapping->getThresholdType()) {
//            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
//                break;
//            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
//                break;
//            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
//                setThresholdingFlag = true;
//                break;
//            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
//                break;
//        }
//        
//        if (setThresholdingFlag) {
//            float bounds[4];
//            if (histogramDrawingInfoOut.getBounds(bounds)) {
//                const float minX = bounds[0];
//                const float maxX = bounds[1];
//                const float minY = bounds[2];
//                const float maxY = bounds[3];
//                
//                float threshMinValue = paletteColorMapping->getThresholdNormalMinimum();
//                float threshMaxValue = paletteColorMapping->getThresholdNormalMaximum();
//                switch (paletteColorMapping->getThresholdTest()) {
//                    case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
//                    {
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ.resize(12);
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[0] = minX;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[1] = minY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[2] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[3] = threshMinValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[4] = minY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[5] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[6] = threshMinValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[7] = maxY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[8] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[9] = minX;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[10] = maxY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[11] = 0.0;
//
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ.resize(12);
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[0] = threshMaxValue;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[1] = minY;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[2] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[3] = maxX;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[4] = minY;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[5] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[6] = maxX;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[7] = maxY;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[8] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[9] = threshMaxValue;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[10] = maxY;
//                        histogramDrawingInfoOut.m_thresholdTwoBoundsXYZ[11] = 0.0;
//                    }
//                        break;
//                    case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
//                    {
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ.resize(12);
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[0] = threshMinValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[1] = minY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[2] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[3] = threshMaxValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[4] = minY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[5] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[6] = threshMaxValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[7] = maxY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[8] = 0.0;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[9] = threshMinValue;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[10] = maxY;
//                        histogramDrawingInfoOut.m_thresholdOneBoundsXYZ[11] = 0.0;
//                    }
//                        break;
//                }
//                
//                switch (paletteColorMapping->getThresholdRangeMode()) {
//                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE:
//                        break;
//                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP:
//                        break;
//                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_UNLIMITED:
//                        break;
//                }
//            }
//        }
//        
//        CaretAssert(histogramDrawingInfoOut.isValid());
//        return histogramDrawingInfoOut.isValid();
//    }
//    
//    return false;
//}

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
 * @return The palette normalization mode for the file.
 * The default is NORMALIZATION_SELECTED_MAP_DATA.
 */
PaletteNormalizationModeEnum::Enum
CaretMappableDataFile::getPaletteNormalizationMode() const
{
    PaletteNormalizationModeEnum::Enum modeValue = PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA;
    
    const AString textValue = getFileMetaData()->get(GiftiMetaDataXmlElements::METADATA_PALETTE_NORMALIZATION_MODE);
    bool validFlag = false;
    modeValue = PaletteNormalizationModeEnum::fromName(textValue, &validFlag);
    
    if ( ! validFlag) {
        std::vector<PaletteNormalizationModeEnum::Enum> validModes;
        getPaletteNormalizationModesSupported(validModes);
        
        if ( ! validModes.empty()) {
            CaretAssertVectorIndex(validModes, 0);
            modeValue = validModes[0];
        }
        else {
            modeValue = PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA;
        }
    }
    
    return modeValue;
}

/**
 * Set the palette normalization mode for the file.
 *
 * @param mode
 *     New value for palette normalization mode.
 */
void
CaretMappableDataFile::setPaletteNormalizationMode(const PaletteNormalizationModeEnum::Enum mode)
{
    getFileMetaData()->set(GiftiMetaDataXmlElements::METADATA_PALETTE_NORMALIZATION_MODE,
                           PaletteNormalizationModeEnum::toName(mode));
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

///**
// * @return The CaretMappableDataFile that implements this interface.
// */
//CaretMappableDataFile*
//CaretMappableDataFile::getAsCaretMappableDataFile()
//{
//    return this;
//}
//
///**
// * @return The CaretMappableDataFile that implements this interface.
// */
//const CaretMappableDataFile*
//CaretMappableDataFile::getAsCaretMappableDataFile() const
//{
//    return this;
//}
//
//
///**
// * Does this file support any type of charting?
// */
//bool
//CaretMappableDataFile::isChartingSupported() const
//{
//    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
//    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
//    
//    return ( ! chartCompoundDataTypes.empty());
//}
//
///**
// * Test for support of the given chart data type.
// *
// * @param chartDataType
// *     Type of chart data.
// * @return
// *     True if the chart data type is supported, else false.
// */
//bool
//CaretMappableDataFile::isChartingSupportedForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const
//{
//    std::vector<ChartTwoDataTypeEnum::Enum> chartDataTypes;
//    getSupportedChartDataTypes(chartDataTypes);
//    
//    if (std::find(chartDataTypes.begin(),
//                  chartDataTypes.end(),
//                  chartDataType) != chartDataTypes.end()) {
//        return true;
//    }
//    
//    return false;
//}
//
///**
// * Test for support of the given chart compound data type.
// *
// * @param chartCompoundDataType
// *     Type of chart compound data.
// * @return
// *     True if the chart compound data type is supported, else false.
// */
//bool
//CaretMappableDataFile::isChartingSupportedForChartCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const
//{
//    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
//    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
//    
//    for (auto& ccdt : chartCompoundDataTypes) {
//        if (ccdt == chartCompoundDataType) {
//            return true;
//        }
//    }
//    
//    return false;
//}
//
///**
// * Get chart data types supported by this file.
// *
// * @param chartDataTypesOut
// *     Output containing all chart data types supported by this data file.
// */
//void
//CaretMappableDataFile::getSupportedChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const
//{
//    chartDataTypesOut.clear();
//
//    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
//    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
//    
//    for (auto& ccdt : chartCompoundDataTypes) {
//        chartDataTypesOut.push_back(ccdt.getChartDataType());
//    }
//}
//
///**
// * Get chart data types supported by this file.
// *
// * @param chartDataTypesOut
// *     Output containing all chart data types supported by this data file.
// */
//void
//CaretMappableDataFile::getSupportedChartCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const
//{
//    chartCompoundDataTypesOut.clear();
//
//    bool hasHistogramFlag  = false;
//    bool hasBrainordinateLineSeriesFlag = false;
//    bool hasNoMapsLineSeriesFlag = false;
//    bool hasMatrixFlag     = false;
//    
//    switch (getDataFileType()) {
//        case DataFileTypeEnum::CONNECTIVITY_DENSE:
//            hasHistogramFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
//            hasHistogramFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
//            hasHistogramFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
//            hasHistogramFlag = true;
//            hasBrainordinateLineSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//            hasHistogramFlag  = true;
//            hasBrainordinateLineSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
//            hasHistogramFlag = true;
//            hasMatrixFlag    = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
//            hasHistogramFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
//            //hasMatrixFlag    = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
//            hasHistogramFlag  = true;
//            hasBrainordinateLineSeriesFlag = true;
//            hasMatrixFlag     = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
//            hasHistogramFlag  = true;
//            hasBrainordinateLineSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
//            hasHistogramFlag  = true;
//            hasNoMapsLineSeriesFlag = true;
//            hasMatrixFlag     = true;
//            break;
//        case DataFileTypeEnum::ANNOTATION:
//            break;
//        case DataFileTypeEnum::BORDER:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
//            break;
//        case DataFileTypeEnum::FOCI:
//            break;
//        case DataFileTypeEnum::IMAGE:
//            break;
//        case DataFileTypeEnum::LABEL:
//            break;
//        case DataFileTypeEnum::METRIC:
//            hasHistogramFlag  = true;
//            hasBrainordinateLineSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::PALETTE:
//            break;
//        case DataFileTypeEnum::RGBA:
//            break;
//        case DataFileTypeEnum::SCENE:
//            break;
//        case DataFileTypeEnum::SPECIFICATION:
//            break;
//        case DataFileTypeEnum::SURFACE:
//            break;
//        case DataFileTypeEnum::UNKNOWN:
//            break;
//        case DataFileTypeEnum::VOLUME:
//            hasHistogramFlag  = true;
//            hasBrainordinateLineSeriesFlag = true;
//            break;
//    }
//    
//    if (hasHistogramFlag) {
//        if (getNumberOfMaps() > 0) {
//            chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForHistogram());
//        }
//    }
//    
//    if (hasBrainordinateLineSeriesFlag) {
//        if (getNumberOfMaps() > 1) {
//            const NiftiTimeUnitsEnum::Enum mapUnits = getMapIntervalUnits();
//            ChartAxisUnitsEnum::Enum xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
//            switch (mapUnits) {
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                    CaretAssert(0);
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                    break;
//            }
//            const int32_t numMaps = getNumberOfMaps();
//            chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
//                                                                                                   numMaps));
//        }
//    }
//    
//    if (hasNoMapsLineSeriesFlag) {
//        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(this);
//        CaretAssert(ciftiMapFile);
//        std::vector<int64_t> dims;
//        ciftiMapFile->getMapDimensions(dims);
//        CaretAssertVectorIndex(dims, 1);
//        const int32_t numCols = dims[0];
//        //const int32_t numRows = dims[1];
//        
//        if (numCols > 1) {
//            const NiftiTimeUnitsEnum::Enum mapUnits = getMapIntervalUnits();
//            ChartAxisUnitsEnum::Enum xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
//            switch (mapUnits) {
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                    CaretAssert(0);
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                    xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                    break;
//            }
//            chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
//                                                                                                   numCols));
//        }
//    }
//    
//    if (hasMatrixFlag) {
//        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(this);
//        CaretAssert(ciftiMapFile);
//        std::vector<int64_t> dims;
//        ciftiMapFile->getMapDimensions(dims);
//        CaretAssertVectorIndex(dims, 1);
//        const int32_t numCols = dims[0];
//        const int32_t numRows = dims[1];
//        
//        chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForMatrix(numRows,
//                                                                                           numCols));
//    }
//}
//
///**
// * Get the chart compound data type supported by this file that uses the given
// * chart data type.
// *
// * @param chartDataType
// *     The chart data type.
// * @param chartCompoundDataTypeOut
// *     Output with the chart compound data type.
// * @return
// *     True if there is output chart compound data type is valid.
// *     False if output chart compound data type is invalid OR if chartDataType is invalid.
// */
//bool
//CaretMappableDataFile::getChartCompoundDataTypeForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
//                                                                ChartTwoCompoundDataType& chartCompoundDataTypeOut) const
//{
//    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
//    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
//    for (auto& cdt : chartCompoundDataTypes) {
//        if (cdt.getChartDataType() == chartDataType) {
//            chartCompoundDataTypeOut = cdt;
//            return true;
//        }
//    }
//
//    /* default constructor is invalid data type */
//    chartCompoundDataTypeOut = ChartTwoCompoundDataType();
//    return false;
//}
//
///**
// * @return Charting history if this file charts to lines.
// */
//ChartTwoDataCartesianHistory*
//CaretMappableDataFile::getLineSeriesChartingHistory()
//{
//    return m_lineChartHistory.get();
//}
