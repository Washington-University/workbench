
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "AlgorithmAnnotationResample.h"
#include "AlgorithmException.h"
#include "Annotation.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationGroup.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "DataFileException.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::AlgorithmAnnotationResample 
 * \brief RESAMPLE AN ANNOTATION FILE TO DIFFERENT MESHES
 */

/**
 * @return Command line switch
 */
AString
AlgorithmAnnotationResample::getCommandSwitch()
{
    return "-annotation-resample";
}

/**
 * @return Short description of algorithm
 */
AString
AlgorithmAnnotationResample::getShortDescription()
{
    return "RESAMPLE AN ANNOTATION FILE TO DIFFERENT MESHES";
}

/**
 * @return Parameters for algorithm
 */
OperationParameters*
AlgorithmAnnotationResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addAnnotationParameter(1, "annotation-in", "the annotation file to resample");
    
    /*
     * We need to preserve 'annotation groups' that may be created by the user.
     * By modifying the annotation file that was read, any user created
     * groups are preserved.  In addition, we are only modifying annotations
     * in 'surface space'; all others are preserved without modification.
     * So, we get a string for the output filename instead of an output annotation file
     */
    ret->addStringParameter(2, "annotation-out", "name of resampled annotation file");
    
    ParameterComponent* surfacePairOpt = ret->createRepeatableParameter(3,
                                                                        "-surface-pair",
                                                                        "pair of surfaces for resampling surface annotations for one structure");
    surfacePairOpt->addSurfaceParameter(1, "source-surface", "the midthickness surface of the current mesh the annotations use");
    surfacePairOpt->addSurfaceParameter(2, "target-surface", "the midthickness surface of the mesh the annotations should be transferred to");
    
    
    AString helpText = ("Resample an annotation file from the source mesh to the target mesh.\n\n"
                        "Only annotations in surface space are modified, no changes are made to "
                        "annotations in other spaces.  "
                        "The -surface-pair option may be repeated for additional "
                        "structures used by surface space annotations."
                        "\n\n"
                        "Note: -source-surface and -target-surface options are deprecated "
                        "and will be removed.");

    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform algorithm
 * @param myParams
 *     Parameters for algorithm
 * @param myProgObj
 *     The progress object
 * @throws
 *     AlgorithmException if errors
 */
void
AlgorithmAnnotationResample::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    AnnotationFile* annotIn = myParams->getAnnotation(1);
    
    AString outputFileName = myParams->getString(2);
    
    std::vector<const SurfaceFile*> sourceSurfaces;
    std::vector<const SurfaceFile*> targetSurfaces;
    for (auto instance : *(myParams->getRepeatableParameterInstances(3))) {
        sourceSurfaces.push_back(instance->getSurface(1));
        targetSurfaces.push_back(instance->getSurface(2));
    }
    
    /*
     * Constructs and executes the algorithm 
     */
    AlgorithmAnnotationResample(myProgObj,
                                annotIn,
                                outputFileName,
                                sourceSurfaces,
                                targetSurfaces);
    
}

/**
 * Constructor
 *
 * Calling the constructor will execute the algorithm
 *
 * @param myProgObj
 *     Parameters for algorithm
 */
AlgorithmAnnotationResample::AlgorithmAnnotationResample(ProgressObject* myProgObj,
                                                         AnnotationFile* annotationFile,
                                                         const AString& annotationFileName,
                                                         const std::vector<const SurfaceFile*>& sourceSurfaces,
                                                         const std::vector<const SurfaceFile*>& targetSurfaces)
   : AbstractAlgorithm(myProgObj)
{
    /*
     * Sets the algorithm up to use the progress object, and will 
     * finish the progress object automatically when the algorithm terminates
     */
    LevelProgress myProgress(myProgObj);

    if (annotationFileName.isEmpty()) {
        throw AlgorithmException("Output annotation file name is empty.");
    }

    setupSurfaces(sourceSurfaces,
                  targetSurfaces);
    

    std::vector<Annotation*> allAnnotations;
    annotationFile->getAllAnnotations(allAnnotations);
    
    for (auto ann : allAnnotations) {
        CaretAssert(ann);
        if (ann->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE) {
            resampleAnnotation(ann);
        }
    }
    
    try {
        const AString outputFileName(DataFileTypeEnum::addFileExtensionIfMissing(annotationFileName,
                                                                                 DataFileTypeEnum::ANNOTATION));
        annotationFile->writeFile(outputFileName);
    }
    catch (const DataFileException& dfe) {
        throw AlgorithmException(dfe);
    }
}

/**
 * Resample the given annotation
 *
 * @param ann
 *     The annotation
 */
