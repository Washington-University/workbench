
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __VOLUME_VERTICES_EDGES_FACES_DECLARE__
#include "VolumeVerticesEdgesFaces.h"
#undef __VOLUME_VERTICES_EDGES_FACES_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "VolumeMappableInterface.h"

using namespace caret;
    
/**
 * \class caret::VolumeVerticesEdgesFaces 
 * \brief Simplify access to a volumes vertices, edges, and faces
 * \ingroup Files
 */

/**
 * @return New instance for the given volume
 * @param volume
 *    The volume
 */
std::unique_ptr<VolumeVerticesEdgesFaces>
VolumeVerticesEdgesFaces::newInstance(const VolumeMappableInterface* volume)
{
    Matrix4x4 matrix;
    const bool matrixValidFlag(false);
    std::unique_ptr<VolumeVerticesEdgesFaces> ptr(new VolumeVerticesEdgesFaces(volume,
                                                                               matrix,
                                                                               matrixValidFlag));
    return ptr;
}

/**
 * @return New instance for the given volume with vertices transformed by the given matrix
 * @param volume
 *    The volume
 * @param matrix
 *    Matrix that transforms vertex coordinates
 */
std::unique_ptr<VolumeVerticesEdgesFaces>
VolumeVerticesEdgesFaces::newInstance(const VolumeMappableInterface* volume,
                                      const Matrix4x4& matrix)
{
    const bool matrixValidFlag(true);
    std::unique_ptr<VolumeVerticesEdgesFaces> ptr(new VolumeVerticesEdgesFaces(volume,
                                                                               matrix,
                                                                               matrixValidFlag));
    return ptr;
}

 
/**
 * Constructor.
 * @param volume
 *    The volume
 * @param matrix
 *    Matrix that transforms vertex coordinates
 * @param matrixValidFlag
 *    True if the matrix is valid, else false.
 */
VolumeVerticesEdgesFaces::VolumeVerticesEdgesFaces(const VolumeMappableInterface* volume,
                                                   const Matrix4x4& matrix,
                                                   const bool matrixValidFlag)
: CaretObject(),
m_volume(volume),
m_matrix(matrix),
m_matrixValidFlag(matrixValidFlag)
{

    CaretAssert(m_volume);
    if ((m_volume) == NULL) {
        CaretLogSevere("Volume is invalid (NULL)");
        return;
    }
    
    std::vector<int64_t> dims;
    m_volume->getDimensions(dims);
    if (dims.size() < 3) {
        CaretLogSevere("Volume dimensions are less than three");
        return;
    }
    if ((dims[0] < 2) || (dims[1] < 2) || (dims[2] < 2)) {
        CaretLogSevere("At least one dimension is less than two");
        return;
    }
    
    m_validFlag = true;
    
    const float maxI(dims[0] - 1);
    const float maxJ(dims[1] - 1);
    const float maxK(dims[2] - 1);
    
    /*
     * Coordinates at volume's 8 corners
     */
    m_xyz000 = m_volume->indexToSpace( 0.0,  0.0, 0.0);
    m_xyzI00 = m_volume->indexToSpace(maxI,  0.0, 0.0);
    m_xyzIJ0 = m_volume->indexToSpace(maxI, maxJ, 0.0);
    m_xyz0J0 = m_volume->indexToSpace( 0.0, maxJ, 0.0);
    
    m_xyz00K = m_volume->indexToSpace( 0.0,  0.0, maxK);
    m_xyzI0K = m_volume->indexToSpace(maxI,  0.0, maxK);
    m_xyzIJK = m_volume->indexToSpace(maxI, maxJ, maxK);
    m_xyz0JK = m_volume->indexToSpace( 0.0, maxJ, maxK);
    
    m_vertices.push_back(m_xyz000);
    m_vertices.push_back(m_xyzI00);
    m_vertices.push_back(m_xyzIJ0);
    m_vertices.push_back(m_xyz0J0);

    m_vertices.push_back(m_xyz00K);
    m_vertices.push_back(m_xyzI0K);
    m_vertices.push_back(m_xyzIJK);
    m_vertices.push_back(m_xyz0JK);
    
    /*
     * Transform the coordinates at the volume's corners
     */
    if (m_matrixValidFlag) {
        m_matrix.multiplyPoint3(m_xyz000);
        m_matrix.multiplyPoint3(m_xyzI00);
        m_matrix.multiplyPoint3(m_xyzIJ0);
        m_matrix.multiplyPoint3(m_xyz0J0);
        
        m_matrix.multiplyPoint3(m_xyz00K);
        m_matrix.multiplyPoint3(m_xyzI0K);
        m_matrix.multiplyPoint3(m_xyzIJK);
        m_matrix.multiplyPoint3(m_xyz0JK);
    }
    
    /*
     * Line segments for the 12 edges
     */
    m_edges.emplace_back(m_xyz000, m_xyzI00);
    m_edges.emplace_back(m_xyzI00, m_xyzIJ0);
    m_edges.emplace_back(m_xyzIJ0, m_xyz0J0);
    m_edges.emplace_back(m_xyz0J0, m_xyz000);
    
    m_edges.emplace_back(m_xyz00K, m_xyzI0K);
    m_edges.emplace_back(m_xyzI0K, m_xyzIJK);
    m_edges.emplace_back(m_xyzIJK, m_xyz0JK);
    m_edges.emplace_back(m_xyz0JK, m_xyz00K);
    
    m_edges.emplace_back(m_xyz000, m_xyz00K);
    m_edges.emplace_back(m_xyzI00, m_xyzI0K);
    m_edges.emplace_back(m_xyzIJ0, m_xyzIJK);
    m_edges.emplace_back(m_xyz0J0, m_xyz0JK);
    
    CaretAssert(m_edges.size() == 12);
}

