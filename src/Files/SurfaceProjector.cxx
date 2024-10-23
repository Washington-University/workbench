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

#include <cmath>
#include <cstdlib>
#include <limits>

#define __SURFACE_PROJECTOR_DEFINE__
#include "SurfaceProjector.h"
#undef __SURFACE_PROJECTOR_DEFINE__

#include "CaretLogger.h"
#include "FociFile.h"
#include "Focus.h"
#include "MathFunctions.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * \class caret::SurfaceProjector
 * \brief Project points to a surface.
 */

/**
 * Constructor for projection to a given surface.
 *
 * @param surfaceFile
 *    Surface to which projection takes place.  For proper
 *    projection (particularly flat and spherical) surfaces, it is important
 *    that the surface's type is correctly set.
 */
SurfaceProjector::SurfaceProjector(const SurfaceFile* surfaceFile)
    : CaretObject(),
    m_surfaceFileLeft(NULL),
    m_surfaceFileRight(NULL),
    m_surfaceFileCerebellum(NULL),
    m_mode(MODE_SURFACES)
{
    CaretAssert(surfaceFile);
    m_surfaceFiles.push_back(surfaceFile);
    initializeMembersSurfaceProjector();
}

/**
 * Constructor for projection to closest of a group of surfaces.
 *
 * @param surfaceFiles
 *    Vector of Surfaces to which projection takes place.  For proper
 *    projection (particularly flat and spherical) surfaces, it is important
 *    that the surface's type is correctly set.
 */
SurfaceProjector::SurfaceProjector(const std::vector<const SurfaceFile*>& surfaceFiles)
: CaretObject(),
m_surfaceFileLeft(NULL),
m_surfaceFileRight(NULL),
m_surfaceFileCerebellum(NULL),
m_mode(MODE_SURFACES)
{
    const int32_t numberOfSurfaces = static_cast<int32_t>(surfaceFiles.size());
    for (int32_t i = 0; i < numberOfSurfaces; i++) {
        m_surfaceFiles.push_back(surfaceFiles[i]);
    }
    initializeMembersSurfaceProjector();
}

/**
 * Constructor that allows ambiguous projections for items between a cortical
 * surface and the cerebellum.  Items that have a positive X-coordinate
 * are projected to the right cortex or the cerebellum.  Items that have
 * a negative X-coordinate are projected to the left cortex or the cerebellum.
 * Some of the surface files may be NULL but at least one must be valid.
 *
 * @param leftSurfaceFile
 *     Surface file for left cortex.
 * @param rightSurfaceFile
 *     Surface file for right cortex.
 * @param cerebellumSurfaceFile
 *     Surface file for cerebellum cortex.
 */
SurfaceProjector::SurfaceProjector(const SurfaceFile* leftSurfaceFile,
                                   const SurfaceFile* rightSurfaceFile,
                                   const SurfaceFile* cerebellumSurfaceFile)
: CaretObject(),
m_surfaceFileLeft(leftSurfaceFile),
m_surfaceFileRight(rightSurfaceFile),
m_surfaceFileCerebellum(cerebellumSurfaceFile),
m_mode(MODE_LEFT_RIGHT_CEREBELLUM)
{
}


/**
 * Destructor
 */
SurfaceProjector::~SurfaceProjector()
{
}

/**
 * Initialize members of this instance.
 */
void
SurfaceProjector::initializeMembersSurfaceProjector()
{
    m_surfaceOffset = 0.0;
    m_surfaceOffsetValid = false;

    /*
     * Validate when logger is set at a specified level
     * If the level is changed, all need to change level
     * where validation message is logged.
     */
    m_validateFlag = CaretLogger::getLogger()->isFine();
    m_validateItemName = "";
}


/**
 * Set the desired offset of projected items from the surface->
 *
 * @param surfaceOffset - distance above the surface->
 *
 */
void
SurfaceProjector::setSurfaceOffset(const float surfaceOffset)
{
    m_surfaceOffset = surfaceOffset;
    m_surfaceOffsetValid = true;
}

