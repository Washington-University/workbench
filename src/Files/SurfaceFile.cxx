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

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

#include <QThread>

#include "BoundingBox.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "SurfaceFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "DataFileContentInformation.h"
#include "DescriptiveStatistics.h"
#include "FastStatistics.h"
#include "EventSurfaceColoringInvalidate.h"
#include "GiftiFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "GraphicsPrimitiveV3fN3fC4f.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Vector3D.h"

#include "CaretPointLocator.h"
#include "GeodesicHelper.h"
#include "PlainTextStringBuilder.h"
#include "SignedDistanceHelper.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * Constructor.
 */
SurfaceFile::SurfaceFile()
: GiftiTypeFile(DataFileTypeEnum::SURFACE)
{
    m_skipSanityCheck = false;//NOTE: this is NOT in the initializeMembersSurfaceFile method, because that method gets used at the top of the validate function,
                              //which is used by setNumberOfNodesAndTriangles, which temporarily puts it the triangles into an invalid state, which is why this flag exists
    this->initializeMembersSurfaceFile();
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
SurfaceFile::SurfaceFile(const SurfaceFile& sf)
: GiftiTypeFile(sf), EventListenerInterface()
{
    m_skipSanityCheck = false;//see above
    this->initializeMembersSurfaceFile();
    this->copyHelperSurfaceFile(sf);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the SurfaceFile parameter.
 */
SurfaceFile& 
SurfaceFile::operator=(const SurfaceFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperSurfaceFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
SurfaceFile::~SurfaceFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    if (this->boundingBox != NULL) {
        delete this->boundingBox;
        this->boundingBox = NULL;
    }
    
    this->invalidateNodeColoringForBrowserTabs();
}

void SurfaceFile::writeFile(const AString& filename)
{
    if (!filename.endsWith(".surf.gii"))
    {
        CaretLogWarning("surface file '" + filename + "' should be saved ending in .surf.gii, see wb_command -gifti-help");
    }
    caret::GiftiTypeFile::writeFile(filename);
}

/**
 * Clear the surface file.
 */
void 
SurfaceFile::clear()
{
    if (this->boundingBox != NULL) {
        delete this->boundingBox;
        this->boundingBox = NULL;
    }
    coordinateDataArray = NULL;
    coordinatePointer = NULL;
    triangleDataArray = NULL;
    trianglePointer = NULL;
    GiftiTypeFile::clear();
    invalidateHelpers();
    this->invalidateNodeColoringForBrowserTabs();
}

/**
 * Runs topology helper creation in a thread
 */
class CreateTopologyHelperThread : public QThread
{
public:
    CreateTopologyHelperThread(SurfaceFile* surfaceFile) {
        this->surfaceFile = surfaceFile;
    }
    ~CreateTopologyHelperThread() {
        std::cout << "Delete topology helper for " << this->surfaceFile->getFileNameNoPath() << std::endl;
    }
    void run() {
        /*
         * NEED to prevent SurfaceFile destructor from completing until this is done
         * Perhaps before starting this thread, set a variable in SurfaceFile and 
         * then have this method clear the variable after topology helper is created.
         */
        this->surfaceFile->getTopologyHelper();
        
        this->deleteLater();
    }
    
    SurfaceFile* surfaceFile;
};

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
SurfaceFile::validateDataArraysAfterReading()
{
    this->initializeMembersSurfaceFile();
    
    int numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays != 2) {
        throw DataFileException(getFileName(),
                                "Number of data arrays MUST be two in a SurfaceFile.");
    }
    
    /*
     * Find the coordinate and topology data arrays.
     */
    for (int i = 0; i < numDataArrays; i++) {
        GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
            if (gda->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
                if (gda->getNumberOfDimensions() == 2) {
                    int64_t dim0 = gda->getDimension(0);
                    int64_t dim1 = gda->getDimension(1);
                    if ((dim0 > 0) && (dim1 == 3)) {
                        this->coordinateDataArray = gda;
                        this->coordinatePointer = gda->getDataPointerFloat();
                    }
                }
            }
        }
        else if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_TRIANGLE) {
            if (gda->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_INT32) {
                if (gda->getNumberOfDimensions() == 2) {
                    int64_t dim0 = gda->getDimension(0);
                    int64_t dim1 = gda->getDimension(1);
                    if ((dim0 > 0) && (dim1 == 3)) {
                        this->triangleDataArray = gda;
                        this->trianglePointer = gda->getDataPointerInt();
                    }
                }
            }
        }
        
    }
    if (this->coordinateDataArray == NULL) {
        throw DataFileException(getFileName(),
                                "Unable to find coordinate data array which "
                                " contains data type FLOAT32, Intent POINTSET, and two "
                                " dimensions with the second dimension set to three.  ");
    }
    if (this->triangleDataArray == NULL) {
        throw DataFileException(getFileName(),
                                "Unable to find topology data array which "
                                " contains data type INT32, Intent TRIANGLE, and two "
                                " dimensions with the second dimension set to three.");
    }
    const int32_t numNodes = this->getNumberOfNodes();
    if (!m_skipSanityCheck)
    {
        const int numTris = getNumberOfTriangles();//sanity check the triangle data array
        for (int i = 0; i < numTris; ++i)
        {
            const int32_t* thisTri = getTriangle(i);
            for (int j = 0; j < 3; ++j)
            {
                if (thisTri[j] < 0 || thisTri[j] >= numNodes)
                {
                    throw DataFileException(getFileName(),
                                            "Invalid vertex in triangle array: triangle " + AString::number(i) + ", vertex " + AString::number(thisTri[j]));
                }
                for (int k = j + 1; k < 3; ++k)
                {
                    if (thisTri[j] == thisTri[k])
                    {
                        throw DataFileException(getFileName(),
                                                "Vertex used twice in one triangle: triangle " + AString::number(i) + ", vertex " + AString::number(thisTri[j]));
                    }
                }
            }
        }
    }

    this->computeNormals(); //this will be garbage in the case that m_skipSanityCheck is used for, but we want it to be allocated to the right size, as multiple functions use it unchecked...

    /*
     * Apply the first transformation matrix that transforms to 
     * Talairach space.
     */
// Disable as FreeSurfer is inserting a matrix that causes issues in 
// HCP Pipeline
//    const AString talairachName = NiftiTransformEnum::toName(NiftiTransformEnum::NIFTI_XFORM_TALAIRACH);
//    for (int32_t im = 0; im < this->coordinateDataArray->getNumberOfMatrices(); im++) {
//        const Matrix4x4* m = this->coordinateDataArray->getMatrix(im);
//        if (m->getTransformedSpaceName() == talairachName) {
//            applyMatrix(*m);
//            break;
//        }
//    }

    /*
     * Create the topology helper in a thread so files dont' take
     * too long to read.
     */
//    (new CreateTopologyHelperThread(this))->start();
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
SurfaceFile::getNumberOfNodes() const
{
    if (this->coordinatePointer == NULL) {
        return 0;
    }
    CaretAssert(this->coordinateDataArray);
    return this->coordinateDataArray->getDimension(0);
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
SurfaceFile::getNumberOfColumns() const
{
    if (this->getNumberOfNodes() > 0) {
        return 1;
    }
    return 0;
}

void SurfaceFile::setNumberOfNodesAndTriangles(const int32_t& nodes, const int32_t& triangles)
{
    if (this->boundingBox != NULL) {
        delete this->boundingBox;
        this->boundingBox = NULL;
    }
    coordinateDataArray = NULL;
    coordinatePointer = NULL;
    triangleDataArray = NULL;
    trianglePointer = NULL;
    giftiFile->clearAndKeepMetadata();
    invalidateHelpers();
    this->invalidateNodeColoringForBrowserTabs();
    std::vector<int64_t> dims(2);
    dims[1] = 3;
    dims[0] = nodes;
    giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_POINTSET, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, dims, GiftiEncodingEnum::GZIP_BASE64_BINARY));
    dims[0] = triangles;
    giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_TRIANGLE, NiftiDataTypeEnum::NIFTI_TYPE_INT32, dims, GiftiEncodingEnum::GZIP_BASE64_BINARY));
    m_skipSanityCheck = true;
    validateDataArraysAfterReading();
    m_skipSanityCheck = false;
    setModified();
}

