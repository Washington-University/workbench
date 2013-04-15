
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
#include "CaretAssert.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDataFile.h"
#include "ConnectivityLoaderFile.h"
#include "CaretVolumeExtension.h"
#include "EventManager.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
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
 * @param browserTabContent
 * @param brain
 */
AString 
IdentificationTextGenerator::createIdentificationText(const SelectionManager* idManager,
                                                      const BrowserTabContent* /*browserTabConent*/,
                                                      const Brain* brain) const
{
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
    
    const SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
    if (voxelID->isValid()) {
        int64_t ijk[3];
        const VolumeMappableInterface* idVolumeFile = voxelID->getVolumeFile();
        voxelID->getVoxelIJK(ijk);
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
        
        const int32_t numVolumeFiles = brain->getNumberOfVolumeFiles();
        
        /*
         * In first loop, show values for 'idVolumeFile' (the underlay volume)
         * In second loop, show values for all other volume files
         */
        for (int32_t iLoop = 0; iLoop < 2; iLoop++) {
            for (int32_t i = 0; i < numVolumeFiles; i++) {
                const VolumeFile* vf = brain->getVolumeFile(i);
                if (vf == idVolumeFile) {
                    if (iLoop != 0) {
                        continue;
                    }
                }
                else if (iLoop == 0) {
                    continue;
                }
                
                int64_t vfI, vfJ, vfK;
                vf->enclosingVoxel(x, y, z,
                                   vfI, vfJ, vfK);
                
                if (vf->indexValid(vfI, vfJ, vfK)) {
                    AString boldText = vf->getFileNameNoPath();
                    boldText += (" IJK ("
                                 + AString::number(vfI)
                                 + ", "
                                 + AString::number(vfJ)
                                 + ", "
                                 + AString::number(vfK)
                                 + ")  ");
                    
                    AString text;
                    const int32_t numMaps = vf->getNumberOfMaps();
                    for (int jMap = 0; jMap < numMaps; jMap++) {
                        if (jMap > 0) {
                            text += " ";
                        }
                        if (vf->getType() == SubvolumeAttributes::LABEL) {
                            const int32_t labelIndex = static_cast<int32_t>(vf->getValue(vfI, vfJ, vfK, jMap));
                            const GiftiLabelTable* glt = vf->getMapLabelTable(jMap);
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
                            text += AString::number(vf->getValue(vfI, vfJ, vfK, jMap));
                        }
                    }
                    
                    idText.addLine(true,
                                   boldText,
                                   text);
                }
            }            
        }
        
        std::vector<ConnectivityLoaderFile*> allConnectivityFiles;
        brain->getMappableConnectivityFilesOfAllTypes(allConnectivityFiles);
        
        const float xyz[3] = { x, y, z };
        for (std::vector<ConnectivityLoaderFile*>::iterator connIter = allConnectivityFiles.begin();
             connIter != allConnectivityFiles.end();
             connIter++) {
            const ConnectivityLoaderFile* clf = *connIter;
            if (clf->isEmpty() == false) {
                float value = 0.0;
                int64_t connIJK[3];
                if (clf->getVolumeVoxelValue(xyz, connIJK, value)) {
                    AString boldText = (clf->getCiftiTypeName().toUpper() 
                                        + " "  
                                        + clf->getFileNameNoPath());
                    boldText += (" IJK ("
                                 + AString::number(connIJK[0])
                                 + ", "
                                 + AString::number(connIJK[1])
                                 + ", "
                                 + AString::number(connIJK[2])
                                 + ")  ");
                    AString text = AString::number(value);
                    idText.addLine(true, boldText, text);
                }
            }
        }

        std::vector<CiftiConnectivityMatrixDataFile*> allConnMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(allConnMatrixFiles);
        for (std::vector<CiftiConnectivityMatrixDataFile*>::iterator connMatIter = allConnMatrixFiles.begin();
             connMatIter != allConnMatrixFiles.end();
             connMatIter++) {
            const CiftiConnectivityMatrixDataFile* cmf = *connMatIter;
            if (cmf->isEmpty() == false) {
                const int numMaps = cmf->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    float value = 0.0;
                    int64_t cmfIJK[3];
                    AString textValue;
                    if (cmf->getMapVolumeVoxelValue(iMap,
                                                    xyz,
                                                    cmfIJK,
                                                    value,
                                                    textValue)) {
                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmf->getDataFileType())
                                            + " "
                                            + cmf->getFileNameNoPath());
                        idText.addLine(true, boldText, textValue);
                    }
                }
            }
        }
        
        std::vector<CiftiBrainordinateFile*> allCiftiBrainordinatesFiles;
        brain->getAllCiftiBrainordinateFiles(allCiftiBrainordinatesFiles);
        for (std::vector<CiftiBrainordinateFile*>::iterator connMatIter = allCiftiBrainordinatesFiles.begin();
             connMatIter != allCiftiBrainordinatesFiles.end();
             connMatIter++) {
            const CiftiBrainordinateFile* cbf = *connMatIter;
            if (cbf->isEmpty() == false) {
                const int numMaps = cbf->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    AString textValue;
                    int64_t voxelIJK[3];
                    if (cbf->getMapVolumeVoxelValue(iMap,
                                                    xyz,
                                                    voxelIJK,
                                                    textValue)) {
                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cbf->getDataFileType())
                                            + " "
                                            + cbf->getFileNameNoPath()
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
    
    return idText.toString();
}

/**
 * Generate identification text for a surface node identification.
 * @param idText
 *     String builder for identification text.
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
        
        
        std::vector<ConnectivityLoaderFile*> allConnectivityFiles;
        brain->getMappableConnectivityFilesOfAllTypes(allConnectivityFiles);
        
        for (std::vector<ConnectivityLoaderFile*>::iterator connIter = allConnectivityFiles.begin();
             connIter != allConnectivityFiles.end();
             connIter++) {
            const ConnectivityLoaderFile* clf = *connIter;
            if (clf->isEmpty() == false) {
                float value = 0.0;
                if (clf->getSurfaceNodeValue(surface->getStructure(),
                                             nodeNumber,
                                             surface->getNumberOfNodes(),
                                             value)) {
                    AString boldText = (clf->getCiftiTypeName().toUpper()
                                        + " "
                                        + clf->getFileNameNoPath());
                    AString text = AString::number(value);
                    idText.addLine(true, boldText, text);
                }
            }
        }
        
        std::vector<CiftiConnectivityMatrixDataFile*> allConnMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(allConnMatrixFiles);
        for (std::vector<CiftiConnectivityMatrixDataFile*>::iterator connMatIter = allConnMatrixFiles.begin();
             connMatIter != allConnMatrixFiles.end();
             connMatIter++) {
            const CiftiConnectivityMatrixDataFile* cmf = *connMatIter;
            if (cmf->isEmpty() == false) {
                const int numMaps = cmf->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    float value = 0.0;
                    AString textValue;
                    if (cmf->getMapSurfaceNodeValue(iMap,
                                                    surface->getStructure(),
                                                    nodeNumber,
                                                    surface->getNumberOfNodes(),
                                                    value,
                                                    textValue)) {
                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmf->getDataFileType())
                                            + " "
                                            + cmf->getFileNameNoPath());
                        idText.addLine(true, boldText, textValue);
                    }
                }
            }
        }
        
        std::vector<CiftiBrainordinateFile*> allCiftiBrainordinatesFiles;
        brain->getAllCiftiBrainordinateFiles(allCiftiBrainordinatesFiles);
        for (std::vector<CiftiBrainordinateFile*>::iterator connMatIter = allCiftiBrainordinatesFiles.begin();
             connMatIter != allCiftiBrainordinatesFiles.end();
             connMatIter++) {
            const CiftiBrainordinateFile* cbf = *connMatIter;
            if (cbf->isEmpty() == false) {
                const int numMaps = cbf->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    AString textValue;
                    if (cbf->getMapSurfaceNodeValue(iMap, surface->getStructure(), nodeNumber, surface->getNumberOfNodes(), textValue)) {
                        AString boldText = (DataFileTypeEnum::toOverlayTypeName(cbf->getDataFileType())
                                            + " "
                                            + cbf->getFileNameNoPath());
                        idText.addLine(true, boldText, textValue);
                    }
                }
            }
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