/**
 * Project all foci in a foci file.
 * @param fociFile
 *     The foci file.
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectFociFile(FociFile* fociFile)
{
    CaretAssert(fociFile);
    const int32_t numberOfFoci = fociFile->getNumberOfFoci();
    
    AString errorMessage = "";
    for (int32_t i = 0; i < numberOfFoci; i++) {
        Focus* focus = fociFile->getFocus(i);
        try {
            if (m_validateFlag) {
                m_validateItemName = ("Focus "
                                      + AString::number(i)
                                      + ", "
                                      + focus->getName());
            }
            projectFocus(i,
                         focus);
        }
        catch (const SurfaceProjectorException& spe) {
            if (errorMessage.isEmpty() == false) {
                errorMessage += "\n";
            }
            errorMessage += (focus->getName()
                             + ", index="
                             + AString::number(i)
                             + ": "
                             + spe.whatString());
        }
    }
    
    if (errorMessage.isEmpty() == false) {
        throw SurfaceProjectorException(errorMessage);
    }
}

/**
 * Project a focus.
 * @param focusIndex
 *    Index of the focus (negative indicates no index)
 * @param focus
 *    The focus.
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectFocus(const int32_t focusIndex,
                               Focus* focus)
{
    const int32_t numberOfProjections = focus->getNumberOfProjections();
    CaretAssert(numberOfProjections > 0);
    if (numberOfProjections < 0) {
        throw SurfaceProjectorException("Focus has no projections, no stereotaxic coordinate.");
    }
    focus->removeExtraProjections();
    SurfaceProjectedItem* spi = focus->getProjection(0);

    SurfaceProjectedItem* spiSecond = NULL;
    if (m_surfaceFileCerebellum != NULL) {
        spiSecond = new SurfaceProjectedItem();
    }
    
    m_allowEdgeProjection = true;
    projectItem(spi,
                spiSecond);
    
    if (spiSecond != NULL) {
        if (spiSecond->hasValidProjection()) {
            focus->addProjection(spiSecond);
        }
        else {
            delete spiSecond;
            spiSecond = NULL;
        }
    }
    
    if (m_projectionWarning.isEmpty() == false) {
        AString msg = ("Focus: Name="
                       + focus->getName());
        if (focusIndex >= 0) {
            msg += (", Index="
                    + AString::number(focusIndex));
        }
        msg += (": "
                + m_projectionWarning);
        CaretLogWarning(msg);;
    }
}

/**
 * Project to the surface(s) triangles (barycentric projection)
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItemToTriangle(SurfaceProjectedItem* spi)
{
    CaretAssert(spi);
    m_allowEdgeProjection = false;
    projectItem(spi,
                NULL);
}

/**
 * Project to the surface(s) triangles (barycentric projection)
 * or edges (van-essen projection).
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItemToTriangleOrEdge(SurfaceProjectedItem* spi)
{
    CaretAssert(spi);
    m_allowEdgeProjection = true;
    projectItem(spi,
                NULL);
}


/**
 * Project to the appropriate surface(s).
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 * @param secondSpi
 *    For left/right/cerebellum projections, if there is ambiguity for an
 *    item (between cortex and cerebellum), this projection will be set if
 *    it is not NULL.
 * @throws SurfaceProjectorException  
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItem(SurfaceProjectedItem* spi,
                              SurfaceProjectedItem* secondSpi)
{
    m_projectionWarning = "";
    
    /*
     * Get position of item.
     */
    float xyz[3];
    if (spi->isStereotaxicXYZValid() == false) {
        throw SurfaceProjectorException( "Stereotaxic position is invalid, cannot project.");
    }
    spi->getStereotaxicXYZ(xyz);
    
    if (secondSpi != NULL) {
        secondSpi->setStereotaxicXYZ(xyz);
    }
    
    switch (m_mode) {
        case MODE_LEFT_RIGHT_CEREBELLUM:
        {
            if (xyz[0] < 0.0) {
                if (m_surfaceFileLeft != NULL) {
                    if (m_surfaceFileCerebellum != NULL) {
                        const float leftDist = m_surfaceFileLeft->getSignedDistanceHelper()->dist(xyz,
                                                                                                  SignedDistanceHelper::NORMALS);
                        const float cerebellumDist = m_surfaceFileCerebellum->getSignedDistanceHelper()->dist(xyz,
                                                                                                              SignedDistanceHelper::NORMALS);
                        float ratio = 1000000.0;
                        if (cerebellumDist != 0.0) {
                            ratio = leftDist / cerebellumDist;
                        }
                        
                        if (ratio > s_cerebellumSurfaceCutoff) {
                            projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                             spi);
                        }
                        else if (ratio < s_corticalSurfaceCutoff) {
                            projectItemToSurfaceFile(m_surfaceFileLeft,
                                                     spi);
                        }
                        else {
                            projectItemToSurfaceFile(m_surfaceFileLeft,
                                                     spi);
                            if (secondSpi != NULL) {
                                projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                                         secondSpi);
                            }
                        }
                    }
                    else {
                        projectItemToSurfaceFile(m_surfaceFileLeft,
                                                 spi);
                    }
                }
                else if (m_surfaceFileCerebellum != NULL) {
                    projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                             spi);
                }
            }
            else {
                if (m_surfaceFileRight != NULL) {
                    if (m_surfaceFileCerebellum != NULL) {
                        const float rightDist = m_surfaceFileRight->getSignedDistanceHelper()->dist(xyz,
                                                                                                  SignedDistanceHelper::NORMALS);
                        const float cerebellumDist = m_surfaceFileCerebellum->getSignedDistanceHelper()->dist(xyz,
                                                                                                              SignedDistanceHelper::NORMALS);
                        float ratio = 1000000.0;
                        if (cerebellumDist != 0.0) {
                            ratio = rightDist / cerebellumDist;
                        }
                        
                        if (ratio > s_cerebellumSurfaceCutoff) {
                            projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                                     spi);
                        }
                        else if (ratio < s_corticalSurfaceCutoff) {
                            projectItemToSurfaceFile(m_surfaceFileRight,
                                                     spi);
                        }
                        else {
                            projectItemToSurfaceFile(m_surfaceFileRight,
                                                     spi);
                            if (secondSpi != NULL) {
                                projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                                         secondSpi);
                            }
                        }
                    }
                    else {
                        projectItemToSurfaceFile(m_surfaceFileRight,
                                                 spi);
                    }
                }
                else if (m_surfaceFileCerebellum != NULL) {
                    projectItemToSurfaceFile(m_surfaceFileCerebellum,
                                             spi);
                }
            }
        }
            break;
        case MODE_SURFACES:
        {
            const int32_t numberOfSurfaceFiles = static_cast<int32_t>(m_surfaceFiles.size());
            if (numberOfSurfaceFiles <= 0) {
                throw SurfaceProjectorException("No surface for projection!");
            }
            
            int32_t nearestSurfaceIndex = -1;
            if (numberOfSurfaceFiles == 1) {
                nearestSurfaceIndex = 0;
            }
            else {
                /*
                 * Find surface closest to node.
                 */
                float nearestDistance = std::numeric_limits<float>::max();
                for (int32_t i = 0; i < numberOfSurfaceFiles; i++) {
                    const SurfaceFile* sf = m_surfaceFiles[i];
                    CaretPointer<SignedDistanceHelper> sdh = sf->getSignedDistanceHelper();
                    const float absDist = std::fabs(sdh->dist(xyz, SignedDistanceHelper::NORMALS));
                    if (absDist < nearestDistance) {
                        nearestDistance = absDist;
                        nearestSurfaceIndex = i;
                    }
                }
            }
            
            if (nearestSurfaceIndex < 0) {
                throw SurfaceProjectorException("Failed to find surface for projection.  This may occur "
                                                "if there is no left or right anatomical surface loaded.");
            }
            const SurfaceFile* projectionSurfaceFile = m_surfaceFiles[nearestSurfaceIndex];
            projectItemToSurfaceFile(projectionSurfaceFile,
                                     spi);
        }
            break;
    }
}