/**
 * Get a coordinate.
 *
 * @param
 *    nodeIndex of coordinate.
 *
 * @return
 *    Pointer to memory containing the XYZ coordinate.
 */
const float* 
SurfaceFile::getCoordinate(const int32_t nodeIndex) const
{
    CaretAssert(this->coordinatePointer);
    const int32_t offset = nodeIndex * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfNodes() * 3)));
    return &(this->coordinatePointer[offset]);    
}

/**
 * Get a coordinate.
 *
 * @param
 *    nodeIndex of coordinate.
 * @param xyzOut
 *    Will contain coordinate upon exit.
 */
void 
SurfaceFile::getCoordinate(const int32_t nodeIndex,
                           float xyzOut[3]) const
{
    CaretAssert(this->coordinatePointer);
    const int32_t offset = nodeIndex * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfNodes() * 3)));
    xyzOut[0] = this->coordinatePointer[offset];
    xyzOut[1] = this->coordinatePointer[offset+1];
    xyzOut[2] = this->coordinatePointer[offset+2];
}

const float* SurfaceFile::getCoordinateData() const
{
    CaretAssert(this->coordinatePointer);
    return this->coordinatePointer;
}

void 
SurfaceFile::setCoordinate(const int32_t nodeIndex,
                           const float xyzIn[3])
{
    setCoordinate(nodeIndex, xyzIn[0], xyzIn[1], xyzIn[2]);
}

void SurfaceFile::setCoordinate(const int32_t nodeIndex, const float xIn, const float yIn, const float zIn)
{
    CaretAssert(this->coordinatePointer);
    const int32_t offset = nodeIndex * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfNodes() * 3)));
    this->coordinatePointer[offset] = xIn;
    this->coordinatePointer[offset+1] = yIn;
    this->coordinatePointer[offset+2] = zIn;
    invalidateNormals();
    invalidateHelpers();
    setModified();
}

void 
SurfaceFile::setCoordinates(const float *coordinates)
{
    CaretAssert(this->coordinatePointer);
    
    memcpy(this->coordinatePointer, coordinates, 3 * sizeof(float) * getNumberOfNodes());    
    invalidateHelpers();
    invalidateNormals();
    //setModified();
}

/**
 * Get the number of triangles.
 *
 * @return
 *    Number of triangles.
 */
int 
SurfaceFile::getNumberOfTriangles() const
{
    if (this->trianglePointer == NULL) {
        return 0;
    }
    CaretAssert(this->triangleDataArray);
    return this->triangleDataArray->getDimension(0);
    
}

/**
 * Get a triangle.
 *
 * @param indx
 *    Index of triangle.
 *
 * @return
 *    Pointer to memory containing the three nodes
 *    in the triangle.
 */
const int32_t* 
SurfaceFile::getTriangle(const int32_t indx) const
{
    CaretAssert(this->trianglePointer != NULL);
    CaretAssert((indx >= 0) && (indx < this->getNumberOfTriangles()));
    const int32_t offset = indx * 3;
    return &(this->trianglePointer[offset]);
}

void SurfaceFile::setTriangle(const int32_t& index, const int32_t* nodes)
{
    setTriangle(index, nodes[0], nodes[1], nodes[2]);
}

void SurfaceFile::setTriangle(const int32_t& index, const int32_t& node1, const int32_t& node2, const int32_t& node3)
{
    CaretAssert(trianglePointer != NULL);
    CaretAssert((index >= 0) && (index < getNumberOfTriangles()));
    const int32_t offset = index * 3;
    trianglePointer[offset] = node1;
    trianglePointer[offset + 1] = node2;
    trianglePointer[offset + 2] = node3;
    invalidateHelpers();
    invalidateNormals();
    setModified();
}

/**
 * Find the triangle that has an edge formed by "n1" and "n2"
 * but its not "oppositeTriangle".
 * 
 * @param n1
 *    First node in the edge.
 * @param n2
 *    Second node in the edge.
 * @param oppositeTriangle
 *    Triangle that on opposite side of the edge.
 * @return
 *    Index of triangle or -1 if not found.  If not
 *    found surface must be open/cut.
 */
int32_t
SurfaceFile::getTriangleThatSharesEdge(const int32_t n1,
                                  const int32_t n2,
                                  const int32_t oppositeTriangle) const
{
    CaretPointer<TopologyHelper> topoHelp = getTopologyHelper();

    /*
     * Get the triangles used by one of the nodes
     */
    int32_t numTriangles = 0;
    const int32_t* triangles = topoHelp->getNodeTiles(n1, numTriangles);
    
    for (int32_t i = 0; i < numTriangles; i++) {
        const int32_t t = triangles[i];
        if (t != oppositeTriangle) {
            const int* nodes = this->getTriangle(t);
            if ((n1 == nodes[0])
                || (n1 == nodes[1])
                || (n1 == nodes[2])) {
                if ((n2 == nodes[0])
                    || (n2 == nodes[1])
                    || (n2 == nodes[2])) {
                    return t;
                }
            }
        }
    }
    
    return -1;
}


/**
 * Initialize members of this class.
 */