void
AlgorithmAnnotationResample::resampleAnnotation(Annotation* ann)
{
    CaretAssert(ann);
    CaretAssert(ann->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE);
    
    AnnotationOneDimensionalShape* oneDimAnn = ann->castToOneDimensionalShape();
    AnnotationTwoDimensionalShape* twoDimAnn = ann->castToTwoDimensionalShape();
    
    std::vector<AnnotationCoordinate*> coordinates;
    if (oneDimAnn != NULL) {
        coordinates.push_back(oneDimAnn->getStartCoordinate());
        coordinates.push_back(oneDimAnn->getEndCoordinate());
    }
    else if (twoDimAnn != NULL) {
        coordinates.push_back(twoDimAnn->getCoordinate());
    }
    else {
        const AString msg("Annotation is neither one- nor two-dimensional "
                          + ann->toString());
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    for (auto coord : coordinates) {
        StructureEnum::Enum structure = StructureEnum::INVALID;
        int32_t numberOfVertices(-1);
        int32_t vertexIndex(-1);
        coord->getSurfaceSpace(structure, numberOfVertices, vertexIndex);
        
        if (numberOfVertices < 0) {
            throw AlgorithmException("Annotation in surface space has invalid number of vertices="
                                     + AString::number(numberOfVertices)
                                     + " for "
                                     + ann->toString());
        }
        if (vertexIndex < 0) {
            throw AlgorithmException("Annotation in surface space has invalid vertex index="
                                     + AString::number(vertexIndex)
                                     + " for "
                                     + ann->toString());
        }
        if (vertexIndex > numberOfVertices) {
            throw AlgorithmException("Annotation has invalid vertex index="
                                     + AString::number(vertexIndex)
                                     + " but number of vertices="
                                     + AString::number(numberOfVertices)
                                     + " for "
                                     + ann->toString());
        }
        
        const auto stsIter = m_surfaces.find(structure);
        if (stsIter != m_surfaces.end()) {
            SourceTargetSurface* sts = stsIter->second;
            CaretAssert(sts);
            CaretAssert(sts->m_source);
            CaretAssert(sts->m_target);
            
            if (numberOfVertices != sts->m_source->getNumberOfNodes()) {
                throw AlgorithmException("Source surface with structure "
                                         + StructureEnum::toName(structure)
                                         + " contains "
                                         + AString::number(sts->m_source->getNumberOfNodes())
                                         + " vertices but annotation requires surface with "
                                         + AString::number(numberOfVertices)
                                         + " for "
                                         + ann->toString());
            }
            float xyz[3];
            sts->m_source->getCoordinate(vertexIndex,
                                         xyz);
            
            const int32_t targetVertexIndex = sts->m_target->closestNode(xyz);
            if (targetVertexIndex >= 0) {
                coord->setSurfaceSpace(structure,
                                       sts->m_target->getNumberOfNodes(),
                                       targetVertexIndex);
            }
            else {
                throw AlgorithmException("Unable to find closest vertex in target surface for "
                                         + ann->toString());
            }
        }
        else {
            throw AlgorithmException("There are no surfaces for annotation with structure "
                                     + StructureEnum::toName(structure)
                                     + " for "
                                     + ann->toString());
        }
    }
}

/**
 * Setup the surfaces and validate them
 *
 * @param sourceSurfaces
 *     The source surfaces
 * @param targetSurfaces
 *     The target surfaces
 */
void
AlgorithmAnnotationResample::setupSurfaces(const std::vector<const SurfaceFile*>& sourceSurfaces,
                                           const std::vector<const SurfaceFile*>& targetSurfaces)
{
    if (sourceSurfaces.empty()) {
        throw AlgorithmException("There are no source surfaces");
    }
    if (targetSurfaces.empty()) {
        throw AlgorithmException("There are no target surfaces");
    }
    
    /*
     * Verify each source surface uses a unique structure
     */
    for (const auto surf : sourceSurfaces) {
        const StructureEnum::Enum structure = surf->getStructure();
        const auto iter = m_surfaces.find(structure);
        if (iter == m_surfaces.end()) {
            m_surfaces.insert(std::make_pair(structure,
                                             new SourceTargetSurface(surf)));
        }
        else {
            throw AlgorithmException("Source surfaces have same structure: "
                                     + iter->second->m_source->getFileName()
                                     + " and "
                                     + surf->getFileName());
        }
    }
    
    /*
     * Verify target surfaces use unique structure and
     * there is a source surface using the same structure
     */
    for (const auto surf : targetSurfaces) {
        const StructureEnum::Enum structure = surf->getStructure();
        auto iter = m_surfaces.find(structure);
        if (iter == m_surfaces.end()) {
            throw AlgorithmException("Target surface "
                                     + surf->getFileName()
                                     + " uses structure "
                                     + StructureEnum::toName(structure)
                                     + " but there is no source surface with the structure.");
        }
        else {
            SourceTargetSurface* sts = iter->second;
            CaretAssert(sts);
            if (sts->m_target == NULL) {
                sts->m_target = surf;
            }
            else {
                throw AlgorithmException("Target surfaces have same structure: "
                                         + sts->m_target->getFileName()
                                         + " and "
                                         + surf->getFileName());
            }
        }
    }
    
    /*
     * Verify no missing target surfaces
     */
    for (auto iter : m_surfaces) {
        SourceTargetSurface* sts = iter.second;
        CaretAssert(sts);
        CaretAssert(sts->m_source);
        if (sts->m_target == NULL) {
            throw AlgorithmException("There is no target surface with structure "
                                     + StructureEnum::toName(iter.first)
                                     + " for source surface "
                                     + sts->m_source->getFileName());
        }
    }
    
    for (auto iter : m_surfaces) {
        CaretAssert(iter.second);
        CaretAssert(iter.second->m_source);
        CaretAssert(iter.second->m_target);
    }
}

/**
 * @return Algorithm internal weight
 */
float
AlgorithmAnnotationResample::getAlgorithmInternalWeight()
{
    /*
     * override this if needed, if the progress bar isn't smooth
     */
    return 1.0f;
}

/**
 * @return Algorithm sub-algorithm weight
 */
float
AlgorithmAnnotationResample::getSubAlgorithmWeight()
{
    /*
     * If you use a subalgorithm
     */
    //return AlgorithmInsertNameHere::getAlgorithmWeight()
    return 0.0f;
}

