
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __IDENTIFICATION_TEXT_GENERATOR_DECLARE__
#include "IdentificationTextGenerator.h"
#undef __IDENTIFICATION_TEXT_GENERATOR_DECLARE__

#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CaretVolumeExtension.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "OverlaySet.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "IdentificationStringBuilder.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Surface.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class IdentificationTextGenerator 
 * \brief Creates text describing selected data.
 *
 * Examine the selected data and generate descriptive text.
 */

/**
 * Constructor.
 */
IdentificationTextGenerator::IdentificationTextGenerator()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
IdentificationTextGenerator::~IdentificationTextGenerator()
{
    
}

/**
 * Create identification text from selection in the identification manager.
 * @param idManager
 *    Identification manager containing selection.
 * @param brain
 *    The brain.
 */
AString 
IdentificationTextGenerator::createIdentificationText(const SelectionManager* idManager,
                                                      const Brain* brain) const
{
    CaretAssert(idManager);
    CaretAssert(brain);
    
    IdentificationStringBuilder idText;
    
    const SelectionItemSurfaceNode* surfaceID = idManager->getSurfaceNodeIdentification();
    
    this->generateSurfaceIdentificationText(idText,
                                            brain,
                                            surfaceID);
    
    this->generateSurfaceBorderIdentifcationText(idText,
                                                 idManager->getSurfaceBorderIdentification());
    
    this->generateSurfaceFociIdentifcationText(idText, 
                                               idManager->getSurfaceFocusIdentification());
    
    this->generateVolumeFociIdentifcationText(idText,
                                              idManager->getVolumeFocusIdentification());
    
    this->generateVolumeIdentificationText(idText,
                                           brain,
                                           idManager->getVoxelIdentification());
    
//    const SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
//    if (voxelID->isValid()) {
//        int64_t ijk[3];
//        const VolumeMappableInterface* idVolumeFile = voxelID->getVolumeFile();
//        voxelID->getVoxelIJK(ijk);
//        float x, y, z;
//        idVolumeFile->indexToSpace(ijk[0], ijk[1], ijk[2], x, y, z);
//                
//        idText.addLine(false,
//                       "Voxel XYZ ("
//                       + AString::number(x)
//                       + ", "
//                       + AString::number(y)
//                       + ", "
//                       + AString::number(z)
//                       + ")");
//        
//        const int32_t numVolumeFiles = brain->getNumberOfVolumeFiles();
//        
//        /*
//         * In first loop, show values for 'idVolumeFile' (the underlay volume)
//         * In second loop, show values for all other volume files
//         */
//        for (int32_t iLoop = 0; iLoop < 2; iLoop++) {
//            for (int32_t i = 0; i < numVolumeFiles; i++) {
//                const VolumeFile* vf = brain->getVolumeFile(i);
//                if (vf == idVolumeFile) {
//                    if (iLoop != 0) {
//                        continue;
//                    }
//                }
//                else if (iLoop == 0) {
//                    continue;
//                }
//                
//                int64_t vfI, vfJ, vfK;
//                vf->enclosingVoxel(x, y, z,
//                                   vfI, vfJ, vfK);
//                
//                if (vf->indexValid(vfI, vfJ, vfK)) {
//                    AString boldText = vf->getFileNameNoPath();
//                    boldText += (" IJK ("
//                                 + AString::number(vfI)
//                                 + ", "
//                                 + AString::number(vfJ)
//                                 + ", "
//                                 + AString::number(vfK)
//                                 + ")  ");
//                    
//                    AString text;
//                    const int32_t numMaps = vf->getNumberOfMaps();
//                    for (int jMap = 0; jMap < numMaps; jMap++) {
//                        if (jMap > 0) {
//                            text += " ";
//                        }
//                        if (vf->getType() == SubvolumeAttributes::LABEL) {
//                            const int32_t labelIndex = static_cast<int32_t>(vf->getValue(vfI, vfJ, vfK, jMap));
//                            const GiftiLabelTable* glt = vf->getMapLabelTable(jMap);
//                            const GiftiLabel* gl = glt->getLabel(labelIndex);
//                            if (gl != NULL) {
//                                text += gl->getName();
//                            }
//                            else {
//                                text += ("LABLE_MISSING_FOR_INDEX=" 
//                                         + AString::number(labelIndex));
//                            }
//                        }
//                        else {
//                            text += AString::number(vf->getValue(vfI, vfJ, vfK, jMap));
//                        }
//                    }
//                    
//                    idText.addLine(true,
//                                   boldText,
//                                   text);
//                }
//            }            
//        }
//        
//        const float xyz[3] = { x, y, z };
//
//        std::vector<CiftiMappableDataFile*> allCiftiMappableDataFiles;
//        brain->getAllCiftiMappableDataFiles(allCiftiMappableDataFiles);
//        for (std::vector<CiftiMappableDataFile*>::iterator ciftiMapIter = allCiftiMappableDataFiles.begin();
//             ciftiMapIter != allCiftiMappableDataFiles.end();
//             ciftiMapIter++) {
//            const CiftiMappableDataFile* cmdf = *ciftiMapIter;
//            if (cmdf->isEmpty() == false) {
//                const int numMaps = cmdf->getNumberOfMaps();
//                std::vector<int32_t> mapIndices;
//                for (int32_t i = 0; i < numMaps; i++) {
//                    mapIndices.push_back(i);
//                }
//
//                /*
//                 * Limit dense scalar and data series to maps selected in the overlay.
//                 */
//                if ((cmdf->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR)
//                    || (cmdf->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES)) {
//                    getMapIndicesOfFileUsedInOverlays(cmdf,
//                                                      mapIndices);
//                }
//                
//                AString textValue;
//                int64_t voxelIJK[3];
//                if (cmdf->getVolumeVoxelIdentificationForMaps(mapIndices,
//                                                              xyz,
//                                                              voxelIJK,
//                                                              textValue)) {
//                    AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
//                                        + " "
//                                        + cmdf->getFileNameNoPath()
//                                        + " IJK ("
//                                        + AString::number(voxelIJK[0])
//                                        + ", "
//                                        + AString::number(voxelIJK[1])
//                                        + ", "
//                                        + AString::number(voxelIJK[2])
//                                        + ")  ");
//                    idText.addLine(true, boldText, textValue);
//                }
//            }
//        }
//        
//    }
    
    return idText.toString();
}