void 
SurfaceFile::initializeMembersSurfaceFile()
{
    this->coordinateDataArray = NULL;
    this->coordinatePointer   = NULL;
    this->triangleDataArray   = NULL;
    this->trianglePointer     = NULL;
    this->boundingBox         = NULL;
    m_distHelperIndex = 0;
    m_geoHelperIndex = 0;
    m_topoHelperIndex = 0;
    m_normalsComputed = false;
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
SurfaceFile::copyHelperSurfaceFile(const SurfaceFile& /*sf*/)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get a normal vector for a coordinate.
 *
 * @param
 *    Index of coordinate.
 *
 * @return
 *    Pointer to memory containing the normal vector.
 */
const float* 
SurfaceFile::getNormalVector(const int32_t nodeIndex) const
{
    const int32_t offset = nodeIndex * 3;
    CaretAssert((offset >= 0) && (offset < static_cast<int>(this->normalVectors.size())));
    return &(this->normalVectors[offset]);    
}

/**
 * Get a normal vector for a coordinate.
 *
 * @param nodeIndex
 *    Index of coordinate.
 * @param normalVectorOut
 *    Output containing the normal vector for the node.
 */
void
SurfaceFile::getNormalVector(const int32_t nodeIndex,
                             float normalVectorOut[3]) const
{
    const int32_t offset = nodeIndex * 3;
    CaretAssertVectorIndex(this->normalVectors, offset+2);
    normalVectorOut[0] = this->normalVectors[offset];
    normalVectorOut[1] = this->normalVectors[offset+1];
    normalVectorOut[2] = this->normalVectors[offset+2];
}

const float* SurfaceFile::getNormalData() const
{
    return normalVectors.data();
}

void
SurfaceFile::invalidateNormals()
{
    m_normalsComputed = false;
}
/**
 * Compute surface normals.
 */
void 
SurfaceFile::computeNormals()
{
    if (m_normalsComputed)//don't recompute when not needed
    {
        return;
    }
    m_normalsComputed = true;
    int32_t numCoords = this->getNumberOfNodes();
    if (numCoords > 0) {
        this->normalVectors.resize(numCoords * 3);

        /*
         * Need to fill with zeros so calculations are correct
         * especially when normal get recomputed (resize does nothing)
         */
        std::fill(this->normalVectors.begin(),
                  this->normalVectors.end(),
                  0.0);
    }
    else {
        this->normalVectors.clear();
    }
    
    const int32_t numTriangles = this->getNumberOfTriangles();
    if ((numCoords > 0) && (numTriangles > 0)) {
        float* normalPointer = &this->normalVectors[0];
        std::vector<int> numContribute(numCoords, 0);

        float triangleNormal[3];
        for (int32_t i = 0; i < numTriangles; i++) {
            const int32_t it3 = i * 3;
            const int n1 = this->trianglePointer[it3];
            const int n2 = this->trianglePointer[it3 + 1];
            const int n3 = this->trianglePointer[it3 + 2];
            const int32_t c1 = n1 * 3;
            const int32_t c2 = n2 * 3;
            const int32_t c3 = n3 * 3;
            if ((n1 >= 0)
                && (n2 >= 0)
                && (n3 >= 0)) {
                
                MathFunctions::normalVector(&this->coordinatePointer[c1],
                                            &this->coordinatePointer[c2],
                                            &this->coordinatePointer[c3],
                                            triangleNormal);
                
                normalPointer[c1 + 0] += triangleNormal[0];//+= is not guaranteed to be atomic, do not parallelize
                normalPointer[c1 + 1] += triangleNormal[1];
                normalPointer[c1 + 2] += triangleNormal[2];
                numContribute[n1] += 1;
                normalPointer[c2 + 0] += triangleNormal[0];
                normalPointer[c2 + 1] += triangleNormal[1];
                normalPointer[c2 + 2] += triangleNormal[2];
                numContribute[n2] += 1;
                normalPointer[c3 + 0] += triangleNormal[0];
                normalPointer[c3 + 1] += triangleNormal[1];
                normalPointer[c3 + 2] += triangleNormal[2];
                numContribute[n3] += 1;
            }
        }
        
        for (int i = 0; i < numCoords; i++) {
            const int i3 = i * 3;
            if (numContribute[i] > 0) {
                MathFunctions::normalizeVector(normalPointer + i3);
            } else {
                normalPointer[i3 + 0] = 0.0f;//zero the normals for unconnected nodes
                normalPointer[i3 + 1] = 0.0f;
                normalPointer[i3 + 2] = 0.0f;
            }
        }
    }
}

std::vector<float> SurfaceFile::computeAverageNormals()
{
    computeNormals();
    const float* normalPointer = getNormalData();
    int numCoords = getNumberOfNodes();
    std::vector<float> ret(numCoords * 3);
    CaretPointer<TopologyHelper> myTopoHelp = getTopologyHelper();//TODO: make this not circular - separate base that doesn't handle helpers (and is used by helpers) from file that handles helpers and normals?
    for (int i = 0; i < numCoords; ++i)
    {
        int i3 = i * 3;
        Vector3D accum;
        const std::vector<int32_t>& neighbors = myTopoHelp->getNodeNeighbors(i);
        int numNeigh = (int)neighbors.size();
        for (int j = 0; j < numNeigh; ++j)
        {
            accum += normalPointer + neighbors[j] * 3;
        }
        Vector3D outVec = accum.normal();
        ret[i3] = outVec[0];
        ret[i3 + 1] = outVec[1];
        ret[i3 + 2] = outVec[2];
    }
    return ret;
}

/**
 * Get the normal vector for a triangle.
 * @param triangleIndex
 *    Index of the triangle.
 * @param normalOut
 *    Output containing the normal for the triangle.
 */
void
SurfaceFile::getTriangleNormalVector(const int32_t triangleIndex,
                                     float normalOut[3]) const
{
    const int32_t it3 = triangleIndex * 3;
    const int n1 = this->trianglePointer[it3];
    const int n2 = this->trianglePointer[it3 + 1];
    const int n3 = this->trianglePointer[it3 + 2];
    const int32_t c1 = n1 * 3;
    const int32_t c2 = n2 * 3;
    const int32_t c3 = n3 * 3;
    MathFunctions::normalVector(&this->coordinatePointer[c1],
                                &this->coordinatePointer[c2],
                                &this->coordinatePointer[c3],
                                normalOut);
}

/**
 * @return The type of this surface.
 */
SurfaceTypeEnum::Enum 
SurfaceFile::getSurfaceType() const
{
    if (this->coordinateDataArray == NULL) {
        return SurfaceTypeEnum::UNKNOWN;
    }
    
    const AString geometricTypeName = 
        this->coordinateDataArray->getMetaData()->get(GiftiMetaDataXmlElements::METADATA_NAME_GEOMETRIC_TYPE);
    SurfaceTypeEnum::Enum surfaceType = SurfaceTypeEnum::fromGiftiName(geometricTypeName, NULL);
    return surfaceType;
}

/**
 * Sets the type of this surface.
 * @param surfaceType 
 *    New type for surface.
 */
void 
SurfaceFile::setSurfaceType(const SurfaceTypeEnum::Enum surfaceType)
{
    if (this->coordinateDataArray == NULL) {
        return;
    }
    const AString geometricTypeName = SurfaceTypeEnum::toGiftiName(surfaceType);
    this->coordinateDataArray->getMetaData()->set(GiftiMetaDataXmlElements::METADATA_NAME_GEOMETRIC_TYPE,
                                        geometricTypeName);
}

/**
 * @return The secondary type of this surface.
 */
SecondarySurfaceTypeEnum::Enum 
SurfaceFile::getSecondaryType() const
{
    if (this->coordinateDataArray == NULL) {
        return SecondarySurfaceTypeEnum::INVALID;
    }
    
    const AString secondaryTypeName = 
        this->coordinateDataArray->getMetaData()->get(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY);
    SecondarySurfaceTypeEnum::Enum surfaceType = SecondarySurfaceTypeEnum::fromGiftiName(secondaryTypeName, NULL);
    return surfaceType;
}

/**
 * Sets the type of this surface.
 * @param surfaceType 
 *    New type for surface.
 */
void 
SurfaceFile::setSecondaryType(const SecondarySurfaceTypeEnum::Enum secondaryType)
{
    if (this->coordinateDataArray == NULL) {
        return;
    }
    const AString secondaryTypeName = SecondarySurfaceTypeEnum::toGiftiName(secondaryType);
    this->coordinateDataArray->getMetaData()->set(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY,
                                        secondaryTypeName);
}

void SurfaceFile::getGeodesicHelper(CaretPointer<GeodesicHelper>& helpOut) const
{
    {//lock before modifying member (base)
        CaretMutexLocker myLock(&m_geoHelperMutex);
        if (m_geoBase == NULL)
        {
            m_geoHelpers.clear();//just to be sure
            m_geoHelperIndex = 0;
            m_geoBase.grabNew(new GeodesicHelperBase(this));//yes, this takes some time, and is single threaded at the moment
        }//keep locked while searching
        int32_t& myIndex = m_geoHelperIndex;
        int32_t myEnd = m_geoHelpers.size();
        for (int32_t i = 0; i < myEnd; ++i)
        {
            if (myIndex >= myEnd) myIndex = 0;
            if (m_geoHelpers[myIndex].getReferenceCount() == 1)//1 reference: in this class, so unused elsewhere
            {
                helpOut = m_geoHelpers[myIndex];
                ++myIndex;
                return;
            }
            ++myIndex;
        }
    }//UNLOCK before building a new one, so they can be built in parallel - this actually just involves initializing the marked array
    CaretPointer<GeodesicHelper> ret(new GeodesicHelper(m_geoBase));
    CaretMutexLocker myLock(&m_geoHelperMutex);//relock before modifying the array
    m_geoHelpers.push_back(ret);
    helpOut = ret;
}

CaretPointer<GeodesicHelper> SurfaceFile::getGeodesicHelper() const
{//this convenience function is here because in order to guarantee thread safety, the real function explicitly copies to a reference argument before letting the mutex unlock
    CaretPointer<GeodesicHelper> ret;//the copy of a return should take place before destructors (including the locker for the helper mutex), but just to be safe
    getGeodesicHelper(ret);//this call is therefore thread safe and guaranteed to modify reference count before it unlocks the mutex for helpers
    return ret;//so we are already safe by here, at the expense of a second copy constructor/operator= of a CaretPointer
}

void SurfaceFile::getTopologyHelper(CaretPointer<TopologyHelper>& helpOut, bool infoSorted) const
{
    {
        CaretMutexLocker myLock(&m_topoHelperMutex);//lock before searching with the shared index
        int32_t& myIndex = m_topoHelperIndex;
        int32_t myEnd = m_topoHelpers.size();
        for (int32_t i = 0; i < myEnd; ++i)
        {
            if (myIndex >= myEnd) myIndex = 0;
            if (m_topoHelpers[myIndex].getReferenceCount() == 1 && (!infoSorted || m_topoHelpers[myIndex]->isNodeInfoSorted()))//1 reference: in this class, so unused elsewhere
            {
                helpOut = m_topoHelpers[myIndex];//NOTE: can give sorted info to something that doesn't ask for sorted, if it already exists
                ++myIndex;
                return;
            }
            ++myIndex;
        }
        if (m_topoBase == NULL || (infoSorted && !m_topoBase->isNodeInfoSorted()))
        {
            m_topoBase.grabNew(new TopologyHelperBase(this, infoSorted));
        }
    }
    CaretPointer<TopologyHelper> ret(new TopologyHelper(m_topoBase));
    CaretMutexLocker myLock(&m_topoHelperMutex);//lock before modifying the array
    m_topoHelpers.push_back(ret);
    helpOut = ret;
}

CaretPointer<TopologyHelper> SurfaceFile::getTopologyHelper(bool infoSorted) const
{//see convenience function for geo helper for explanation
    CaretPointer<TopologyHelper> ret;
    getTopologyHelper(ret, infoSorted);
    return ret;
}

void SurfaceFile::getSignedDistanceHelper(CaretPointer<SignedDistanceHelper>& helpOut) const
{
    {
        CaretMutexLocker myLock(&m_distHelperMutex);//lock before searching with the shared index
        int32_t& myIndex = m_distHelperIndex;
        int32_t myEnd = m_distHelpers.size();
        for (int32_t i = 0; i < myEnd; ++i)
        {
            if (myIndex >= myEnd) myIndex = 0;
            if (m_distHelpers[myIndex].getReferenceCount() == 1)//1 reference: in this class, so unused elsewhere
            {
                helpOut = m_distHelpers[myIndex];
                ++myIndex;
                return;
            }
            ++myIndex;
        }
        if (m_distBase == NULL)
        {
            m_distBase.grabNew(new SignedDistanceHelperBase(this));
        }
    }
    CaretPointer<SignedDistanceHelper> ret(new SignedDistanceHelper(m_distBase));
    CaretMutexLocker myLock(&m_distHelperMutex);//lock before modifying the array
    m_distHelpers.push_back(ret);
    helpOut = ret;
}

CaretPointer<SignedDistanceHelper> SurfaceFile::getSignedDistanceHelper() const
{//see convenience function for geo helper for explanation
    CaretPointer<SignedDistanceHelper> ret;
    getSignedDistanceHelper(ret);
    return ret;
}

void SurfaceFile::invalidateHelpers()
{
    if (m_geoBase != NULL)
    {
        CaretMutexLocker myLock(&m_geoHelperMutex);//make this function threadsafe
        m_geoHelperIndex = 0;
        m_geoHelpers.clear();//CaretPointers make this nice, if they are still in use elsewhere, they don't vanish, even though this class is supposed to "control" them to some extent
        m_geoBase.grabNew(NULL);
    }
    if (m_topoBase != NULL)
    {
        CaretMutexLocker myLock2(&m_topoHelperMutex);
        m_topoHelperIndex = 0;
        m_topoHelpers.clear();
        m_topoBase.grabNew(NULL);
    }
    if (m_distBase != NULL)
    {
        CaretMutexLocker myLock4(&m_distHelperMutex);
        m_distHelperIndex = 0;
        m_distHelpers.clear();
        m_distBase.grabNew(NULL);
    }
    if (m_locator != NULL)
    {
        CaretMutexLocker myLock3(&m_locatorMutex);
        m_locator.grabNew(NULL);
    }
}

/**
 * @return A bounding box for this surface.
 */
const BoundingBox* 
SurfaceFile::getBoundingBox() const
{
    if (this->boundingBox == NULL) {
        this->boundingBox = new BoundingBox();
        
        /*
         * For bounding box, must make sure each node is connected.
         */
        CaretPointer<TopologyHelper> th = this->getTopologyHelper();
        const int32_t numberOfNodes = this->getNumberOfNodes();
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (th->getNodeHasNeighbors(i)) {
                this->boundingBox->update(&this->coordinatePointer[i*3]);
            }
        }
    }
    return this->boundingBox;
}