/**
 * Project to the given surface
 * @param surfaceFile
 *    Surface to which triangle projection is made.
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This items XYZ coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 */
void
SurfaceProjector::projectItemToSurfaceFile(const SurfaceFile* surfaceFile,
                                              SurfaceProjectedItem* spi)
{
    float originalXYZ[3];
    spi->getStereotaxicXYZ(originalXYZ);
    
    float xyz[3] = {
        originalXYZ[0],
        originalXYZ[1],
        originalXYZ[2]
    };
    
    float projXYZ[3];
    float stereoXYZ[3];
    float distanceError = 0.0;
    bool projectionValid = false;
    
    projectToSurface(surfaceFile,
                     xyz,
                     spi);
    
    if (spi->getBarycentricProjection()->isValid()
        || spi->getVanEssenProjection()->isValid()) {
        spi->getProjectedPosition(*surfaceFile, projXYZ, false);
        spi->getStereotaxicXYZ(stereoXYZ);
        distanceError = MathFunctions::distance3D(projXYZ, stereoXYZ);
        projectionValid = true;
    }
    
    if (distanceError > s_projectionDistanceError) {
        bool perturbIfError = true;
        if (perturbIfError) {
            /*
             * Initialize with first try
             */
            float bestXYZ[3] = {
                xyz[0],
                xyz[1],
                xyz[2]
            };
            float bestDistance = distanceError;
            bool bestValid = true;
            const float originalDistanceError = distanceError;
            
            for (int32_t iTry = 0; iTry < 10; iTry++) {
                const float randomZeroToOne = ((float)std::rand()) / ((float)RAND_MAX);
                const float randomPlusMinusOneHalf = randomZeroToOne - 0.5;
                const float moveLittleBit = randomPlusMinusOneHalf * 0.5;
                xyz[0] = originalXYZ[0] + moveLittleBit;
                xyz[1] = originalXYZ[1] + moveLittleBit;
                xyz[2] = originalXYZ[2] + moveLittleBit;
                
                SurfaceProjectedItem spiTest;
                spiTest.setStereotaxicXYZ(originalXYZ);
                projectToSurface(surfaceFile,
                                 xyz,
                                 &spiTest);
                
                if (spiTest.getBarycentricProjection()->isValid()
                    || spiTest.getVanEssenProjection()->isValid()) {
                    spiTest.getProjectedPosition(*surfaceFile, projXYZ, false);
                    spiTest.getStereotaxicXYZ(stereoXYZ);
                    distanceError = MathFunctions::distance3D(projXYZ, stereoXYZ);
                    projectionValid = true;
                    
//                    std::cout << "Moved from original ("
//                    << AString::fromNumbers(originalXYZ, 3, ",")
//                    << ") to ("
//                    << AString::fromNumbers(xyz, 3, ",")
//                    << ") distance error now="
//                    << distanceError
//                    << std::endl;
                    
                    if (distanceError < bestDistance) {
                        bestXYZ[0] = xyz[0];
                        bestXYZ[1] = xyz[1];
                        bestXYZ[2] = xyz[2];
                        bestDistance = distanceError;
                        bestValid = true;
                    }
                }
            }
            
            if (bestValid) {
                projectToSurface(surfaceFile,
                                 bestXYZ,
                                 spi);
                
                if (spi->getBarycentricProjection()->isValid()
                    || spi->getVanEssenProjection()->isValid()) {
                    spi->getProjectedPosition(*surfaceFile, projXYZ, false);
                    spi->getStereotaxicXYZ(stereoXYZ);
                    distanceError = MathFunctions::distance3D(projXYZ, stereoXYZ);
                    
                    m_projectionWarning += ("Was moved due to projection error from ("
                                            + AString::fromNumbers(originalXYZ, 3, ",")
                                            + ") to ("
                                            + AString::fromNumbers(bestXYZ, 3, ",")
                                            + ") with distance error reduced from "
                                            + AString::number(originalDistanceError)
                                            + " to "
                                            + AString::number(distanceError));
                }
            }
        }
        
    }
    
    if (m_validateFlag == false) {
        if (distanceError > s_projectionDistanceError) {
            m_projectionWarning += ("Projection Warning: Error="
                                   + AString::number(distanceError)
                                   + "mm, Stereotaxic=("
                                   + AString::fromNumbers(stereoXYZ, 3, ",")
                                   + "), Projected=("
                                   + AString::fromNumbers(projXYZ, 3, ",")
                                   + ")");
        }
    }
    
    if (m_validateFlag) {
        bool errorFlag = false;
        AString validateString;
        if (projectionValid) {
            AString projTypeString = "Unprojected";
            AString projInfo;
            if (spi->getBarycentricProjection()->isValid()) {
                projTypeString = "Triangle";
                if (spi->getBarycentricProjection()->isDegenerate()) {
                    projTypeString = "-degenerate";
                }
                projInfo = spi->getBarycentricProjection()->toString();
            }
            else if (spi->getVanEssenProjection()->isValid()) {
                projTypeString = "Edge";
                projInfo = spi->getVanEssenProjection()->toString();
            }
            AString matchString = "";
            if (distanceError > 0.001) {
                matchString = " FAILED *************************";
                errorFlag = true;
            }
            if (validateString.isEmpty() == false) {
                validateString += "\n";
            }
            if (spi->getStructure() == StructureEnum::CEREBELLUM) {
                if (spi->getVanEssenProjection()->isValid()) {
                    //errorFlag = true;
                }
            }
            validateString += (m_validateItemName
                               + ": projType="
                               + projTypeString
                               + ": structure="
                               + StructureEnum::toName(spi->getStructure())
                               + ", stereoPos=("
                               + AString::fromNumbers(stereoXYZ, 3, ",")
                               + "), projPos=("
                               + AString::fromNumbers(projXYZ, 3, ",")
                               + "): stereo/proj positions differ by "
                               + AString::number(distanceError, 'f', 3)
                               + matchString
                               + "\n");
            if (projInfo.isEmpty() == false) {
                validateString += (projInfo
                                   + "\n");
            }
        }
        else {
            validateString += (m_validateItemName
                               + ": failed to project\n");
            errorFlag = true;
        }
        
        if (errorFlag
            && (validateString.isEmpty() == false)) {
            CaretLogFine(validateString);
        }
    }
}

/**
 * Project to the surface
 * @param surfaceFile
 *    Surface to which triangle projection is made.
 * @param xyz
 *    Coordinate that is being projected.
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This items XYZ coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 */
void
SurfaceProjector::projectToSurface(const SurfaceFile* surfaceFile,
                                   const float xyz[3],
                                   SurfaceProjectedItem* spi)
           