/**
 * Destructor.
 */
VolumeVerticesEdgesFaces::~VolumeVerticesEdgesFaces()
{
}

/**
 * @return True if this instance is valid
 */
bool
VolumeVerticesEdgesFaces::isValid() const
{
    return m_validFlag;
}

/**
 * @return The vertices
 */
const std::vector<Vector3D>&
VolumeVerticesEdgesFaces::getVertices() const
{
    return m_vertices;
}

/**
 * @return The edges
 */
const std::vector<VolumeVerticesEdgesFaces::Edge>&
VolumeVerticesEdgesFaces::getEdges() const
{
    return m_edges;
}

/**
 * @return The faces
 */
const std::vector<VolumeVerticesEdgesFaces::Face>&
VolumeVerticesEdgesFaces::getFaces() const
{
    if ( ! m_facesValidFlag) {
        createFaces();
    }
    
    return m_faces;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeVerticesEdgesFaces::toString() const
{
    return "VolumeVerticesEdgesFaces";
}

/**
 * Create the faces of the volume
 */
void
VolumeVerticesEdgesFaces::createFaces() const
{
    if (m_facesValidFlag) {
        return;
    }
    m_facesValidFlag = true;
    
    /* Bottom */
    m_faces.emplace_back(m_xyz000, m_xyzI00, m_xyzIJ0, m_xyz0J0);
    
    /* Top */
    m_faces.emplace_back(m_xyz00K, m_xyzI0K, m_xyzIJK, m_xyz0JK);
    
    /* Left */
    m_faces.emplace_back(m_xyz000, m_xyz00K, m_xyz0JK, m_xyz0J0);
    
    /* Right */
    m_faces.emplace_back(m_xyzI00, m_xyzI0K, m_xyzIJK, m_xyzIJ0);
    
    /* Near */
    m_faces.emplace_back(m_xyz000, m_xyzI00, m_xyzI0K, m_xyz00K);
    
    /* Far */
    m_faces.emplace_back(m_xyz0J0, m_xyzIJ0, m_xyzIJK, m_xyz0JK);
    
    if (s_debugFlag) {
        for (const auto& f : m_faces) {
            std::cout << "Face: " << f.v1().toString() << std::endl;
            std::cout << "      " << f.v2().toString() << std::endl;
            std::cout << "      " << f.v3().toString() << std::endl;
            std::cout << "      " << f.v4().toString() << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
    
    CaretAssert(m_faces.size() == 6);
}