/**
 * Match the surface to the given anatomical surface.  If this
 * surface is an anatomical or raw surface, no action is taken.
 *
 * @param anatomicalSurfaceFile
 *     Match to this anatomical surface file.
 * @param matchStatus
 *     The match status
 */
void
SurfaceFile::matchToAnatomicalSurface(const SurfaceFile* anatomicalSurfaceFile,
                                      const bool matchStatus)
{
    CaretAssert(anatomicalSurfaceFile);
    if (anatomicalSurfaceFile == NULL) {
        return;
    }
    if (this == anatomicalSurfaceFile) {
        return;
    }
    
    if (matchStatus) {
        bool sphereMatchFlag(false);
        bool matchFlag(false);
        switch (getSurfaceType()) {
            case SurfaceTypeEnum::ANATOMICAL:
                break;
            case SurfaceTypeEnum::ELLIPSOID:
                break;
            case SurfaceTypeEnum::FLAT:
                matchFlag = true;
                break;
            case SurfaceTypeEnum::HULL:
                break;
            case SurfaceTypeEnum::INFLATED:
                matchFlag = true;
                break;
            case SurfaceTypeEnum::RECONSTRUCTION:
                break;
            case SurfaceTypeEnum::SEMI_SPHERICAL:
                break;
            case SurfaceTypeEnum::SPHERICAL:
                sphereMatchFlag = true;
                break;
            case SurfaceTypeEnum::UNKNOWN:
                break;
            case SurfaceTypeEnum::VERY_INFLATED:
                matchFlag = true;
                break;
        }
        
        if (matchFlag
            || sphereMatchFlag) {
            /*
             * Save the unmatched coordinates
             */
            const int32_t numXYZ = getNumberOfNodes() * 3;
            m_unmatchedCoordinates.resize(numXYZ);
            CaretAssert(this->coordinatePointer);
            std::copy_n(this->coordinatePointer,
                        numXYZ,
                        m_unmatchedCoordinates.begin());
            
            const bool modStatus(isModified());
            if (sphereMatchFlag) {
                matchSphereToSurface(anatomicalSurfaceFile);
                if ( ! modStatus) {
                    clearModified();
                }
            }
            else if (matchFlag) {
                matchSurfaceBoundingBox(anatomicalSurfaceFile);
                if ( ! modStatus) {
                    clearModified();
                }
            }
        }
    }
    else {
        /*
         * Restore the unmatched coordinates
         */
        const int32_t numXYZ = getNumberOfNodes() * 3;
        if (static_cast<int32_t>(m_unmatchedCoordinates.size()) == numXYZ) {
            CaretAssert(this->coordinatePointer);
            std::copy_n(m_unmatchedCoordinates.begin(),
                        numXYZ,
                        this->coordinatePointer);
        }
        m_unmatchedCoordinates.clear();
    }
}


/**
 * Match a sphere to the given surface while retaining the spherical
 * shape.  The center of gravity of the sphere will be the same as the center of gravity of
 * the match surface and the radius of the sphere will be the
 * distance furthest from the origin in the match surface.
 *
 * @param matchSurfaceFile
 *     Match to this surface file.
 */