{
    //
    // If needed, create node locator
    //
    if (surfaceFile->getNumberOfNodes() <= 0) {
        throw SurfaceProjectorException("Surface file contains no nodes: "
                                        + surfaceFile->getFileNameNoPath());
    }
    
    if (surfaceFile->getNumberOfTriangles() <= 0) {
        throw SurfaceProjectorException("Surface topology contains no triangles: "
                                        + surfaceFile->getFileNameNoPath());
    }
    
    m_sphericalSurfaceRadius = 0.0;
    m_surfaceTypeHint = SURFACE_HINT_THREE_DIMENSIONAL;
    switch (surfaceFile->getSurfaceType()) {
        case SurfaceTypeEnum::FLAT:
            m_surfaceTypeHint = SURFACE_HINT_FLAT;
            break;
        case SurfaceTypeEnum::SPHERICAL:
            m_surfaceTypeHint = SURFACE_HINT_SPHERE;
            m_sphericalSurfaceRadius = surfaceFile->getSphericalRadius();
            break;
        default:
            m_surfaceTypeHint = SURFACE_HINT_THREE_DIMENSIONAL;
            break;
    }
    
    //
    // Default to invalid projection
    //
    SurfaceProjectionBarycentric* baryProj = spi->getBarycentricProjection();
    baryProj->setValid(false);
    SurfaceProjectionVanEssen* vanEssenProj = spi->getVanEssenProjection();
    vanEssenProj->setValid(false);
        
    /*
     * Determine if projected to node/edge/triangle
     */
    ProjectionLocation projectionLocation;
    getProjectionLocation(surfaceFile,
                          xyz,
                          projectionLocation);
    if (m_validateFlag) {
        if (m_validateItemName.isEmpty() == false) {
            m_validateItemName += "\n";
        }
        m_validateItemName += ("ORIGINAL: "
                               + projectionLocation.toString(surfaceFile));
    }
    
    /*
     * If projected to edge and edge projection allowed
     */
    if (m_allowEdgeProjection
        && (projectionLocation.m_type == ProjectionLocation::EDGE)) {
        vanEssenProj->setPosAnatomical(xyz);
        projectWithVanEssenAlgorithm(surfaceFile,
                                     projectionLocation,
                                     vanEssenProj);
        
        if (vanEssenProj->isValid() == false) {
            throw SurfaceProjectorException("Edge projection failed.");
        }
    }
    else {
        /*
         * Convert the projection to a triangle projection.
         */
        if ((projectionLocation.m_type == ProjectionLocation::EDGE) ||
            (projectionLocation.m_type == ProjectionLocation::NODE)) {
            convertToTriangleProjection(surfaceFile,
                                        projectionLocation);
            if (m_validateFlag) {
                if (m_validateItemName.isEmpty() == false) {
                    m_validateItemName += "\n";
                }
                m_validateItemName += ("ALTERED: "
                                       + projectionLocation.toString(surfaceFile));
            }
        }
        
        
        projectToSurfaceTriangle(surfaceFile,
                                 projectionLocation,
                                 spi->getBarycentricProjection());
        
        if (baryProj->isValid() == false) {
            throw SurfaceProjectorException("Triangle projection failed.");
        }
    }
    
    spi->setStructure(surfaceFile->getStructure());
}

/**
 * Convert an edge or node projection to a triangle projection which
 * may become a degenerate triangle projection.
 * @param surfaceFile
 *    Surface to which triangle projection is made.
 * @param projectionLocation
 *    Contains informaiton about item on the surface file.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 */
void
SurfaceProjector::convertToTriangleProjection(const SurfaceFile* surfaceFile,
                                              ProjectionLocation& projectionLocation)
                                                        
{
    bool doIt = false;
    
    switch (projectionLocation.m_type) {
        case ProjectionLocation::EDGE:
            doIt = true;
            break;
        case ProjectionLocation::INVALID:
            break;
        case ProjectionLocation::NODE:
            doIt = true;
            break;
        case ProjectionLocation::TRIANGLE:
            break;
    }
    
    if (doIt) {
        SurfaceProjectionBarycentric baryProj;
        checkItemInTriangle(surfaceFile,
                            projectionLocation.m_triangleIndices[0],
                            projectionLocation.m_pointXYZ,
                            s_extremeTriangleAreaTolerance,
                            &baryProj);
        if (baryProj.isValid()) {
            projectionLocation.m_type = ProjectionLocation::TRIANGLE;
            const int32_t* nodes = baryProj.getTriangleNodes();
            projectionLocation.m_nodes[0] = nodes[0];
            projectionLocation.m_nodes[1] = nodes[1];
            projectionLocation.m_nodes[2] = nodes[2];
            const float* areas = baryProj.getTriangleAreas();
            projectionLocation.m_weights[0] = areas[0];
            projectionLocation.m_weights[1] = areas[1];
            projectionLocation.m_weights[2] = areas[2];
            projectionLocation.m_signedDistance = baryProj.getSignedDistanceAboveSurface();
            projectionLocation.m_absoluteDistance = std::fabs(projectionLocation.m_signedDistance);
        }
        else {
            throw SurfaceProjectorException("Failed to convert from edge/node projection to triangle projection");
        }
    }
    
}

/**
 * Project a coordinate to the surface using a barycentric projection.
 * @param surfaceFile
 *    Surface to which triangle projection is made.
 * @param projectionLocation
 *    Contains informaiton about item on the surface file.
 * @param baryProj
 *    The barycentric projection that will be updated.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 *
 */
void
SurfaceProjector::projectToSurfaceTriangle(const SurfaceFile* surfaceFile,
                                           const ProjectionLocation& projectionLocation,
                                           SurfaceProjectionBarycentric* baryProj)
                                           
{
    /*
     * At one time, there was a need to 'perturb' (slightly move) the
     * surface, probably for registration.
     */
    
    
    projectToSurfaceTriangleAux(surfaceFile,
                        projectionLocation,
                        baryProj);
    
    if (baryProj->isValid()) {
        if (m_surfaceOffsetValid) {
            baryProj->setSignedDistanceAboveSurface(m_surfaceOffset);
        }
    }
}

/**
 * Get the location on the surface nearest the given coordinate.
 * @param surfaceFile
 *    Surface for location.
 * @param xyz
 *    The coordinate.
 * @param projectionLocation
 *    Output containing location on surface information.
 */
