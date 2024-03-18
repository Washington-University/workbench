
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include <iostream>

#define __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_DECLARE__
#include "SurfacePlaneIntersectionToContour.h"
#undef __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_DECLARE__

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "ElapsedTimer.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "Plane.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;


    
/**
 * \class caret::SurfacePlaneIntersectionToContour 
 * \brief Create a contour from intersecting the plane with a surface.
 * \ingroup Files
 *
 * This algorithm is model upon these papers:
 *
 * Utilizing Topological Information to Increase Scan Vector Generation Efficiency
 * Stephen J. Rock Michael J. Wozny
 * Rensselaer Design Research Center Rensselaer Polytechnic Institute Troy, New York 12180
 * Appears in Solid Freeform Fabrication Symposium Proceedings, H. L. Marcus, et. al. (eds.), pp. 28-36, The University of Texas at Austin, Austin, TX, 1991.
 * https://pdfs.semanticscholar.org/1129/b21303dcae34fed2a523f5991f8a1f9b3e32.pdf
 *
 *
 * A Simple Arbitrary Solid Slicer
 * Jin Yao
 * B-Division, Lawrence Livermore National Laboratory Livermore, California 94551
 * https://e-reports-ext.llnl.gov/pdf/321754.pdf
 *
 */

/**
 * Constructor.
 *
 * @param surfaceFile
 *     The surface file.
 * @param intersectionPlane
 *     Plane intersected with the surface.
 * @param caretColor
 *     Solid coloring or, if value is CUSTOM, use the vertex coloring
 * @param vertexColoringRGBA
 *     The per-vertex coloring if 'caretColor' is CUSTOM
 * @param contourThicknessPercentOfViewportHeight
 *     Thickness for the contour as a percentage of viewport height.
 */
SurfacePlaneIntersectionToContour::SurfacePlaneIntersectionToContour(const SurfaceFile* surfaceFile,
                                                                     const Plane& intersectionPlane,
                                                                     const CaretColorEnum::Enum caretColor,
                                                                     const float* vertexColoringRGBA,
                                                                     const float contourThicknessPercentOfViewportHeight)
: SurfacePlaneIntersectionToContour(surfaceFile,
                                    intersectionPlane,
                                    intersectionPlane,
                                    caretColor,
                                    vertexColoringRGBA,
                                    contourThicknessPercentOfViewportHeight)
{
}

/**
 * Constructor.
 *
 * @param surfaceFile
 *     The surface file.
 * @param intersectionPlane
 *     Plane intersected with the surface.
 * @param drawOnPlane
 *     Intersected points are projected to this plane
 * @param caretColor
 *     Solid coloring or, if value is CUSTOM, use the vertex coloring
 * @param vertexColoringRGBA
 *     The per-vertex coloring if 'caretColor' is CUSTOM
 * @param contourThicknessPercentOfViewportHeight
 *     Thickness for the contour as a percentage of viewport height.
 */
SurfacePlaneIntersectionToContour::SurfacePlaneIntersectionToContour(const SurfaceFile* surfaceFile,
                                                                     const Plane& intersectionPlane,
                                                                     const Plane& drawOnPlane,
                                                                     const CaretColorEnum::Enum caretColor,
                                                                     const float* vertexColoringRGBA,
                                                                     const float contourThicknessPercentOfViewportHeight)
: CaretObject(),
m_surfaceFile(surfaceFile),
m_intersectionPlane(intersectionPlane),
m_drawOnPlane(drawOnPlane),
m_caretColor(caretColor),
m_vertexColoringRGBA(vertexColoringRGBA),
m_contourThicknessPercentOfViewportHeight(contourThicknessPercentOfViewportHeight)
{
    CaretAssert(m_surfaceFile);
    
    CaretColorEnum::toRGBAFloat(caretColor,
                                m_solidRGBA.data());
}

/**
 * Destructor.
 */
SurfacePlaneIntersectionToContour::~SurfacePlaneIntersectionToContour()
{
}

/**
 * Create the contour(s).
 *
 * @param graphicsPrimitivesOut
 *     On exit, contains primitives for drawing the contours.
 * @param errorMessageOut
 *     Output containing the error message.
 * @return 
 *     True if successful, otherwise false.
 */