void
SurfaceFile::matchSphereToSurface(const SurfaceFile* matchSurfaceFile)
{
    CaretAssert(matchSurfaceFile);
    
    const float oldRadius = getSphericalRadius();
    if (oldRadius <= 0.0) {
        CaretLogWarning("Match sphere has invalid radius");
        return;
    }

    /*
     * Find match surface vertex distance furthest from origin (0,0,0)
     * This value will become the radius of the sphere
     */
    CaretPointer<TopologyHelper> matchTH = matchSurfaceFile->getTopologyHelper();
    const int32_t numMatchVertices = matchSurfaceFile->getNumberOfNodes();
    float newRadius = 0.0;
    for (int32_t i = 0; i < numMatchVertices; i++) {
        if (matchTH->getNodeHasNeighbors(i)) {
            const float* xyz = matchSurfaceFile->getCoordinate(i);
            const float dist = ((xyz[0]*xyz[0])
                                 + (xyz[1]*xyz[1])
                                + (xyz[2]*xyz[2]));
            if (dist > newRadius) {
                newRadius = dist;
            }
        }
    }
    newRadius = std::sqrt(newRadius);
    
    float myCOG[3];
    getCenterOfGravity(myCOG);
    
    float matchCOG[3];
    matchSurfaceFile->getCenterOfGravity(matchCOG);
    
    
    Matrix4x4 matrix;
    
    /*
     * Scale to new radius
     */
    const float scale = newRadius / oldRadius;
    matrix.scale(scale,
                 scale,
                 scale);

    applyMatrix(matrix);
}

/**
 * Get the center of gravity (average coordinate) of the surface.
 *
 * param cogOut
 *     Output containing center of gravity.
 */
void
SurfaceFile::getCenterOfGravity(float cogOut[3]) const
{
    cogOut[0] = 0.0;
    cogOut[1] = 0.0;
    cogOut[2] = 0.0;
    
    const int32_t numberOfNodes = getNumberOfNodes();
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    
    double numberOfNodesWithNeighbors = 0.0;
    
    double cx(0.0), cy(0.0), cz(0.0);
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (th->getNodeHasNeighbors(i)) {
            const float* xyz = getCoordinate(i);
            
            cx += xyz[0];
            cy += xyz[1];
            cz += xyz[2];
            numberOfNodesWithNeighbors += 1.0;
        }
    }
    
    if (numberOfNodesWithNeighbors > 0.0) {
        cogOut[0] = cx / numberOfNodesWithNeighbors;
        cogOut[1] = cy / numberOfNodesWithNeighbors;
        cogOut[2] = cz / numberOfNodesWithNeighbors;
    }
}

/**
 * Match this surface to the given surface.  That is, after this
 * method is called, this surface and the given surface will
 * fit within the same bounding box.
 * @param surfaceFile
 *     Match to this surface file.
 */
void
SurfaceFile::matchSurfaceBoundingBox(const SurfaceFile* surfaceFile)
{
    CaretAssert(surfaceFile);
    
    const BoundingBox* targetBoundingBox = surfaceFile->getBoundingBox();
    const BoundingBox* myBoundingBox = getBoundingBox();
    
    Matrix4x4 matrix;
    
    /*
     * Translate min x/y/z to origin
     */
    matrix.translate(-myBoundingBox->getMinX(),
                     -myBoundingBox->getMinY(),
                     -myBoundingBox->getMinZ());
    
    /*
     * Scale to match size of match surface
     */
    float scaleX = (targetBoundingBox->getDifferenceX()
                    / myBoundingBox->getDifferenceX());
    float scaleY = (targetBoundingBox->getDifferenceY()
                    / myBoundingBox->getDifferenceY());
    float scaleZ = (targetBoundingBox->getDifferenceZ()
                    / myBoundingBox->getDifferenceZ());
    
    if (getSurfaceType() == SurfaceTypeEnum::FLAT) {
        /*
         * For a flat surface, need to retain overall shape.
         * Thus, just one scale factor that matches flat X-range
         * to 3D Y-range.
         */
        const float scale = (targetBoundingBox->getDifferenceY()
                             / myBoundingBox->getDifferenceX());
        scaleX = scale;
        scaleY = scale;
        scaleZ = scale;
    }
    
    matrix.scale(scaleX,
                 scaleY,
                 scaleZ);
    
    /*
     * Translate to min x/y/z of match surface so that
     * the two surfaces are now within the same "shoebox".
     */
    matrix.translate(targetBoundingBox->getMinX(),
                     targetBoundingBox->getMinY(),
                     targetBoundingBox->getMinZ());
    
    applyMatrix(matrix);
}

/**
 * Apply the given matrix to the coordinates of this surface.
 *
 * @param matrix
 *    Transformation matrix that is used.
 */
void
SurfaceFile::applyMatrix(const Matrix4x4& matrix)
{
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    
    const int32_t numberOfNodes = getNumberOfNodes();
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (th->getNodeHasNeighbors(i)) {
            matrix.multiplyPoint3(&coordinatePointer[i*3]);
        }
    }
    
    computeNormals();
    
    setModified();
}

/**
 * Translate to the surface center of mass.
 */
void
SurfaceFile::translateToCenterOfMass()
{
    const int32_t numberOfNodes = getNumberOfNodes();
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    
    double cx = 0.0;
    double cy = 0.0;
    double cz = 0.0;
    double numberOfNodesWithNeighbors = 0.0;
    
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (th->getNodeHasNeighbors(i)) {
            const float* xyz = getCoordinate(i);
            
            cx += xyz[0];
            cy += xyz[1];
            cz += xyz[2];
            numberOfNodesWithNeighbors += 1.0;
        }
    }
    
    if (numberOfNodesWithNeighbors > 0.0) {
        cx /= numberOfNodesWithNeighbors;
        cy /= numberOfNodesWithNeighbors;
        cz /= numberOfNodesWithNeighbors;
        
        Matrix4x4 matrix;
        matrix.setTranslation(-cx, -cy, -cz);
        applyMatrix(matrix);
    }
}

/**
 * Flip the normal vectors.
 */
void
SurfaceFile::flipNormals()
{
    if (trianglePointer == NULL) return;
    const int numTiles = getNumberOfTriangles();
    int32_t tempvert;
    for (int i = 0; i < numTiles; ++i)//swap first and second verts of all triangles
    {
        int offset = i * 3;
        tempvert = trianglePointer[offset];
        trianglePointer[offset] = trianglePointer[offset + 1];
        trianglePointer[offset + 1] = tempvert;
    }
    invalidateNormals();
    invalidateHelpers();//sorted topology helpers would change, so just for completeness
    setModified();
}

/**
 * @return True if normal vectors are correct, else false.
 *
 * Find the node with the greatest Z-coordinate.  If the Z-component
 * of the this node's normal vector is positive, then the normal vectors
 * point out of the surface and they are correct.  Otherwise, the normal
 * vectors are pointing into the surface.
 */
bool
SurfaceFile::areNormalVectorsCorrect() const
{
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    
    float maxZ = -std::numeric_limits<float>::max();
    int32_t indxMaxZ = -1;
    
    const int32_t numberOfNodes = getNumberOfNodes();
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (th->getNodeHasNeighbors(i)) {
            const float* xyz = getCoordinate(i);
            
            if (xyz[2] > maxZ) {
                maxZ     = xyz[2];
                indxMaxZ = i;
            }
        }
    }
    
    if (indxMaxZ >= 0) {
        const float* normal = getNormalVector(indxMaxZ);
        if (normal[2] > 0.0) {
            return true;
        }
    }
    
    return false;
}


/**
 * @return The radius of the spherical surface.
 *    Surface is assumed spherical.
 */
float
SurfaceFile::getSphericalRadius() const
{
    const BoundingBox* bb = getBoundingBox();
    const float radius = bb->getMaxX() - bb->getCenterX();
    return radius;
}