void
SurfaceProjector::getProjectionLocation(const SurfaceFile* surfaceFile,
                                        const float xyz[3],
                                        ProjectionLocation& projectionLocation) const
{
    /*
     * Find nearest point on the surface
     */
    CaretPointer<SignedDistanceHelper> sdh = surfaceFile->getSignedDistanceHelper();
    BarycentricInfo baryInfo;
    sdh->barycentricWeights(xyz, baryInfo);
    
    int32_t nearestNode = -1;
    float maxWeight = -1;
    for (int32_t i = 0; i < 3; i++) {
        if (baryInfo.baryWeights[i] >= 0.0) {
            const float w = baryInfo.baryWeights[i];
            if (w > maxWeight) {
                nearestNode = baryInfo.nodes[i];
                maxWeight = w;
            }
        }
    }
    
    if (nearestNode < 0) {
        throw SurfaceProjectorException("ERROR: Nearest node is invalid");
    }
    
    float signedDistance = 0.0;
    switch (baryInfo.type) {
        case BarycentricInfo::NODE:
        {
            const float* nodeNormal = surfaceFile->getNormalVector(baryInfo.nodes[0]);
            const float* c1 = surfaceFile->getCoordinate(baryInfo.nodes[0]);
            const float aboveBelowPlane =
            MathFunctions::signedDistanceFromPlane(nodeNormal, c1, xyz);
            const float signValue = ((aboveBelowPlane > 0.0) ? 1.0 : -1.0);
            signedDistance = (MathFunctions::distance3D(xyz, c1) * signValue);
        }
            break;
        case BarycentricInfo::EDGE:
        {
            const float* n1 = surfaceFile->getNormalVector(baryInfo.nodes[0]);
            const float* n2 = surfaceFile->getNormalVector(baryInfo.nodes[1]);
            float avgNormal[3];
            MathFunctions::addVectors(n1, n2, avgNormal);
            MathFunctions::normalizeVector(avgNormal);
            
            const float* c1 = surfaceFile->getCoordinate(baryInfo.nodes[0]);
            const float* c2 = surfaceFile->getCoordinate(baryInfo.nodes[1]);
            MathFunctions::distanceToLine3D(c1, c2, xyz);
            
            const float aboveBelowPlane =
            MathFunctions::signedDistanceFromPlane(avgNormal, baryInfo.point, xyz);
            const float signValue = ((aboveBelowPlane > 0.0) ? 1.0 : -1.0);
            signedDistance = (MathFunctions::distance3D(xyz, baryInfo.point) * signValue);
        }
            break;
        case BarycentricInfo::TRIANGLE:
        {
            float triangleNormal[3];
            surfaceFile->getTriangleNormalVector(baryInfo.triangle, triangleNormal);
            const float* c1 = surfaceFile->getCoordinate(baryInfo.nodes[0]);
            signedDistance =
            MathFunctions::signedDistanceFromPlane(triangleNormal, c1, xyz);
        }
            break;
    }
    
    /*
     * Topology helper
     */
    CaretPointer<const TopologyHelper> topologyHelper = surfaceFile->getTopologyHelper();
    
    /*
     * Triangle(s) near projection point on surface
     */
    std::vector<int32_t> nearbyTriangles;
    
    /*
     * Load up the projection information.
     */
    projectionLocation.m_type = ProjectionLocation::INVALID;
    switch (baryInfo.type) {
        case BarycentricInfo::NODE:
        {
            projectionLocation.m_type = ProjectionLocation::NODE;
            int32_t numTriangles = 0;
            const int32_t* nodesTriangles = topologyHelper->getNodeTiles(baryInfo.nodes[0],
                                                                        numTriangles);
            
            /*
             * Make sure nearest triangle is first and 
             * keep triangles ordering
             */
            int32_t iStart = 0;
            for (int32_t i = 0; i < numTriangles; i++) {
                if (nodesTriangles[i] == baryInfo.triangle) {
                    iStart = i;
                    break;
                }
            }
            if (iStart < 0) {
                throw SurfaceProjectorException("PROGRAM ERROR: Nearest triangle node found to be associated with nearest node");
            }
            for (int32_t i = iStart; i < numTriangles; i++) {
                nearbyTriangles.push_back(nodesTriangles[i]);
            }
            for (int32_t i = 0; i < iStart; i++) {
                nearbyTriangles.push_back(nodesTriangles[i]);
            }
        }
            break;
        case BarycentricInfo::EDGE:
        {
            projectionLocation.m_type = ProjectionLocation::EDGE;
            const int32_t oppositeTriangle = surfaceFile->getTriangleThatSharesEdge(baryInfo.nodes[0],
                                                                                    baryInfo.nodes[1],
                                                                                    baryInfo.triangle);
            nearbyTriangles.push_back(baryInfo.triangle);
            nearbyTriangles.push_back(oppositeTriangle);
        }
            break;
        case BarycentricInfo::TRIANGLE:
            projectionLocation.m_type = ProjectionLocation::TRIANGLE;
            nearbyTriangles.push_back(baryInfo.triangle);
            break;
    }
    
    for (int32_t i = 0; i < 3; i++) {
        projectionLocation.m_pointXYZ[i] = xyz[i];
        projectionLocation.m_surfaceXYZ[i] = baryInfo.point[i];
        projectionLocation.m_nodes[i] = baryInfo.nodes[i];
        projectionLocation.m_weights[i] = baryInfo.baryWeights[i];
    }

    projectionLocation.m_numberOfTriangles = static_cast<int32_t>(nearbyTriangles.size());
    projectionLocation.m_triangleIndices = new int32_t[projectionLocation.m_numberOfTriangles];
    for (int32_t i = 0; i < projectionLocation.m_numberOfTriangles; i++) {
        projectionLocation.m_triangleIndices[i] = nearbyTriangles[i];
    }
    projectionLocation.m_absoluteDistance = baryInfo.absDistance;
    projectionLocation.m_signedDistance   = signedDistance;
    projectionLocation.m_nearestNode = nearestNode;
    
    
    AString distErrorMessage = "";
    float distError = std::fabs(signedDistance) - baryInfo.absDistance;
    if (distError > 0.01) {
        throw SurfaceProjectorException("ERROR: signed/abs distance mismatch: "
                                        + projectionLocation.toString(surfaceFile));
    }
}

/**
 * Project a coordinate to the surface
 * @param surfaceFile
 *    Surface file to which item is projected.
 * @param projectionLocation
 *    Contains informaiton about item on the surface file.
 * @param baryProj
 *    The barycentric projection that will be set.
 * @return
 *    The node nearest the coordinate.
 * @throws SurfaceProjectorException
 *    If projecting an item failed.
 */
int32_t
SurfaceProjector::projectToSurfaceTriangleAux(const SurfaceFile* surfaceFile,
                                              const ProjectionLocation& projectionLocation,
                                      SurfaceProjectionBarycentric* baryProj)
                                            
{
    /*
     * Set the projection.
     */
    baryProj->setTriangleAreas(projectionLocation.m_weights);
    baryProj->setTriangleNodes(projectionLocation.m_nodes);
    baryProj->setProjectionSurfaceNumberOfNodes(surfaceFile->getNumberOfNodes());
    if (m_surfaceOffsetValid) {
        baryProj->setSignedDistanceAboveSurface(m_surfaceOffset);
    }
    else {
        baryProj->setSignedDistanceAboveSurface(projectionLocation.m_signedDistance);
    }
    baryProj->setValid(true);
    
    return projectionLocation.m_nearestNode;    
}

/**
 * See if the coordinate is within the triangle.
 * @param surfaceFile
 *    Surface file to which item is projected.
 * @param triangleNumber
 *    Triangle to check.
 * @param xyz
 *    The coordinate
 * @param degenerateTolerance
 *    If the point is outside the triangle, an output area will be negative.
 *    In most cases use zero or a negative value (-0.01) very near zero.  A very
 *    negative value can be used to allow degenerate cases.
 * @param baryProj
 *    Barycentric projection into triangle.
 */
