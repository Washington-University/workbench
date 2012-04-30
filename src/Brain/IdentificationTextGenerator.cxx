
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
#include "ConnectivityLoaderFile.h"
#include "CaretVolumeExtension.h"
#include "EventManager.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "IdentificationItemBorderSurface.h"
#include "IdentificationItemFocusSurface.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationManager.h"
#include "IdentificationStringBuilder.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Surface.h"
#include "SurfaceProjectedItem.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class IdentificationTextGenerator 
 * \brief Creates text describing identified data.
 *
 * Examine the identified data and generate descriptive text.
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
IdentificationTextGenerator::createIdentificationText(const IdentificationManager* idManager,
                                                      const BrowserTabContent* /*browserTabConent*/,
                                                      const Brain* brain) const
{
    IdentificationStringBuilder idText;
    
    const IdentificationItemSurfaceNode* surfaceID = idManager->getSurfaceNodeIdentification();
    
    this->generateSurfaceIdentificationText(idText,
                                            brain,
                                            surfaceID);
    
    const int32_t numAdditionalSurfaceIdentifications = idManager->getNumberOfAdditionalSurfaceNodeIdentifications();
    for (int32_t i = 0; i < numAdditionalSurfaceIdentifications; i++) {
        /*
         * Ignore additional identification if surface from same structure
         */
        if (surfaceID->isValid()) {
            if (surfaceID->isContralateral() == false) {
                if (surfaceID->getSurface()->getStructure() ==
                    idManager->getAdditionalSurfaceNodeIdentification(i)->getSurface()->getStructure()) {
                    continue;
                }
            }
        }
        this->generateSurfaceIdentificationText(idText, 
                                                brain, 
                                                idManager->getAdditionalSurfaceNodeIdentification(i));
    }
    
    this->generateSurfaceBorderIdentifcationText(idText,
                                                 idManager->getSurfaceBorderIdentification());
    
    this->generateSurfaceFociIdentifcationText(idText, 
                                               idManager->getSurfaceFocusIdentification());
    
    const IdentificationItemVoxel* voxelID = idManager->getVoxelIdentification();
    if (voxelID->isValid()) {
        int64_t ijk[3];
        const VolumeFile* idVolumeFile = voxelID->getVolumeFile();
        voxelID->getVoxelIJK(ijk);
        float xyz[3];
        idVolumeFile->indexToSpace(ijk, xyz);
                
        idText.addLine(false,
                       "Voxel XYZ ("
                       + AString::number(xyz[0])
                       + ", "
                       + AString::number(xyz[1])
                       + ", "
                       + AString::number(xyz[2])
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
                
                int64_t vfIJK[3];
                vf->enclosingVoxel(xyz, 
                                   vfIJK);
                
                if (vf->indexValid(vfIJK[0], vfIJK[1], vfIJK[2])) {
                    AString boldText = vf->getFileNameNoPath();
                    boldText += (" IJK ("
                                 + AString::number(vfIJK[0])
                                 + ", "
                                 + AString::number(vfIJK[1])
                                 + ", "
                                 + AString::number(vfIJK[2])
                                 + ")  ");
                    
                    AString text;
                    const int32_t numMaps = vf->getNumberOfMaps();
                    for (int jMap = 0; jMap < numMaps; jMap++) {
                        if (jMap > 0) {
                            text += " ";
                        }
                        if (vf->getType() == SubvolumeAttributes::LABEL) {
                            const int32_t labelIndex = static_cast<int32_t>(vf->getValue(vfIJK, jMap));
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
                            text += AString::number(vf->getValue(vfIJK, jMap));
                        }
                    }
                    
                    idText.addLine(true,
                                   boldText,
                                   text);
                }
            }            
        }
        
        std::vector<ConnectivityLoaderFile*> allConnectivityFiles;
        brain->getConnectivityFilesOfAllTypes(allConnectivityFiles);
        
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
                                                               const IdentificationItemSurfaceNode* idSurfaceNode) const
{
    const Surface* surface = idSurfaceNode->getSurface();
    const int32_t nodeNumber = idSurfaceNode->getNodeNumber();
    
    if ((surface != NULL) 
        && (nodeNumber >= 0)) {
        AString surfaceID;
        if (idSurfaceNode->isContralateral()) {
            surfaceID += "CONTRALATERAL ";
        }
        surfaceID += ("NODE " + StructureEnum::toGuiName(surface->getStructure()));
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
        brain->getConnectivityFilesOfAllTypes(allConnectivityFiles);
        
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
                    AString boldText = clf->getCiftiTypeName().toUpper() + " "  + clf->getFileNameNoPath() + ":";
                    AString text = AString::number(value);
                    idText.addLine(true, boldText, text);
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
                                                                    const IdentificationItemBorderSurface* idSurfaceBorder) const
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
                                          const IdentificationItemFocusSurface* idSurfaceFocus) const
{
    if (idSurfaceFocus->isValid()) {
        const Focus* focus = idSurfaceFocus->getFocus();
        const SurfaceProjectedItem* spi = focus->getProjection(idSurfaceFocus->getFocusProjectionIndex());
        float xyz[3];
        spi->getProjectedPosition(*idSurfaceFocus->getSurface(), xyz, false);
        
        idText.addLine(false, 
                       "FOCUS", 
                       focus->getName());

        idText.addLine(true,
                       "Structure",
                       StructureEnum::toGuiName(spi->getStructure()));

        idText.addLine(true,
                       "XYZ",
                       xyz,
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
                       focus->getArea());
        
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