/**
 * @return Area of the surface.
 */
float
SurfaceFile::getSurfaceArea() const
{
    float areaOut = 0.0;
    
    CaretAssert(this->trianglePointer);
    const int32_t numberOfTriangles = getNumberOfTriangles();
    for (int32_t i = 0; i < numberOfTriangles; ++i) {
        const int32_t* triangleNodeIndices = getTriangle(i);
        const float* node1 = getCoordinate(triangleNodeIndices[0]);
        const float* node2 = getCoordinate(triangleNodeIndices[1]);
        const float* node3 = getCoordinate(triangleNodeIndices[2]);
        areaOut += MathFunctions::triangleArea(node1,
                                               node2,
                                               node3);
    }
    
    return areaOut;
}



void SurfaceFile::computeNodeAreas(std::vector<float>& areasOut) const
{
    CaretAssert(this->trianglePointer);
    int32_t triEnd = getNumberOfTriangles();
    int32_t numNodes = getNumberOfNodes();
    areasOut.resize(numNodes);
    for (int32_t i = 0; i < numNodes; ++i)
    {
        areasOut[i] = 0.0f;
    }
    for (int32_t i = 0; i < triEnd; ++i)
    {
        const int32_t* thisTri = getTriangle(i);
        const float* node1 = getCoordinate(thisTri[0]);
        const float* node2 = getCoordinate(thisTri[1]);
        const float* node3 = getCoordinate(thisTri[2]);
        float area3 = MathFunctions::triangleArea(node1, node2, node3) / 3.0f;
        areasOut[thisTri[0]] += area3;
        areasOut[thisTri[1]] += area3;
        areasOut[thisTri[2]] += area3;
    }
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
void 
SurfaceFile::setModified()
{
    if (this->boundingBox != NULL) {
        delete this->boundingBox;
        this->boundingBox = NULL;
    }
    
    GiftiTypeFile::setModified();
}

int32_t SurfaceFile::closestNode(const float target[3], const float maxDist) const
{
    if (maxDist > 0.0f)
    {
        return getPointLocator()->closestPointLimited(target, maxDist);
    } else {
        return getPointLocator()->closestPoint(target);
    }
}

CaretPointer<const CaretPointLocator> SurfaceFile::getPointLocator() const
{
    if (m_locator == NULL)//try to avoid locking even once
    {
        CaretMutexLocker myLock(&m_locatorMutex);
        if (m_locator == NULL)//test again AFTER lock to avoid race conditions
        {
            m_locator.grabNew(new CaretPointLocator(getCoordinateData(), getNumberOfNodes()));
        }
    }
    return m_locator;
}

void SurfaceFile::clearCachedHelpers() const
{
    {
        CaretMutexLocker locked(&m_topoHelperMutex);
        m_topoHelperIndex = 0;
        m_topoHelpers.clear();
        m_topoBase.grabNew(NULL);
    }
    {
        CaretMutexLocker locked(&m_geoHelperMutex);
        m_geoHelperIndex = 0;
        m_geoHelpers.clear();
        m_geoBase.grabNew(NULL);
    }
    {
        CaretMutexLocker locked(&m_distHelperMutex);
        m_distHelperIndex = 0;
        m_distHelpers.clear();
        m_distBase.grabNew(NULL);
    }
    {
        CaretMutexLocker locked(&m_locatorMutex);
        m_locator.grabNew(NULL);
    }
}

/**
 * @return Information about the surface.
 */
AString 
SurfaceFile::getInformation() const
{
    AString txt;
    
    txt += ("Name: "
            + this->getFileNameNoPath()
            + "\n");
    txt += ("Type: "
            + SurfaceTypeEnum::toGuiName(this->getSurfaceType())
            + "\n");
    const int32_t numberOfNodes = this->getNumberOfNodes();
    txt += ("Number of Vertices: "
            + AString::number(numberOfNodes)
            + "\n");
    txt += ("Number of Triangles: "
            + AString::number(this->getNumberOfTriangles())
            + "\n");
    txt += ("Bounds: ("
            + AString::fromNumbers(this->getBoundingBox()->getBounds(), 6, ", ")
            + ")\n");
    
    if (numberOfNodes > 0) {
//        std::vector<float> nodeSpacing;
//        nodeSpacing.reserve(numberOfNodes * 10);
//        CaretPointer<TopologyHelper> th = this->getTopologyHelper();
//        int numberOfNeighbors;
//        for (int32_t i = 0; i < numberOfNodes; i++) {
//            const int* neighbors = th->getNodeNeighbors(i, numberOfNeighbors);
//            for (int32_t j = 0; j < numberOfNeighbors; j++) {
//                const int n = neighbors[j];
//                if (n > i) {
//                    const float dist = MathFunctions::distance3D(this->getCoordinate(i),
//                                                                 this->getCoordinate(n));
//                    nodeSpacing.push_back(dist);
//                }
//            }
//        }
//        
//        DescriptiveStatistics stats;
//        stats.update(nodeSpacing);

        DescriptiveStatistics stats;
        getNodesSpacingStatistics(stats);
        const float mean = stats.getMean();
        const float stdDev = stats.getStandardDeviationSample();
        const float minValue = stats.getMinimumValue();
        const float maxValue = stats.getMaximumValue();
        
        txt += ("Spacing:\n");
        txt += ("    Mean: "
                + AString::number(mean, 'f', 6)
                + "\n");
        txt += ("    Std Dev: "
                + AString::number(stdDev, 'f', 6)
                + "\n");
        txt += ("    Minimum: "
                + AString::number(minValue, 'f', 6)
                + "\n");
        txt += ("    Maximum: "
                + AString::number(maxValue, 'f', 6)
                + "\n");
    }
    
    return txt;
}

/**
 * Get statistics on node spacing.
 * @param statsOut
 *    Upon exit, contains node spacing descriptive statistics.
 */
void
SurfaceFile::getNodesSpacingStatistics(DescriptiveStatistics& statsOut) const
{
    const int32_t numberOfNodes = this->getNumberOfNodes();
    std::vector<float> nodeSpacing;
    nodeSpacing.reserve(numberOfNodes * 10);
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    int numberOfNeighbors;
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int* neighbors = th->getNodeNeighbors(i, numberOfNeighbors);
        for (int32_t j = 0; j < numberOfNeighbors; j++) {
            const int n = neighbors[j];
            if (n > i) {
                const float dist = MathFunctions::distance3D(this->getCoordinate(i),
                                                             this->getCoordinate(n));
                nodeSpacing.push_back(dist);
            }
        }
    }
    
    statsOut.update(nodeSpacing);

}

void
SurfaceFile::getNodesSpacingStatistics(FastStatistics& statsOut) const
{
    const int32_t numberOfNodes = this->getNumberOfNodes();
    std::vector<float> nodeSpacing;
    nodeSpacing.reserve(numberOfNodes * 10);
    CaretPointer<TopologyHelper> th = this->getTopologyHelper();
    int numberOfNeighbors;
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int* neighbors = th->getNodeNeighbors(i, numberOfNeighbors);
        for (int32_t j = 0; j < numberOfNeighbors; j++) {
            const int n = neighbors[j];
            if (n > i) {
                const float dist = MathFunctions::distance3D(this->getCoordinate(i),
                                                             this->getCoordinate(n));
                nodeSpacing.push_back(dist);
            }
        }
    }
    
    statsOut.update(nodeSpacing.data(), nodeSpacing.size());

}

/**
 * Invalidate surface coloring.
 */