void
SurfaceProjector::checkItemInTriangle(const SurfaceFile* surfaceFile,
                                      const int32_t triangleNumber,
                                      const float xyz[3],
                                      const float degenerateTolerance,
                                      SurfaceProjectionBarycentric* baryProj)
{
    //
    // Vertices of the triangle
    //
    const int32_t* tn = surfaceFile->getTriangle(triangleNumber);
    const float* v1 = surfaceFile->getCoordinate(tn[0]);
    const float* v2 = surfaceFile->getCoordinate(tn[1]);
    const float* v3 = surfaceFile->getCoordinate(tn[2]);
    
    //
    // coordinate that may be pushed to a plane depending upon surfac type
    //
    float queryXYZ[3] = {
        xyz[0],
        xyz[1],
        xyz[2]
    };
    
    //
    // Initialize normal vector to normal of triangle
    //
    float normal[3];
    MathFunctions::normalVector(v1, v2, v3, normal);
    
    //
    // Adjust the query coordinate based upon the surface type
    //
    switch (m_surfaceTypeHint) {
        case SURFACE_HINT_FLAT:
            //
            // Override normal with flat surface normal
            //
            normal[0] = 0.0f;
            normal[1] = 0.0f;
            normal[2] = 1.0f;
            queryXYZ[2] = 0.0f; // place on plane
            break;
        case SURFACE_HINT_SPHERE:
        {
            if (m_sphericalSurfaceRadius > 0.0) {
                MathFunctions::normalizeVector(queryXYZ);
                queryXYZ[0] *= m_sphericalSurfaceRadius;
                queryXYZ[1] *= m_sphericalSurfaceRadius;
                queryXYZ[2] *= m_sphericalSurfaceRadius;
            }
            
            float origin[3] = { 0.0f, 0.0f, 0.0f };
            float xyzDistance[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            if (MathFunctions::rayIntersectPlane(v1, v2, v3,
                                                origin, queryXYZ,
                                                xyzDistance) == false) {
                //
                // Ray does not intersect, must be parallel to plane
                //
                return;
            }
            
            //
            // Use intersection point
            //
            queryXYZ[0] = xyzDistance[0];
            queryXYZ[1] = xyzDistance[1];
            queryXYZ[2] = xyzDistance[2];
        }
            break;
        case SURFACE_HINT_THREE_DIMENSIONAL:
        {
            //
            // Project point to the triangle
            //
            float xyzOnPlane[3];
            MathFunctions::projectPoint(queryXYZ, v1, normal, xyzOnPlane);
            queryXYZ[0] = xyzOnPlane[0];
            queryXYZ[1] = xyzOnPlane[1];
            queryXYZ[2] = xyzOnPlane[2];
        }
            break;
    }
    
    //
    // Note that if tolerance is a small negative number (which is done to handle
    // degenerate cases - projected point on vertex or edge of triangle) an area may
    // be negative and we continue searching tiles.  If all areas are positive
    // then there is no need to continue searching.
    //
    float areas[3] = { 0.0f, 0.0f, 0.0f };
    int result = triangleAreas(v1,
                               v2,
                               v3,
                               normal,
                               queryXYZ,
                               degenerateTolerance,
                               areas);
    if (result != 0) {
        baryProj->setValid(true);
        if (result < 0) {
            baryProj->setDegenerate(true);
        }
        float signedDistanceToTriangle =
        MathFunctions::signedDistanceFromPlane(normal, v1, xyz);
         
        baryProj->setTriangleAreas(areas);
        baryProj->setTriangleNodes(tn);
        baryProj->setSignedDistanceAboveSurface(signedDistanceToTriangle);
        baryProj->setProjectionSurfaceNumberOfNodes(surfaceFile->getNumberOfNodes());
    }
}

/**
 * Compute the signed areas formed by assuming "xyz" is contained in the triangle formed
 * by the points "p1, p2, p3".  "area2" and "area3" may not be set if "xyz" is not
 * within the triangle.
 *
 * @param p1
 *    Coordinate of triangle node1.
 * @param p2
 *    Coordinate of triangle node2.
 * @param p3
 *    Coordinate of triangle node3.
 * @param normal
 *    Triangle's normal vector.
 * @param xyz
 *    The coordinate being examined.
 * @param degenerateTolerance
 *    If the point is outside the triangle, an output area will be negative.
 *    In most cases use zero or a negative value (-0.01) very near zero.  A very
 *    negative value can be used to allow degenerate cases.
 * @param areasOut
 *    Output barycentric areas of xyz in the triangle OUTPUT.
 * @return
 *    Returns 1 if all areas are positive (point32_t inside the triangle).
 *    Returns -1 if all areas are greater than the tolerance
 *        (point32_t may be on edge or vertex)
 *    Returns 0 if not in the triangle.
 *
 */
int32_t
SurfaceProjector::triangleAreas(
                   const float p1[3],
                   const float p2[3],
                   const float p3[3],
                   const float normal[3],
                   const float xyz[3],
                   const float degenerateTolerance,
                   float areasOut[3])
{
    float area1 = 0.0f;
    float area2 = 0.0f;
    float area3 = 0.0f;
    int32_t result = 0;
    float triangleArea = 0.0f;
    bool inside = false;
    
    switch (m_surfaceTypeHint) {
        case SURFACE_HINT_FLAT:
            area1 = MathFunctions::triangleAreaSigned2D(p2, p3, xyz);
            if (area1 > degenerateTolerance) {
                area2 = MathFunctions::triangleAreaSigned2D(p3, p1, xyz);
                if (area2 > degenerateTolerance) {
                    area3 = MathFunctions::triangleAreaSigned2D(p1, p2, xyz);
                    if (area3 > degenerateTolerance) {
                        inside = true;
                        triangleArea = MathFunctions::triangleAreaSigned2D(p1,
                                                                          p2,
                                                                          p3);
                    }
                }
            }
            break;
        case SURFACE_HINT_SPHERE:
        case SURFACE_HINT_THREE_DIMENSIONAL:
            area1 = MathFunctions::triangleAreaSigned3D(normal, p2, p3, xyz);
            if (area1 >= degenerateTolerance) {
                area2 = MathFunctions::triangleAreaSigned3D(normal, p3, p1, xyz);
                if (area2 >= degenerateTolerance) {
                    area3 = MathFunctions::triangleAreaSigned3D(normal, p1,p2,xyz);
                    if (area3 >= degenerateTolerance) {
                        inside = true;
                        triangleArea = MathFunctions::triangleArea(p1, p2, p3);
                    }
                }
            }
            break;
    }
    
    if (inside) {
        if ((area1 > 0.0) && (area2 > 0.0) && (area3 > 0.0)) {
            result = 1;
        }
        else {
            result = -1;
        }
//        if (area1 < 0.0) area1 = -area1;
//        if (area2 < 0.0) area2 = -area2;
//        if (area3 < 0.0) area3 = -area3;
        
        if (triangleArea > 0.0) {
            //area1 /= triangleArea;
            //area2 /= triangleArea;
            //area3 /= triangleArea;
        }
        else {
            area1 = 1.0f;
            area2 = 0.0f;
            area3 = 0.0f;
        }
    }
    
    areasOut[0] = area1;
    areasOut[1] = area2;
    areasOut[2] = area3;
    
    return result;
}

/**
 * Perform a VanEssen Projection that projects to the edge of two triangles.
 *
 * @param surfaceFile
 *    File to which item is projected.
 * @param projectionLocation
 *    Contains informaiton about item on the surface file.
 * @param spve
 *    The Van Essen Projection that is setup.
 * @throws SurfaceProjectorException  If projection failure.
 *
 */
void
SurfaceProjector::projectWithVanEssenAlgorithm(const SurfaceFile* surfaceFile,
                                               const ProjectionLocation& projectionLocation,
                                               SurfaceProjectionVanEssen* spve)
           
{
    float xyz[3] = {
        projectionLocation.m_pointXYZ[0],
        projectionLocation.m_pointXYZ[1],
        projectionLocation.m_pointXYZ[2]
    };
    
    const bool pointIsUnderSurface = (projectionLocation.m_signedDistance < 0.0);
    
    //
    // Find nearest triangle to coordinate
    //
    const int32_t nearestTriangle = projectionLocation.m_triangleIndices[0];
    if (nearestTriangle < 0) {
        throw SurfaceProjectorException(
                                            "Unable to find nearest triangle for VanEssen projection.");
    }
    
    //
    // Get triangle's nodes and their coordinates
    //
    const int32_t* tn = surfaceFile->getTriangle(nearestTriangle);
    int32_t n1 = tn[0];
    int32_t n2 = tn[1];
    int32_t n3 = tn[2];
    const float* p1 = surfaceFile->getCoordinate(n1);
    const float* p2 = surfaceFile->getCoordinate(n2);
    const float* p3 = surfaceFile->getCoordinate(n3);
    
    //
    // Project the coordinate to the plane of nearest triangle
    //
    float planeNormal[3];
    MathFunctions::normalVector(p1, p2, p3, planeNormal);
    float xyzOnPlane[3];
    MathFunctions::projectPoint(xyz, p1, planeNormal, xyzOnPlane);
    
    //
    // Adjust for surface offset
    //
    if (m_surfaceOffsetValid) {
        for (int32_t i = 0; i < 3; i++) {
            xyz[i] = xyzOnPlane[i] + planeNormal[i] * m_surfaceOffset;
        }
    }
    
    /*
     * With the nearest triangle, determine which edge is closest
     * to the coordinate
     */
    const int32_t closestVertices[2] = {
        projectionLocation.m_nodes[0],
        projectionLocation.m_nodes[1]
    };
    
    /*
     * Nodes and triangles using the edge
     */
    int32_t iR = closestVertices[0];
    int32_t jR = closestVertices[1];
    int32_t triA = nearestTriangle;
    int32_t triB = projectionLocation.m_triangleIndices[1];
    
    const float* coordJR = surfaceFile->getCoordinate(jR);
    const float* coordIR = surfaceFile->getCoordinate(iR);
    
    /*
     * Normal vector for triangle nearest the coordinate
     */
    float normalA[3];
    surfaceFile->getTriangleNormalVector(triA, normalA);
    
    /*
     * When point is under surface, need to flip the normal vector
     */
    if (pointIsUnderSurface) {
        for (int32_t i = 0; i < 3; i++) {
            normalA[i] *= -1.0;
        }
    }
    
    /*
     * Second triangle might not be found if topology is open or cut.
     */
    float normalB[3] = { 0.0f, 0.0f, 0.0f };
    if (triB >= 0) {
        /*
         * Normal vector for triangle sharing edge with nearest triangle
         */
        surfaceFile->getTriangleNormalVector(triB, normalB);
        
        /*
         * When point is under surface, need to flip the normal vector
         */
        if (pointIsUnderSurface) {
            for (int32_t i = 0; i < 3; i++) {
                normalB[i] *= -1.0;
            }
        }
    }
    else {
        float dR =
        (float)std::sqrt(MathFunctions::distance3D(xyzOnPlane, xyz));
        
        float v[3];
        MathFunctions::subtractVectors(coordJR, coordIR, v);
        
        float t1[3];
        MathFunctions::subtractVectors(xyz, coordIR, t1);
        float t2 = MathFunctions::dotProduct(v, v);
        float t3 = MathFunctions::dotProduct(t1, v);
        
        float QR[3] = { 0.0f, 0.0f, 0.0f };
        for (int32_t j = 0; j < 3; j++) {
            QR[j] = coordIR[j] + ((t3/t2) * v[j]);
        }
        MathFunctions::subtractVectors(coordJR, coordIR, v);
        t2 = MathFunctions::vectorLength(v);
        MathFunctions::subtractVectors(QR, coordIR, t1);
        t3 = MathFunctions::vectorLength(t1);
        float fracRI = 0.0f;
        if (t2 > 0.0f) {
            fracRI = t3/t2;
        }
        
        MathFunctions::subtractVectors(coordIR, coordJR, v);
        t2 = MathFunctions::vectorLength(v);
        MathFunctions::subtractVectors(QR, coordJR, t1);
        t3 = MathFunctions::vectorLength(t1);
        float fracRJ = 0.0f;
        if (t2 > 0.0f) {
            fracRJ = t3/t2;
        }
        else {
            fracRI = 0.0f;  // uses fracRI seems wrong but like this in OLD code
        }
        
        if (fracRI > 1.0f) {
            for (int32_t j = 0; j < 3; j++) {
                QR[j] = coordJR[j];
            }
        }
        if (fracRJ > 1.0f) {
            for (int32_t j = 0; j < 3; j++) {
                QR[j] = coordIR[j];
            }
        }
        
        MathFunctions::subtractVectors(xyz, xyzOnPlane, t1);
        t2 = MathFunctions::vectorLength(t1);
        if (t2 > 0.0f) {
            for (int32_t j = 0; j < 3; j++) {
                t1[j] = t1[j]/t2;
            }
        }
        t3 = MathFunctions::dotProduct(t1, normalA);
        for (int32_t j = 0; j < 3; j++) {
            xyz[j] = QR[j] + (dR * t3 * normalA[j]);
        }
    }
    
    /*
     * Vector from "IR" to "JR"
     */
    float v[3];
    MathFunctions::subtractVectors(coordJR, coordIR, v);

    /*
     * Vector from "IR" to "xyz"
     */
    float t1[3];
    MathFunctions::subtractVectors(xyz, coordIR, t1);
    
    float t2 = MathFunctions::dotProduct(v, v);
    float t3 = MathFunctions::dotProduct(t1, v);
    float QR[3] = { 0.0f, 0.0f, 0.0f };
    for (int32_t j = 0; j < 3; j++) {
        QR[j] = coordIR[j] + ((t3/t2) * v[j]);
    }
    
    if ((triA >= 0) && (triB >= 0)) {
        /*
         * t2 is arccos of angle between the normal vectors of the two triangles
         */
        t2 = MathFunctions::dotProduct(normalA, normalB);
        t2 = std::min(t2, 1.0f);
        
        /*
         * Angle formed by the normal vectors of the two triangles
         */
        spve->setPhiR((float)std::acos(t2));
        if (m_validateFlag) {
            m_validateItemName += (", t2="
                                   + AString::number(t2)
                                   + ", angleDegrees="
                                   + AString::number(MathFunctions::toDegrees(spve->getPhiR())));
        }
    }
    else {
        spve->setPhiR(0.0f);
    }
    
    /*
     * Vector from "QR" to "xyz"
     */
    MathFunctions::subtractVectors(xyz, QR, t1);
    MathFunctions::normalizeVector(t1);
    
    /*
     * t3 is arccos of nearest triangle and "t1"
     */
    t3 = MathFunctions::dotProduct(normalA, t1);
    if (t3 > 0.0f) {
        spve->setThetaR((float)std::acos(t3 * (t3/std::fabs(t3))));
        if ((spve->getThetaR() > -0.001)
            && (spve->getThetaR() < 0.001)) {
            const float oneDegreeRadians = (1.0 * M_PI) / 180.0;
            if (spve->getPhiR() < oneDegreeRadians) {
                m_validateItemName += (",t3="
                                       + AString::number(t3)
                                       + ",thetaR="
                                       + AString::number(spve->getThetaR(), 'f', 10)
                                       + "THETAR=NEAR0,t2=SMALL-POSITIVE***");
            }
        }
    }
    else {
        spve->setThetaR(0.0f);
        if (m_validateFlag) {
            if (t2 < 0.0) {
                m_validateItemName += (" ***THETAR=0,t2=NEG***");
            }
        }
    }
    
    MathFunctions::subtractVectors(coordJR, coordIR, v);
    t2 = MathFunctions::vectorLength(v);
    MathFunctions::subtractVectors(QR, coordIR, t1);
    t3 = MathFunctions::vectorLength(t1);
    if (t2 > 0.0f) {
        spve->setFracRI(t3/t2);
    }
    else {
        spve->setFracRI(0.0f);
    }
    
    MathFunctions::subtractVectors(coordIR, coordJR, v);
    t2 = MathFunctions::vectorLength(v);
    MathFunctions::subtractVectors(QR, coordJR, t1);
    t3 = MathFunctions::vectorLength(t1);
    if (t2 > 0.0f) {
        spve->setFracRJ(t3/t2);
    }
    else {
        spve->setFracRJ(0.0f);
    }
    
    spve->setDR(MathFunctions::distance3D(QR, xyz));
    const int32_t* triANodes = surfaceFile->getTriangle(triA);
    int32_t nodesA[3] = {
        triANodes[0],
        triANodes[1],
        triANodes[2]
    };
    int32_t swapA = nodesA[0];
    nodesA[0] = nodesA[2];
    nodesA[2] = swapA;
    spve->setTriVertices(0, nodesA);
    spve->setTriAnatomical(0,0,surfaceFile->getCoordinate(nodesA[0]));
    spve->setTriAnatomical(0,1,surfaceFile->getCoordinate(nodesA[1]));
    spve->setTriAnatomical(0,2,surfaceFile->getCoordinate(nodesA[2]));
    
    
    if (triB >= 0) {
        const int32_t* triBNodes = surfaceFile->getTriangle(triB);
        int32_t nodesB[3] = {
            triBNodes[0],
            triBNodes[1],
            triBNodes[2]
        };
        int32_t swapB = nodesB[0];
        nodesB[0] = nodesB[2];
        nodesB[2] = swapB;
        spve->setTriVertices(1, nodesB);
        spve->setTriAnatomical(1,0,surfaceFile->getCoordinate(nodesB[0]));
        spve->setTriAnatomical(1,1,surfaceFile->getCoordinate(nodesB[1]));
        spve->setTriAnatomical(1,2,surfaceFile->getCoordinate(nodesB[2]));
    }
    else {
        int32_t intZeros[3] = { 0, 0, 0 };
        spve->setTriVertices(1, intZeros);
        float zeros[3] = { 0.0f, 0.0f, 0.0f };
        spve->setTriAnatomical(1, 0, zeros);
        spve->setTriAnatomical(1, 1, zeros);
        spve->setTriAnatomical(1, 2, zeros);
    }
    
    spve->setVertexAnatomical(0, coordIR);
    spve->setVertexAnatomical(1, coordJR);
    
    spve->setVertex(0, iR);
    spve->setVertex(1, jR);

    spve->setProjectionSurfaceNumberOfNodes(surfaceFile->getNumberOfNodes());
    spve->setValid(true);
}

/* ========================================================================== */

/**
 * \class caret::SurfaceProjector::ProjectionLocation
 * \brief Contains information about nearby point on surface
 */

/**
 * Constructor.
 */
SurfaceProjector::ProjectionLocation::ProjectionLocation()
{
    m_type = INVALID;
    m_triangleIndices = NULL;
    m_numberOfTriangles = 0;
}

/**
 * Destructor.
 */
SurfaceProjector::ProjectionLocation::~ProjectionLocation() {
    if (m_triangleIndices != NULL) {
        delete[] m_triangleIndices;
    }
}

/**
 * Get String describing content.
 * @param surfaceFile
 *    Surface file used for projection
 * @return
 *    Description.
 */
AString
SurfaceProjector::ProjectionLocation::toString(const SurfaceFile* surfaceFile) const
{
    AString typeString;
    switch (m_type) {
        case EDGE:
            typeString = "EDGE";
            break;
        case INVALID:
            typeString = "INVALID";
            break;
        case NODE:
            typeString = "NODE";
            break;
        case TRIANGLE:
            typeString = "TRIANGLE";
            break;
    }
    
    AString msg = (" Type="
                   + typeString
                   + " Pos=("
                   + AString::fromNumbers(m_pointXYZ, 3, ",")
                   + ") SurfacePos=("
                   + AString::fromNumbers(m_surfaceXYZ, 3, ",")
                   + ") Triangles=("
                   + AString::fromNumbers(m_triangleIndices, m_numberOfTriangles, ",")
                   + ") AbsDistance="
                   + AString::number(m_absoluteDistance)
                   + " SignedDistance="
                   + AString::number(m_signedDistance)
                   + " Nodes=("
                   + AString::fromNumbers(m_nodes, 3, ",")
                   + ") Weights=("
                   + AString::fromNumbers(m_weights, 3, ",")
                   + ") NearestNode="
                   + AString::number(m_nearestNode)
                   + " Node-XYZ=("
                   + AString::fromNumbers(surfaceFile->getCoordinate(m_nearestNode), 3,",")
                   + ")");
    
    return msg;
}