bool
SurfacePlaneIntersectionToContour::createContours(std::vector<GraphicsPrimitive*>& graphicsPrimitivesOut,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    try {
        if (m_surfaceFile->getNumberOfNodes() <= 2) {
            throw CaretException("Surface has an invalid number of vertices.");
        }
        if ( ! m_intersectionPlane.isValidPlane()) {
            throw CaretException("Intersection plane is invalid.");
        }
        if ( ! m_drawOnPlane.isValidPlane()) {
            throw CaretException("Draw on plane is invalid.");
        }
        
        m_topologyHelper = m_surfaceFile->getTopologyHelper(true);
        if (m_topologyHelper->getNumberOfNodes() <= 2) {
            throw CaretException("Toplogy helper has an invalid number of vertices.");
        }
        
        if (m_surfaceFile->getNumberOfNodes() != m_topologyHelper->getNumberOfNodes()) {
            throw CaretException("Surface File and its Topology contain a different number of vertices.");
        }
        static bool timingFlag = false;
        ElapsedTimer timer;
        if (timingFlag) {
            timer.start();
        }
        
        prepareVertices();
        const float verticesTime = (timingFlag ? timer.getElapsedTimeMilliseconds(): 0.0f);

        prepareEdges();
        const float edgesTime = (timingFlag ? timer.getElapsedTimeMilliseconds() - verticesTime : 0.0f);
        
        generateContours(graphicsPrimitivesOut);
        const float contoursTime = (timingFlag ? timer.getElapsedTimeMilliseconds() - edgesTime : 0.0f);
        
        if (timingFlag) {
            std::cout << "Total Time=" << timer.getElapsedTimeMilliseconds() << std::endl;
            std::cout << "   Vertices=" << verticesTime << std::endl;
            std::cout << "      Edges=" << edgesTime << std::endl;
            std::cout << "   Contours=" << contoursTime << std::endl;
        }
    }
    catch (const CaretException& caretException) {
        errorMessageOut = caretException.whatString();
        return false;
    }
    
    return true;
}

/**
 * Prepare the vertices by computing their signed distance from the plane.
 * If vertex is on of very, very close to the plane, move the vertex
 * away from the play by a very small amount so that the plane
 * always intersects edges and never at a vertex.
 */
void
SurfacePlaneIntersectionToContour::prepareVertices()
{
    const float epsilon = 0.0000001f;
    
    CaretAssert(m_surfaceFile);
    
    float planeNormalVector[3];
    m_intersectionPlane.getNormalVector(planeNormalVector);
    const float abovePlaneOffset[3] = { planeNormalVector[0] * epsilon, planeNormalVector[1] * epsilon, planeNormalVector[2] * epsilon };
    
    const float* surfaceXYZ = m_surfaceFile->getCoordinateData();
    
    const int32_t numberOfVertices = m_surfaceFile->getNumberOfNodes();
    
    m_vertices.resize(numberOfVertices);
    
#pragma omp CARET_PARFOR
    for (int32_t i = 0; i < numberOfVertices; i++) {
        const int32_t i3 = i * 3;
        std::array<float, 3> xyz = {{ surfaceXYZ[i3], surfaceXYZ[i3 + 1], surfaceXYZ[i3 + 2] }};
        
        const float signedDistanceToPlane = m_intersectionPlane.signedDistanceToPlane(xyz.data());
        if ((signedDistanceToPlane < epsilon)
            && (signedDistanceToPlane > -epsilon)) {
            /*
             * Point is on or nearly on the plane so move it away from the plane
             */
            float projectedXYZ[3];
            m_intersectionPlane.projectPointToPlane(xyz.data(), projectedXYZ);
            
            if (signedDistanceToPlane >= 0) {
                xyz[0] = projectedXYZ[0] + abovePlaneOffset[0];
                xyz[1] = projectedXYZ[1] + abovePlaneOffset[1];
                xyz[2] = projectedXYZ[2] + abovePlaneOffset[2];
            }
            else {
                xyz[0] = projectedXYZ[0] - abovePlaneOffset[0];
                xyz[1] = projectedXYZ[1] - abovePlaneOffset[1];
                xyz[2] = projectedXYZ[2] - abovePlaneOffset[2];
            }
        }
        
        m_vertices[i].reset(new Vertex(xyz, signedDistanceToPlane));
    }
    
    CaretAssert(static_cast<int32_t>(m_vertices.size()) == numberOfVertices);
}

/**
 * Find the edges that intersect the plane and save
 * information about these intersecting edges.
 */
