/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <limits>

#include <QThread>

#include "BoundingBox.h"
#include "DataFileTypeEnum.h"
#include "SurfaceFile.h"
#include "CaretAssert.h"
#include "CaretOMP.h"
#include "DescriptiveStatistics.h"
#include "EventSurfaceColoringInvalidate.h"

#include "GiftiFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"

#include "CaretPointLocator.h"
#include "GeodesicHelper.h"
#include "SignedDistanceHelper.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * Constructor.
 */
SurfaceFile::SurfaceFile()
: GiftiTypeFile(DataFileTypeEnum::SURFACE)
{
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
SurfaceFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->initializeMembersSurfaceFile();
    
    int numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays != 2) {
        throw DataFileException("Number of data arrays MUST be two in a SurfaceFile.");
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
    
    AString errorMessage;
    if (this->coordinateDataArray == NULL) {
        if (errorMessage.isEmpty() == false) {
        }
        errorMessage += "Unable to find coordinate data array which "
            " contains data type FLOAT32, Intent POINTSET, and two "
            " dimensions with the second dimension set to three.  ";
    }
    if (this->triangleDataArray == NULL) {
        errorMessage += "Unable to find topology data array which "
        " contains data type INT32, Intent TRIANGLE, and two "
        " dimensions with the second dimension set to three.";
    }
    if (errorMessage.isEmpty() == false) {
        throw DataFileException(errorMessage);
    }
    
    this->computeNormals();

    const int64_t numNodes = this->getNumberOfNodes();
    const uint64_t numColorComponents = numNodes * 4;
    
    if (numColorComponents != this->nodeColoring.size()) {
        this->nodeColoring.resize(numColorComponents);
        
        for (int64_t i = 0; i < numNodes; i++) {
            const int64_t i4 = i * 4;
            this->nodeColoring[i4]   = 0.75f;
            this->nodeColoring[i4+1] = 0.75f;
            this->nodeColoring[i4+2] = 0.75f;
            this->nodeColoring[i4+3] = 1.0f;
        }
    }
    
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
    CaretAssert(this->coordinatePointer);
    const int32_t offset = nodeIndex * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfNodes() * 3)));
    this->coordinatePointer[offset] = xyzIn[0];
    this->coordinatePointer[offset+1] = xyzIn[1];
    this->coordinatePointer[offset+2] = xyzIn[2];
    invalidateHelpers();
    setModified();
}