/**
 * Generate identification text for volume voxel identification.
 *
 * @param idText
 *     String builder for identification text.
 * @param brain
 *     The brain.
 * @param idVolumeVoxel
 *     Information for volume voxel ID.
 */
void
IdentificationTextGenerator::generateVolumeIdentificationText(IdentificationStringBuilder& idText,
                                                              const Brain* brain,
                                                              const SelectionItemVoxel* idVolumeVoxel) const
{
    if (idVolumeVoxel->isValid() == false) {
        return;
    }
    
    int64_t ijk[3];
    const VolumeMappableInterface* idVolumeFile = idVolumeVoxel->getVolumeFile();
    idVolumeVoxel->getVoxelIJK(ijk);
    float x, y, z;
    idVolumeFile->indexToSpace(ijk[0], ijk[1], ijk[2], x, y, z);
    
    idText.addLine(false,
                   "Voxel XYZ ("
                   + AString::number(x)
                   + ", "
                   + AString::number(y)
                   + ", "
                   + AString::number(z)
                   + ")");
    
    const float xyz[3] = { x, y, z };

    
    /*
     * Get all volume files
     */
    std::vector<const VolumeMappableInterface*> volumeInterfaces;
    const int32_t numVolumeFiles = brain->getNumberOfVolumeFiles();
    for (int32_t i = 0; i < numVolumeFiles; i++) {
        const VolumeFile* vf = brain->getVolumeFile(i);
        volumeInterfaces.push_back(vf);
    }
    
    /*
     * Get the CIFTI files that are volume mappable
     */
    std::vector<CiftiMappableDataFile*> allCiftiMappableDataFiles;
    brain->getAllCiftiMappableDataFiles(allCiftiMappableDataFiles);
    for (std::vector<CiftiMappableDataFile*>::iterator ciftiMapIter = allCiftiMappableDataFiles.begin();
         ciftiMapIter != allCiftiMappableDataFiles.end();
         ciftiMapIter++) {
        const CiftiMappableDataFile* cmdf = *ciftiMapIter;
        if (cmdf->isEmpty() == false) {
            if (cmdf->isVolumeMappable()) {
                volumeInterfaces.push_back(cmdf);
            }
        }
    }
    
    /*
     * In first loop, show values for 'idVolumeFile' (the underlay volume)
     * In second loop, show values for all other volume files
     */
    const int32_t numberOfVolumeMappableFiles = static_cast<int32_t>(volumeInterfaces.size());
    for (int32_t iLoop = 0; iLoop < 2; iLoop++) {
        for (int32_t i = 0; i < numberOfVolumeMappableFiles; i++) {
            const VolumeMappableInterface* volumeInterfaceFile = volumeInterfaces[i];
            const VolumeFile* volumeFile = dynamic_cast<const VolumeFile*>(volumeInterfaceFile);
            const CiftiMappableDataFile* ciftiFile = dynamic_cast<const CiftiMappableDataFile*>(volumeInterfaceFile);
            CaretAssert((volumeFile != NULL)
                        || (ciftiFile != NULL));
            const CaretMappableDataFile* caretMappableDataFile = dynamic_cast<const CaretMappableDataFile*>(volumeInterfaceFile);
            CaretAssert(caretMappableDataFile != NULL);
            
            if (volumeInterfaceFile == idVolumeFile) {
                if (iLoop != 0) {
                    continue;
                }
            }
            else if (iLoop == 0) {
                continue;
            }
            
            int64_t vfI, vfJ, vfK;
            volumeInterfaceFile->enclosingVoxel(x, y, z,
                               vfI, vfJ, vfK);
            
            if (volumeInterfaceFile->indexValid(vfI, vfJ, vfK)) {
                if (volumeFile != NULL) {
                    AString boldText = caretMappableDataFile->getFileNameNoPath();
                    boldText += (" IJK ("
                                 + AString::number(vfI)
                                 + ", "
                                 + AString::number(vfJ)
                                 + ", "
                                 + AString::number(vfK)
                                 + ")  ");
                    
                    AString text;
                    const int32_t numMaps = caretMappableDataFile->getNumberOfMaps();
                    for (int jMap = 0; jMap < numMaps; jMap++) {
                        if (jMap > 0) {
                            text += " ";
                        }
                        if (volumeFile != NULL) {
                            if (volumeFile->getType() == SubvolumeAttributes::LABEL) {
                                const int32_t labelIndex = static_cast<int32_t>(volumeFile->getValue(vfI, vfJ, vfK, jMap));
                                const GiftiLabelTable* glt = volumeFile->getMapLabelTable(jMap);
                                const GiftiLabel* gl = glt->getLabel(labelIndex);
                                if (gl != NULL) {
                                    text += gl->getName();
                                }
                                else {
                                    text += ("LABLE_MISSING_FOR_INDEX="
                                             + AString::number(labelIndex));
                                }
                            }
                            else {
                                text += AString::number(volumeFile->getValue(vfI, vfJ, vfK, jMap));
                            }
                        }
                        else if (ciftiFile != NULL) {
                            
                        }
                    }
                    
                    idText.addLine(true,
                                   boldText,
                                   text);
                }
                else if (ciftiFile != NULL) {
                    if (ciftiFile->isEmpty() == false) {
                        const int numMaps = ciftiFile->getNumberOfMaps();
                        std::vector<int32_t> mapIndices;
                        for (int32_t i = 0; i < numMaps; i++) {
                            mapIndices.push_back(i);
                        }
                        
                        /*
                         * Limit dense scalar and data series to maps selected in the overlay.
                         */
                        if ((ciftiFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR)
                            || (ciftiFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES)) {
                            getMapIndicesOfFileUsedInOverlays(ciftiFile,
                                                              mapIndices);
                        }
                        
                        AString textValue;
                        int64_t voxelIJK[3];
                        if (ciftiFile->getVolumeVoxelIdentificationForMaps(mapIndices,
                                                                      xyz,
                                                                      voxelIJK,
                                                                      textValue)) {
                            AString boldText = (DataFileTypeEnum::toOverlayTypeName(ciftiFile->getDataFileType())
                                                + " "
                                                + ciftiFile->getFileNameNoPath()
                                                + " IJK ("
                                                + AString::number(voxelIJK[0])
                                                + ", "
                                                + AString::number(voxelIJK[1])
                                                + ", "
                                                + AString::number(voxelIJK[2])
                                                + ")  ");
                            idText.addLine(true, boldText, textValue);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Generate identification text for a surface node identification.
 * @param idText
 *     String builder for identification text.
 * @param brain
 *     The brain.
 * @param browserTabContent
 *     Content of the browser tab.
 * @param idSurfaceNode
 *     Information for surface node ID.
 */
void 
IdentificationTextGenerator::generateSurfaceIdentificationText(IdentificationStringBuilder& idText,
                                                               const Brain* brain,
                                                               const SelectionItemSurfaceNode* idSurfaceNode) const
{
    const Surface* surface = idSurfaceNode->getSurface();
    const int32_t nodeNumber = idSurfaceNode->getNodeNumber();
    
    if ((surface != NULL) 
        && (nodeNumber >= 0)) {
        AString surfaceID;
        surfaceID += ("VERTEX " + StructureEnum::toGuiName(surface->getStructure()));
        idText.addLine(false, surfaceID, nodeNumber, false);
        
        const float* xyz = surface->getCoordinate(nodeNumber);
        
        idText.addLine(true, SurfaceTypeEnum::toGuiName(surface->getSurfaceType()).toUpper()
                       + " XYZ: "
                       + AString::number(xyz[0])
                       + ", "
                       + AString::number(xyz[1])
                       + ", "
                       + AString::number(xyz[2]));
        
        const BrainStructure* brainStructure = surface->getBrainStructure();
        CaretAssert(brainStructure);
        
//        std::vector<CiftiMappableDataFile*> allCiftiMappableDataFiles;
//        brain->getAllCiftiMappableDataFiles(allCiftiMappableDataFiles);
//        for (std::vector<CiftiMappableDataFile*>::iterator ciftiMapIter = allCiftiMappableDataFiles.begin();
//             ciftiMapIter != allCiftiMappableDataFiles.end();
//             ciftiMapIter++) {
//            const CiftiMappableDataFile* cmdf = *ciftiMapIter;
//            if (cmdf->isEmpty() == false) {
//                const int numMaps = cmdf->getNumberOfMaps();
//                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
//                    AString textValue;
//                    if (cmdf->getMapSurfaceNodeValue(iMap, surface->getStructure(), nodeNumber, surface->getNumberOfNodes(), textValue)) {
//                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
//                                            + " "
//                                            + cmdf->getFileNameNoPath());
//                        idText.addLine(true, boldText, textValue);
//                    }
//                }
//            }
//        }

        std::vector<CiftiMappableDataFile*> allCiftiMappableDataFiles;
        brain->getAllCiftiMappableDataFiles(allCiftiMappableDataFiles);
        for (std::vector<CiftiMappableDataFile*>::iterator ciftiMapIter = allCiftiMappableDataFiles.begin();
             ciftiMapIter != allCiftiMappableDataFiles.end();
             ciftiMapIter++) {
            const CiftiMappableDataFile* cmdf = *ciftiMapIter;
            AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
                                + " "
                                + cmdf->getFileNameNoPath());
            
            std::vector<int32_t> mapIndices;
            for (int32_t i = 0; i < cmdf->getNumberOfMaps(); i++) {
                mapIndices.push_back(i);
            }
            
            /*
             * Limit dense scalar and data series to maps selected in the overlays
             * from all tabs.
             */
            if ((cmdf->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR)
                || (cmdf->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES)) {
                getMapIndicesOfFileUsedInOverlays(cmdf,
                                                  mapIndices);
            }
            AString textValue;
            
            const bool valid = cmdf->getSurfaceNodeIdentificationForMaps(mapIndices,
                                                                            surface->getStructure(),
                                                                            nodeNumber,
                                                                            surface->getNumberOfNodes(),
                                                                            textValue);
            if (valid) {
                idText.addLine(true,
                               boldText,
                               textValue);
            }
            
//            const CiftiMappableConnectivityMatrixDataFile* connCifti = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(cmdf);
//            if (cmdf->isEmpty() == false) {
//                const int numMaps = cmdf->getNumberOfMaps();
//                if (numMaps > 0) {
//                    if (connCifti != NULL) {
//                        AString textValue;
//                        const int32_t mapIndex = 0;
//                        if (cmdf->getMapSurfaceNodeValue(mapIndex, surface->getStructure(), nodeNumber, surface->getNumberOfNodes(), textValue)) {
//                            AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
//                                                + " "
//                                                + cmdf->getFileNameNoPath());
//                            idText.addLine(true, boldText, textValue);
//                        }
//                    }
//                    else {
//                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
//                                            + " "
//                                            + cmdf->getFileNameNoPath());
//                        std::vector<float> nodeData;
//                        if (cmdf->getSeriesDataForSurfaceNode(surface->getStructure(),
//                                                              nodeNumber,
//                                                              nodeData)) {
//                            for (int32_t iMap = 0; iMap < numMaps; iMap++) {
//                                AString textValue = AString::number(nodeData[iMap]);
//                                idText.addLine(true, boldText, textValue);
//                            }
//                        }
//                    }
//                }
//            }
        }
        
        
        const int32_t numLabelFiles = brainStructure->getNumberOfLabelFiles();
        for (int32_t i = 0; i < numLabelFiles; i++) {
            const LabelFile* lf = brainStructure->getLabelFile(i);
            AString boldText = "LABEL " + lf->getFileNameNoPath() + ":";
            AString text;
            const int numMaps = lf->getNumberOfMaps();
            for (int32_t j = 0; j < numMaps; j++) {
                AString labelName = lf->getLabelName(nodeNumber, j);
                if (labelName.isEmpty()) {
                    labelName = ("Map-" + AString::number(j + 1));
                }
                text += (" " + labelName);
            }
            idText.addLine(true, boldText, text);
        }
        
        const int32_t numMetricFiles = brainStructure->getNumberOfMetricFiles();
        for (int32_t i = 0; i < numMetricFiles; i++) {
            const MetricFile* mf = brainStructure->getMetricFile(i);
            AString boldText = "METRIC " + mf->getFileNameNoPath() + ":";
            AString text;
            const int numMaps = mf->getNumberOfMaps();
            for (int32_t j = 0; j < numMaps; j++) {
                text += (" " + AString::number(mf->getValue(nodeNumber, j)));
            }
            idText.addLine(true, boldText, text);
        }
    }
    
}

/**
 * Find the usage of the file's maps in all overlays.
 *
 * @param caretMappableDataFile
 *    The file whose usage is desired.
 * @param mapIndicesOut
 *    Indices of maps of the file that are used in overlays.
 */
void
IdentificationTextGenerator::getMapIndicesOfFileUsedInOverlays(const CaretMappableDataFile* caretMappableDataFile,
                                                               std::vector<int32_t>& mapIndicesOut) const
{
    mapIndicesOut.clear();
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    const std::vector<BrowserTabContent*> allTabs = allTabsEvent.getAllBrowserTabs();
    for (std::vector<BrowserTabContent*>::const_iterator tabIter = allTabs.begin();
         tabIter != allTabs.end();
         tabIter++) {
        std::vector<int32_t> mapIndices;
        (*tabIter)->getOverlaySet()->getSelectedMapIndicesForFile(caretMappableDataFile,
                                                                  false,
                                                                  mapIndices);
        mapIndicesOut.insert(mapIndicesOut.end(),
                             mapIndices.begin(),
                             mapIndices.end());
    }
    
    /*
     * Sort and remove all duplicates
     */
    if (mapIndicesOut.empty() == false) {
        std::sort(mapIndicesOut.begin(),
                  mapIndicesOut.end());
        std::vector<int32_t>::iterator uniqueIter = std::unique(mapIndicesOut.begin(),
                                                                mapIndicesOut.end());
        mapIndicesOut.resize(std::distance(mapIndicesOut.begin(),
                                        uniqueIter));
    }
}

/**
 * Generate identification text for a surface border identification.
 * @param idText
 *     String builder for identification text.
 * @param idSurfaceBorder
 *     Information for surface border ID.
 */
void
IdentificationTextGenerator::generateSurfaceBorderIdentifcationText(IdentificationStringBuilder& idText,
                                                                    const SelectionItemBorderSurface* idSurfaceBorder) const
{
    if (idSurfaceBorder->isValid()) {
        const Border* border = idSurfaceBorder->getBorder();
        const SurfaceProjectedItem* spi = border->getPoint(idSurfaceBorder->getBorderPointIndex());
        float xyz[3];
        spi->getProjectedPosition(*idSurfaceBorder->getSurface(), xyz, false);

        AString boldText = ("BORDER " 
                            + StructureEnum::toGuiName(spi->getStructure())
                            + " Name: "
                            + border->getName());
        if (border->getClassName().isEmpty() == false) {
            boldText += (" ClassName: "
                         + border->getClassName()
                         + ": ");
        }
        const AString text = ("("
                              + AString::number(idSurfaceBorder->getBorderIndex())
                              + ","
                              + AString::number(idSurfaceBorder->getBorderPointIndex())
                              + ") ("
                              + AString::fromNumbers(xyz, 3, ",")
                              + ")");
        idText.addLine(true, boldText, text);
    }
}

/**
 * Generate identification text for a surface focus identification.
 * @param idText
 *     String builder for identification text.
 * @param idSurfaceFocus
 *     Information for surface focus ID.
 */void 
IdentificationTextGenerator::generateSurfaceFociIdentifcationText(IdentificationStringBuilder& idText,
                                          const SelectionItemFocusSurface* idSurfaceFocus) const
{
    if (idSurfaceFocus->isValid()) {
        const Focus* focus = idSurfaceFocus->getFocus();
        idText.addLine(false,
                       "FOCUS", 
                       focus->getName());

        idText.addLine(true,
                       "Index",
                       AString::number(idSurfaceFocus->getFocusIndex()));
        
        const int32_t projectionIndex = idSurfaceFocus->getFocusProjectionIndex();
        const SurfaceProjectedItem* spi = focus->getProjection(projectionIndex);
        float xyzProj[3];
        spi->getProjectedPosition(*idSurfaceFocus->getSurface(), xyzProj, false);
        float xyzStereo[3];
        spi->getStereotaxicXYZ(xyzStereo);
        
        idText.addLine(true,
                       "Structure",
                       StructureEnum::toGuiName(spi->getStructure()));

        if (spi->isStereotaxicXYZValid()) {
            idText.addLine(true,
                           "XYZ (Stereotaxic)",
                           xyzStereo,
                           3,
                           true);
        }
        else {
            idText.addLine(true,
                           "XYZ (Stereotaxic)",
                           "Invalid");
        }
        
        bool projValid = false;
        AString xyzProjName = "XYZ (Projected)";
        if (spi->getBarycentricProjection()->isValid()) {
            xyzProjName = "XYZ (Projected to Triangle)";
            projValid = true;
        }
        else if (spi->getVanEssenProjection()->isValid()) {
            xyzProjName = "XYZ (Projected to Edge)";
            projValid = true;
        }
        if (projValid) {
            idText.addLine(true,
                           xyzProjName,
                           xyzProj,
                           3,
                           true);
        }
        else {
            idText.addLine(true,
                           xyzProjName,
                           "Invalid");
        }
        
        const int32_t numberOfProjections = focus->getNumberOfProjections();
        for (int32_t i = 0; i < numberOfProjections; i++) {
            if (i != projectionIndex) {
                const SurfaceProjectedItem* proj = focus->getProjection(i);
                AString projTypeName = "";
                if (proj->getBarycentricProjection()->isValid()) {
                    projTypeName = "Triangle";
                    
                }
                else if (proj->getVanEssenProjection()->isValid()) {
                    projTypeName = "Edge";
                }
                if (projTypeName.isEmpty() == false) {
                    const AString txt = (StructureEnum::toGuiName(proj->getStructure())
                                         + " ("
                                         + projTypeName
                                         + ")");
                                         
                    idText.addLine(true,
                                   "Ambiguous Projection",
                                   txt);
                }
            }
        }
        
        idText.addLine(true,
                       "Area",
                       focus->getArea());
        
        idText.addLine(true,
                       "Class Name",
                       focus->getClassName());

        idText.addLine(true,
                       "Comment",
                       focus->getComment());
        
        idText.addLine(true,
                       "Extent",
                       focus->getExtent(),
                       true);
        
        idText.addLine(true,
                       "Geography",
                       focus->getGeography());
        
        idText.addLine(true,
                       "Region of Interest",
                       focus->getRegionOfInterest());
        
        idText.addLine(true,
                       "Statistic",
                       focus->getStatistic());
        
    }
}

/**
 * Generate identification text for a volume focus identification.
 * @param idText
 *     String builder for identification text.
 * @param idVolumeFocus
 *     Information for surface focus ID.
 */void
IdentificationTextGenerator::generateVolumeFociIdentifcationText(IdentificationStringBuilder& idText,
                                                                  const SelectionItemFocusVolume* idVolumeFocus) const
{
    if (idVolumeFocus->isValid()) {
        const Focus* focus = idVolumeFocus->getFocus();
        const SurfaceProjectedItem* spi = focus->getProjection(idVolumeFocus->getFocusProjectionIndex());
        float xyzVolume[3];
        spi->getVolumeXYZ(xyzVolume);
        float xyzStereo[3];
        spi->getStereotaxicXYZ(xyzStereo);
        
        idText.addLine(false,
                       "FOCUS",
                       focus->getName());
        
        idText.addLine(true,
                       "Index",
                       AString::number(idVolumeFocus->getFocusIndex()));
        
        idText.addLine(true,
                       "Structure",
                       StructureEnum::toGuiName(spi->getStructure()));
        
        if (spi->isStereotaxicXYZValid()) {
            idText.addLine(true,
                           "XYZ (Stereotaxic)",
                           xyzStereo,
                           3,
                           true);
        }
        else {
            idText.addLine(true,
                           "XYZ (Stereotaxic)",
                           "Invalid");
        }
        
        AString xyzVolumeName = "XYZ (Volume)";
        idText.addLine(true,
                       xyzVolumeName,
                       xyzVolume,
                       3,
                       true);
        
        idText.addLine(true,
                       "Area",
                       focus->getArea());
        
        idText.addLine(true,
                       "Class Name",
                       focus->getClassName());
        
        idText.addLine(true,
                       "Comment",
                       focus->getComment());
        
        idText.addLine(true,
                       "Extent",
                       focus->getExtent(),
                       true);
        
        idText.addLine(true,
                       "Geography",
                       focus->getGeography());
        
        idText.addLine(true,
                       "Region of Interest",
                       focus->getRegionOfInterest());
        
        idText.addLine(true,
                       "Statistic",
                       focus->getStatistic());
        
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationTextGenerator::toString() const
{
    return "IdentificationTextGenerator";
}