void
SurfacePlaneIntersectionToContour::prepareEdges()
{
    const std::vector<TopologyEdgeInfo>& allEdgeInfo = m_topologyHelper->getEdgeInfo();
    
    const int32_t numEdges = static_cast<int32_t>(allEdgeInfo.size());
    
    m_topoHelperEdgeToIntersectingEdgeIndices.reserve(numEdges);
    
    for (int32_t i = 0; i < numEdges; i++) {
        CaretAssertVectorIndex(allEdgeInfo, i);
        const TopologyEdgeInfo& edgeInfo = allEdgeInfo[i];
        const int32_t indexOne = edgeInfo.node1;
        const int32_t indexTwo = edgeInfo.node2;
        CaretAssertVectorIndex(m_vertices, indexOne);
        CaretAssertVectorIndex(m_vertices, indexTwo);
        
        int32_t intersectingEdgeIndex = -1;
        
        /*
         * We are only interested in edges that have one vertex below the plane
         * and one vertex above the plane so the edge MUST intersect the plane.
         */
        if (m_vertices[indexOne]->m_abovePlaneFlag != m_vertices[indexTwo]->m_abovePlaneFlag) {
            const int32_t abovePlaneVertexIndex = (m_vertices[indexOne]->m_abovePlaneFlag
                                                   ? indexOne
                                                   : indexTwo);
            const int32_t belowPlaneVertexIndex = (m_vertices[indexOne]->m_abovePlaneFlag
                                                   ? indexTwo
                                                   : indexOne);
            
            std::array<float, 3> intersectionXYZ;
            const bool validFlag = m_intersectionPlane.lineSegmentIntersectPlane(m_vertices[belowPlaneVertexIndex]->m_xyz.data(),
                                                                     m_vertices[abovePlaneVertexIndex]->m_xyz.data(),
                                                                     intersectionXYZ.data());
            if (validFlag) {
                /*
                 * Get the edge's triangles
                 */
                if (edgeInfo.numTiles <= 0) {
                    const AString msg = ("Edge index="
                                         + AString::number(i)
                                         + " contains zero triangles.  This should never happend!");
                    throw CaretException(msg);
                }
                
                int32_t triangleOne(edgeInfo.tiles[0].tile);
                int32_t triangleTwo(-1);
                if (edgeInfo.numTiles > 1) {
                    triangleTwo = edgeInfo.tiles[1].tile;
                }
                
                std::unique_ptr<IntersectionEdge> edge(new IntersectionEdge(intersectionXYZ,
                                                    belowPlaneVertexIndex,
                                                    abovePlaneVertexIndex,
                                                    triangleOne,
                                                    triangleTwo));
                m_intersectingEdges.push_back(std::move(edge));
                
                intersectingEdgeIndex = m_intersectingEdges.size() - 1;
                
                m_numberOfIntersectingEdges++;
            }
        }
        
        m_topoHelperEdgeToIntersectingEdgeIndices.push_back(intersectingEdgeIndex);
    }
    
    CaretAssert(static_cast<int32_t>(m_topoHelperEdgeToIntersectingEdgeIndices.size()) == numEdges);
}

/**
 * Examine edges and find contours by moving from edge to neighboring triangle to edge.
 * In many instances, there will be more than one contour due to the way the 
 * cortex intersects the plane.
 *
 * @param graphicsPrimitivesOut
 *     On exit, contains primitives for drawing the contours.
 */
void
SurfacePlaneIntersectionToContour::generateContours(std::vector<GraphicsPrimitive*>& graphicsPrimitivesOut)
{
    if (m_debugFlag) {
        std::cout << "*********************************************" << std::endl;
    }
    
    for (auto& edge : m_intersectingEdges) {
        if ( ! edge->m_processedFlag) {
            GraphicsPrimitive* primitive = generateContourFromEdge(edge.get());
            if (primitive != NULL) {
                graphicsPrimitivesOut.push_back(primitive);
            }
        }
    }
}

/**
 * Starting with the given edge, follow the intersecting edges to create a contour.
 * The contour will end when it returns to the original edge or encounters an 
 * edge that is used by only one triangle.  An edge with one triangle indicates 
 * open topology such as a surface without a medial wall.
 *
 * @param startingEdge
 *     First edge in the contour.
 */