void
SurfaceFile::invalidateNodeColoringForBrowserTabs()
{
    /*
     * Free memory since could have many tabs and many surfaces equals lots of memory
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->surfaceNodeColoringForBrowserTabs[i].clear();
        this->surfaceMontageNodeColoringForBrowserTabs[i].clear();
        this->wholeBrainNodeColoringForBrowserTabs[i].clear();
    }
    
    m_surfaceGraphicsPrimitives.clear();
    m_surfaceMontageGraphicsPrimitives.clear();
    m_wholeBrainGraphicsPrimitives.clear();
}

/**
 * Allocate node coloring for a single surface in a browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param zeroizeColorsFlag
 *    If true and memory is allocated for colors, the color components
 *    are set to all zeros, otherwise the memory may be left unitialized.
 */
void 
SurfaceFile::allocateSurfaceNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                               const bool zeroizeColorsFlag)
{
    CaretAssertArrayIndex(this->surfaceNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    const uint64_t numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    if (this->surfaceNodeColoringForBrowserTabs[browserTabIndex].size() != numberOfComponentsRGBA) {
        if (zeroizeColorsFlag) {
            this->surfaceNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA, 0.0);
        }
        else {
            this->surfaceNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA);
        }
    }
}

/**
 * Allocate node coloring for a surface montage in a browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param zeroizeColorsFlag
 *    If true and memory is allocated for colors, the color components
 *    are set to all zeros, otherwise the memory may be left unitialized.
 */
void 
SurfaceFile::allocateSurfaceMontageNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                                             const bool zeroizeColorsFlag)
{
    CaretAssertArrayIndex(this->surfaceMontageNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    const uint64_t numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    if (this->surfaceMontageNodeColoringForBrowserTabs[browserTabIndex].size() != numberOfComponentsRGBA) {
        if (zeroizeColorsFlag) {
            this->surfaceMontageNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA, 0.0);
        }
        else {
            this->surfaceMontageNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA);
        }
    }
}

/**
 * Allocate node coloring for a whole brain surface in a browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param zeroizeColorsFlag
 *    If true and memory is allocated for colors, the color components
 *    are set to all zeros, otherwise the memory may be left unitialized.
 */
void 
SurfaceFile::allocateWholeBrainNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                                      const bool zeroizeColorsFlag)
{
    CaretAssertArrayIndex(this->wholeBrainNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    const uint64_t numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    if (this->wholeBrainNodeColoringForBrowserTabs[browserTabIndex].size() != numberOfComponentsRGBA) {
        if (zeroizeColorsFlag) {
            this->wholeBrainNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA, 0.0);
        }
        else {
            this->wholeBrainNodeColoringForBrowserTabs[browserTabIndex].resize(numberOfComponentsRGBA);
        }
    }
}

/**
 * Get the RGBA color components for this single surface in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @return
 *    Coloring for the tab or NULL if coloring is invalid and needs to be 
 *    set.
 */
float* 
SurfaceFile::getSurfaceNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex)
{
    CaretAssertArrayIndex(this->surfaceNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    std::vector<float>& rgba = this->surfaceNodeColoringForBrowserTabs[browserTabIndex];
    if (rgba.empty()) {
        return NULL;
    }
    
    return &rgba[0];
}

/**
 * Set the RGBA color components for this a single surface in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param rgbaNodeColorComponents
 *    RGBA color components for this surface in the given tab.
 */
void 
SurfaceFile::setSurfaceNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                         const float* rgbaNodeColorComponents)
{
    CaretAssertArrayIndex(this->surfaceNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    this->allocateSurfaceNodeColoringForBrowserTab(browserTabIndex, 
                                            false);
    const int numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    std::vector<float>& rgba = this->surfaceNodeColoringForBrowserTabs[browserTabIndex];
    for (int32_t i = 0; i < numberOfComponentsRGBA; i++) {
        rgba[i] = rgbaNodeColorComponents[i];
    }
    
    if ((browserTabIndex >= 0)
        && (browserTabIndex < static_cast<int32_t>(m_surfaceGraphicsPrimitives.size()))) {
        m_surfaceGraphicsPrimitives[browserTabIndex].reset();
    }
}

/**
 * Get the RGBA color components for this surface montage in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @return
 *    Coloring for the tab or NULL if coloring is invalid and needs to be 
 *    set.
 */
float* 
SurfaceFile::getSurfaceMontageNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex)
{
    CaretAssertArrayIndex(this->surfaceMontageNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    std::vector<float>& rgba = this->surfaceMontageNodeColoringForBrowserTabs[browserTabIndex];
    if (rgba.empty()) {
        return NULL;
    }
    
    return &rgba[0];
}

/**
 * Set the RGBA color components for this a surface montage in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param rgbaNodeColorComponents
 *    RGBA color components for this surface montage in the given tab.
 */
void 
SurfaceFile::setSurfaceMontageNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                                                     const float* rgbaNodeColorComponents)
{
    CaretAssertArrayIndex(this->surfaceMontageNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    this->allocateSurfaceMontageNodeColoringForBrowserTab(browserTabIndex, 
                                                   false);
    const int numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    std::vector<float>& rgba = this->surfaceMontageNodeColoringForBrowserTabs[browserTabIndex];
    for (int32_t i = 0; i < numberOfComponentsRGBA; i++) {
        rgba[i] = rgbaNodeColorComponents[i];
    }
    
    if ((browserTabIndex >= 0)
        && (browserTabIndex < static_cast<int32_t>(m_surfaceMontageGraphicsPrimitives.size()))) {
        m_surfaceMontageGraphicsPrimitives[browserTabIndex].reset();
    }
}


/**
 * Get the RGBA color components for this whole brain surface in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @return
 *    Coloring for the tab or NULL if coloring is invalid and needs to be set.
 */
float* 
SurfaceFile::getWholeBrainNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex)
{
    CaretAssertArrayIndex(this->wholeBrainNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    std::vector<float>& rgba = this->wholeBrainNodeColoringForBrowserTabs[browserTabIndex];
    if (rgba.empty()) {
        return NULL;
    }
    return &rgba[0];
}


/**
 * Set the RGBA color components for this a whole brain surface in the given tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param rgbaNodeColorComponents
 *    RGBA color components for this surface in the given tab.
 */
void 
SurfaceFile::setWholeBrainNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                                                     const float* rgbaNodeColorComponents)
{
    CaretAssertArrayIndex(this->wholeBrainNodeColoringForBrowserTabs, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          browserTabIndex);
    
    this->allocateWholeBrainNodeColoringForBrowserTab(browserTabIndex, 
                                                   false);
    const int numberOfComponentsRGBA = this->getNumberOfNodes() * 4;
    std::vector<float>& rgba = this->wholeBrainNodeColoringForBrowserTabs[browserTabIndex];
    for (int32_t i = 0; i < numberOfComponentsRGBA; i++) {
        rgba[i] = rgbaNodeColorComponents[i];
    }
    
    if ((browserTabIndex >= 0)
        && (browserTabIndex < static_cast<int32_t>(m_wholeBrainGraphicsPrimitives.size()))) {
        m_wholeBrainGraphicsPrimitives[browserTabIndex].reset();
    }
}

/**
 * @return the graphics primitive for drawing this surface for a single surface view
 * in the given tab index
 * @param browserTabIndex
 *    Index of the tab
 */
GraphicsPrimitiveV3fN3fC4f*
SurfaceFile::getSurfaceGraphicsPrimitiveForBrowserTab(const int32_t browserTabIndex)
{
    const float* allRGBA(getSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex));
    GraphicsPrimitiveV3fN3fC4f* primitiveOut(getGraphicsPrimitive(m_surfaceGraphicsPrimitives,
                                                                  browserTabIndex,
                                                                  allRGBA));
    return primitiveOut;
}

/**
 * @return the graphics primitive for drawing this surface for a single surface view
 * in the given tab index
 * @param primitives
 *    Primitives for each tab index
 * @param browserTabIndex
 *    Index of the tab
 * @param rgba
 *    The RGBA coloring for the surface
 */