void 
SurfaceFile::setCoordinates(const float *coordinates, const int64_t coordCount)
{
    CaretAssert(this->coordinatePointer);
    
    CaretAssert(this->getNumberOfNodes() == coordCount);
    memcpy(this->coordinatePointer,coordinates,12*coordCount);    
    invalidateHelpers();
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
SurfaceFile::computeNormals(const bool averageNormals)
{
    if (m_normalsComputed && (averageNormals == m_normalsAveraged))//don't recompute when not needed
    {
        return;
    }
    m_normalsComputed = true;
    m_normalsAveraged = averageNormals;
    int32_t numCoords = this->getNumberOfNodes();
    if (numCoords > 0) {
        this->normalVectors.resize(numCoords * 3);
    }
    else {
        this->normalVectors.clear();
    }
    
    const int32_t numTriangles = this->getNumberOfTriangles();
    if ((numCoords > 0) && (numTriangles > 0)) {
        float* normalPointer = &this->normalVectors[0];
        std::vector<float> numContribute(numCoords, 0.0f);

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
                numContribute[n1] += 1.0;
                normalPointer[c2 + 0] += triangleNormal[0];
                normalPointer[c2 + 1] += triangleNormal[1];
                normalPointer[c2 + 2] += triangleNormal[2];
                numContribute[n2] += 1.0;
                normalPointer[c3 + 0] += triangleNormal[0];
                normalPointer[c3 + 1] += triangleNormal[1];
                normalPointer[c3 + 2] += triangleNormal[2];
                numContribute[n3] += 1.0;
            }
        }
        
#pragma omp CARET_PAR
        {
			int32_t i = 0;
#pragma omp CARET_FOR schedule(static,1000) private(i)
            for (i = 0; i < numCoords; i++) {
                const int32_t i3 = i * 3;
                if (numContribute[i] > 0.0) {
                    MathFunctions::normalizeVector(normalPointer + i3);
                } else {
                    normalPointer[i3 + 0] = 0.0f;//zero the normals for unconnected nodes
                    normalPointer[i3 + 1] = 0.0f;
                    normalPointer[i3 + 2] = 0.0f;
                }
            }
        }
        
        if (averageNormals)
        {
            float* avgTemp = new float[numCoords * 3];
#pragma omp CARET_PAR
            {
                float tempVec[3];
                CaretPointer<TopologyHelper> myTopoHelp = getTopologyHelper();//TODO: make this not circular - separate base that doesn't handle helpers (and is used by helpers) from file that handles helpers and normals?
                int32_t i = 0; 
#pragma omp CARET_FOR schedule(static,1000) private(i)
                for (i = 0; i < numCoords; ++i)
                {
                    int32_t i3 = i * 3;
                    tempVec[0] = 0.0f;
                    tempVec[1] = 0.0f;
                    tempVec[2] = 0.0f;
                    const std::vector<int32_t>& neighbors = myTopoHelp->getNodeNeighbors(i);
                    int32_t numNeigh = (int32_t)neighbors.size();
                    for (int32_t j = 0; j < numNeigh; ++j)
                    {
                        int32_t neighbase = neighbors[j] * 3;
                        tempVec[0] += normalPointer[neighbase];
                        tempVec[1] += normalPointer[neighbase + 1];
                        tempVec[2] += normalPointer[neighbase + 2];
                    }
                    MathFunctions::normalizeVector(tempVec);
                    avgTemp[i3] = tempVec[0];
                    avgTemp[i3 + 1] = tempVec[1];
                    avgTemp[i3 + 2] = tempVec[2];
                }
                for (int32_t i = 0; i < numCoords; ++i)
                {
                    int32_t i3 = i * 3;
                    normalPointer[i3] = avgTemp[i3];
                    normalPointer[i3 + 1] = avgTemp[i3 + 1];
                    normalPointer[i3 + 2] = avgTemp[i3 + 2];
                }
            }
            delete[] avgTemp;
        }
    }
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
 * Get the coloring for a node.
 *
 * @param nodeIndex
 *    Index of node for color components.
 * @return
 *    A pointer to 4 elements that are the 
 *    red, green, blue, and alpha components
 *    each of which ranges zero to one.
 */
const float* 
SurfaceFile::getNodeColor(int32_t nodeIndex) const
{
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()),
                       "Invalid index for vertex coloring.");
    
    return &this->nodeColoring[nodeIndex * 4];
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
    const float scaleX = (targetBoundingBox->getDifferenceX()
                          / myBoundingBox->getDifferenceX());
    const float scaleY = (targetBoundingBox->getDifferenceY()
                          / myBoundingBox->getDifferenceY());
    const float scaleZ = (targetBoundingBox->getDifferenceZ()
                          / myBoundingBox->getDifferenceZ());
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
    if (m_locator == NULL)//try to avoid locking even once
    {
        CaretMutexLocker myLock(&m_locatorMutex);
        if (m_locator == NULL)//test again AFTER lock to avoid race conditions
        {
            m_locator.grabNew(new CaretPointLocator(getCoordinateData(), getNumberOfNodes()));
        }
    }
    if (maxDist > 0.0f)
    {
        return m_locator->closestPointLimited(target, maxDist);
    } else {
        return m_locator->closestPoint(target);
    }
}

/**
 * @populates timeline with surface coordinates and structure information
 */
void
SurfaceFile::getTimeLineInformation(int32_t nodeIndex, TimeLine &tl) const
{
    float point[3] = {0.0, 0.0, 0.0};
    this->getCoordinate(nodeIndex,point);
    AString structure = StructureEnum::toGuiName(this->getStructure());
    AString label = structure + ":[" + AString::fromNumbers(point,3,AString(", ")) + "]";
    for(int i = 0;i<3;i++) tl.point[i] = point[i];
    tl.label = label;
    
    tl.surfaceNumberOfNodes = getNumberOfNodes();
    tl.structure = getStructure();
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