GraphicsPrimitive*
SurfacePlaneIntersectionToContour::generateContourFromEdge(IntersectionEdge* startingEdge)
{
    static std::array<float, 4> debugRgba      = {{ 1.0f, 1.0f, 0.0f, 1.0f }};
    static std::array<float, 4> startDebugRgba = {{ 0.0, 1.0, 0.0, 1.0 }};
    static std::array<float, 4> endDebugRgba   = {{ 1.0, 0.0, 0.0, 1.0 }};
    
    const std::vector<TopologyTileInfo>& allTileInfo = m_topologyHelper->getTileInfo();
    
    std::unique_ptr<GraphicsPrimitiveV3fC4f> primitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN));
    primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                            m_contourThicknessPercentOfViewportHeight);
    primitive->reserveForNumberOfVertices(m_numberOfIntersectingEdges + 5);
    
    IntersectionEdge* edge = startingEdge;
    
    if (m_debugFlag) {
        std::cout << "Start: " << startingEdge->toString() << std::endl;
    }
    
    while (edge != NULL) {
        edge->m_processedFlag = true;
        
        IntersectionEdge* nextEdge = NULL;
        
        for (int32_t iTile = 0; iTile < 2; iTile++) {
            CaretAssert(edge);
            const int32_t tileIndex = (iTile == 0) ? edge->m_triangleOneIndex : edge->m_triangleTwoIndex;
            if (tileIndex >= 0) {
                CaretAssertVectorIndex(allTileInfo, tileIndex);
                const TopologyTileInfo& tileInfo = allTileInfo[tileIndex];
                
                for (int32_t iEdge = 0; iEdge < 3; iEdge++) {
                    const int32_t tileEdgeIndex = tileInfo.edges[iEdge].edge;
                    const int32_t intersectingEdgeIndex = m_topoHelperEdgeToIntersectingEdgeIndices[tileEdgeIndex];
                    if (intersectingEdgeIndex >= 0) {
                        CaretAssertVectorIndex(m_intersectingEdges, intersectingEdgeIndex);
                        if ( ! m_intersectingEdges[intersectingEdgeIndex]->m_processedFlag) {
                            nextEdge = m_intersectingEdges[intersectingEdgeIndex].get();
                            break;
                        }
                    }
                }
                
                if (nextEdge != NULL) {
                    break;
                }
            }
        }
        
        std::array<float, 4> colorRGBA = m_solidRGBA;
        if (m_debugFlag) {
            colorRGBA = debugRgba;
            if (primitive->getNumberOfVertices() == 0) {
                colorRGBA = startDebugRgba;
            }
            if (nextEdge == NULL) {
                colorRGBA = endDebugRgba;
                if (m_debugFlag) {
                    std::cout << "    End: " << edge->toString() << std::endl;
                }
            }
        }
        else {
            if (m_caretColor == CaretColorEnum::CUSTOM) {
                CaretAssert(m_vertexColoringRGBA);
                const int32_t belowI4 = edge->m_belowPlaneVertexIndex * 4;
                const int32_t aboveI4 = edge->m_abovePlaneVertexIndex * 4;
                
                for (int32_t m = 0; m < 4; m++) {
                    colorRGBA[m] = ((m_vertexColoringRGBA[belowI4+m] + m_vertexColoringRGBA[aboveI4+m]) / 2.0f);
                }
            }
        }
        
        Vector3D xyzOne;
        m_drawOnPlane.projectPointToPlane(edge->m_intersectionXYZ.data(),
                                          xyzOne);
        primitive->addVertex(xyzOne,
                             colorRGBA.data());

        if (startingEdge->hasMatchingTriangle(edge)) {
            if (primitive->getNumberOfVertices() > 3) {
                Vector3D xyzTwo;
                m_drawOnPlane.projectPointToPlane(startingEdge->m_intersectionXYZ.data(),
                                                  xyzTwo);
                if (m_debugFlag) {
                    primitive->addVertex(xyzTwo,
                                         startDebugRgba.data());
                    std::cout << "    Closed contour" << std::endl;
                }
                else {
                    primitive->addVertex(xyzTwo,
                                         colorRGBA.data());
                }
            }
        }
        
        edge = nextEdge;
    }
    
    if (m_debugFlag) {
        std::cout << "    Contour vertex count: " << primitive->getNumberOfVertices() << std::endl;
    }
    
    if (primitive->isValid()) {
        return primitive.release();
    }
    
    return NULL;
}