GraphicsPrimitiveV3fN3fC4f*
SurfaceFile::getGraphicsPrimitive(std::vector<std::unique_ptr<GraphicsPrimitiveV3fN3fC4f>>& primitives,
                                                                     const int32_t browserTabIndex,
                                                                     const float* rgba)
{
    GraphicsPrimitiveV3fN3fC4f* primitiveOut(NULL);
    
    if ((browserTabIndex >= 0)
        && (browserTabIndex < static_cast<int32_t>(primitives.size()))) {
        primitiveOut = primitives[browserTabIndex].get();
    }
    
    if (primitiveOut == NULL) {
        primitiveOut = createSurfaceGraphicsPrimitive(rgba);
        
        if (primitiveOut != NULL) {
            if (browserTabIndex >= static_cast<int32_t>(primitives.size())) {
                primitives.resize(browserTabIndex + 1);
            }
            primitives[browserTabIndex].reset(primitiveOut);
        }
    }
    
    return primitiveOut;

}

/*
 * @return Graphics primitive for drawing this surface in the given RGBA colors
 * @param rgba
 *     The RGBA coloring
 * @return
 *     The graphics primitive
 */
GraphicsPrimitiveV3fN3fC4f*
SurfaceFile::createSurfaceGraphicsPrimitive(const float* rgba)
{
    GraphicsPrimitiveV3fN3fC4f* primitiveOut(GraphicsPrimitive::newPrimitiveV3fN3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES));
    
    const int32_t numberOfTriangles(getNumberOfTriangles());
    for (int32_t i = 0; i < numberOfTriangles; i++) {
        const int32_t* triangleIndices(getTriangle(i));
        for (int32_t j = 0; j < 3; j++) {
            const int32_t vertexIndex(triangleIndices[j]);
            const float* xyz(getCoordinate(vertexIndex));
            const float* normalXYZ(getNormalVector(vertexIndex));
            const float* vertexRgba(&rgba[vertexIndex * 4]);
            primitiveOut->addVertex(xyz, normalXYZ, vertexRgba);
        }
    }
    return primitiveOut;
}
/**
 * @return the graphics primitive for drawing this surface for a  surface montage view
 * in the given tab index
 * @param browserTabIndex
 *    Index of the tab
 */
GraphicsPrimitiveV3fN3fC4f*
SurfaceFile::getSurfaceMontageGraphicsPrimitiveForBrowserTab(const int32_t browserTabIndex)
{
    const float* allRGBA(getSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex));
    GraphicsPrimitiveV3fN3fC4f* primitiveOut(getGraphicsPrimitive(m_surfaceMontageGraphicsPrimitives,
                                                                  browserTabIndex,
                                                                  allRGBA));
    return primitiveOut;
}

/**
 * @return the graphics primitive for drawing this surface for a whole brain view
 * in the given tab index
 * @param browserTabIndex
 *    Index of the tab
 */
GraphicsPrimitiveV3fN3fC4f*
SurfaceFile::getWholeBrainGraphicsPrimitiveForBrowserTab(const int32_t browserTabIndex)
{
    const float* allRGBA(getWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex));
    GraphicsPrimitiveV3fN3fC4f* primitiveOut(getGraphicsPrimitive(m_wholeBrainGraphicsPrimitives,
                                                                  browserTabIndex,
                                                                  allRGBA));
    return primitiveOut;
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
SurfaceFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* invalidateEvent =
        dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(invalidateEvent);
        
        invalidateEvent->setEventProcessed();
        
        this->invalidateNodeColoringForBrowserTabs();
    }    
}

bool SurfaceFile::matchesTopology(const SurfaceFile& rhs) const
{
    if (getNumberOfNodes() != rhs.getNumberOfNodes()) return false;
    int numTriangles = getNumberOfTriangles();
    if (numTriangles != rhs.getNumberOfTriangles()) return false;
    for (int i = 0; i < numTriangles; ++i)
    {
        int i3 = i * 3;
        if (trianglePointer[i3] != rhs.trianglePointer[i3]) return false;//exactly same order of triangles and nodes, for strictest topology equivalence
        if (trianglePointer[i3 + 1] != rhs.trianglePointer[i3 + 1]) return false;//also, is a faster test
        if (trianglePointer[i3 + 2] != rhs.trianglePointer[i3 + 2]) return false;
    }
    return true;
}

bool SurfaceFile::hasNodeCorrespondence(const SurfaceFile& rhs) const
{
    int numNodes = getNumberOfNodes();
    if (numNodes != rhs.getNumberOfNodes()) return false;
    if (getNumberOfTriangles() != rhs.getNumberOfTriangles()) return false;
    if (matchesTopology(rhs)) return true;//short circuit the common, faster to check case, should fail very early if it fails at all
    CaretPointer<TopologyHelper> myHelp = getTopologyHelper(), rightHelp = rhs.getTopologyHelper();
    for (int i = 0; i < numNodes; ++i)
    {
        const std::vector<int32_t>& myNeigh = myHelp->getNodeNeighbors(i);
        const std::vector<int32_t>& rightNeigh = rightHelp->getNodeNeighbors(i);
        int mySize = (int)myNeigh.size();
        if (mySize != (int)rightNeigh.size()) return false;
        std::set<int32_t> myUsed;
        for (int j = 0; j < mySize; ++j)
        {
            myUsed.insert(myNeigh[j]);
        }
        for (int j = 0; j < mySize; ++j)
        {
            if (myUsed.find(rightNeigh[j]) == myUsed.end()) return false;
        }
    }
    return true;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
SurfaceFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    GiftiTypeFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Number of Triangles",
                                        getNumberOfTriangles());
    dataFileInformation.addNameAndValue("Normal Vectors Correct",
                                        areNormalVectorsCorrect());
    dataFileInformation.addNameAndValue("Surface Type (Primary)",
                                        SurfaceTypeEnum::toGuiName(getSurfaceType()));
    dataFileInformation.addNameAndValue("Surface Type (Secondary)",
                                        SecondarySurfaceTypeEnum::toGuiName(getSecondaryType()));
    
    const BoundingBox* boundingBox = getBoundingBox();
    dataFileInformation.addNameAndValue("X-minimum", boundingBox->getMinX());
    dataFileInformation.addNameAndValue("X-maximum", boundingBox->getMaxX());
    dataFileInformation.addNameAndValue("Y-minimum", boundingBox->getMinY());
    dataFileInformation.addNameAndValue("Y-maximum", boundingBox->getMaxY());
    dataFileInformation.addNameAndValue("Z-minimum", boundingBox->getMinZ());
    dataFileInformation.addNameAndValue("Z-maximum", boundingBox->getMaxZ());
    if (getSurfaceType() == SurfaceTypeEnum::SPHERICAL) {
        dataFileInformation.addNameAndValue("Spherical Radius", getSphericalRadius());
    }
    dataFileInformation.addNameAndValue("Surface Area", getSurfaceArea());
    
    DescriptiveStatistics stats;
    getNodesSpacingStatistics(stats);
    
    dataFileInformation.addNameAndValue("Spacing Mean", stats.getMean());
    dataFileInformation.addNameAndValue("Spacing Std Dev", stats.getStandardDeviationSample());
    dataFileInformation.addNameAndValue("Spacing Minimum", stats.getMinimumValue());
    dataFileInformation.addNameAndValue("Spacing Maximum", stats.getMaximumValue());
}

/**
 * @return A String describing the content of this object.
 */
AString
SurfaceFile::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the instance's content.
 *
 * @param descriptionOut
 *    Description of the instance's content.
 */
void
SurfaceFile::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Surface: "
                          + getFileNameNoPath());
    descriptionOut.addLine("   Structure: "
                          + StructureEnum::toGuiName(getStructure()));
    descriptionOut.addLine("   Primary Type: "
                          + SurfaceTypeEnum::toGuiName(getSurfaceType()));
    descriptionOut.addLine("   Secondary Type: "
                          + SecondarySurfaceTypeEnum::toGuiName(getSecondaryType()));
}

