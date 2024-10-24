
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

#define __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__
#include "SurfaceProjectionBarycentric.h"
#undef __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__

#include "CaretAssert.h"
#include "DataFileException.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "XmlWriter.h"

#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QStringList>

using namespace caret;

    
/**
 * \class caret::SurfaceProjectionBarycentric 
 * \brief Maintains a barycentric projection.
 *
 */

/**
 * Constructor.
 */
SurfaceProjectionBarycentric::SurfaceProjectionBarycentric()
: SurfaceProjection()
{
    this->resetAllValues();
}

/**
 * Destructor.
 */
SurfaceProjectionBarycentric::~SurfaceProjectionBarycentric()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SurfaceProjectionBarycentric::SurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj)
: SurfaceProjection(obj)
{
    this->copyHelperSurfaceProjectionBarycentric(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SurfaceProjectionBarycentric&
SurfaceProjectionBarycentric::operator=(const SurfaceProjectionBarycentric& obj)
{
    if (this != &obj) {
        SurfaceProjection::operator=(obj);
        this->copyHelperSurfaceProjectionBarycentric(obj);
    }
    return *this;    
}

bool SurfaceProjectionBarycentric::operator==(const SurfaceProjectionBarycentric& rhs)
{
    if (projectionValid != rhs.projectionValid) return false;
    if (projectionValid)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (triangleAreas[i] != rhs.triangleAreas[i]) return false;
            if (triangleNodes[i] != rhs.triangleNodes[i]) return false;
        }
        if (m_degenerate != rhs.m_degenerate) return false;
        return (signedDistanceAboveSurface == rhs.signedDistanceAboveSurface);
    }
    return true;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SurfaceProjectionBarycentric::copyHelperSurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj)
{
    this->setTriangleAreas(obj.getTriangleAreas());
    this->setTriangleNodes(obj.getTriangleNodes());
    this->signedDistanceAboveSurface = obj.signedDistanceAboveSurface;
    this->projectionValid = obj.projectionValid;
    this->m_degenerate = obj.m_degenerate;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceProjectionBarycentric::toString() const
{
    AString txt = SurfaceProjection::toString();
    if (txt.isEmpty() == false) {
        txt += ", ";
    }
    txt += ("projectionValid=" + AString::fromBool(projectionValid)
            + ", triangleAreas=(" + AString::fromNumbers(this->triangleAreas, 3, ",")
            + "), triangleNodes=(" + AString::fromNumbers(this->triangleNodes, 3, ",")
            + "), signedDistanceAboveSurface=" + AString::number(this->signedDistanceAboveSurface));
    
    return txt;
}

/**
 * @return The signed distance above the surface.
 */
float 
SurfaceProjectionBarycentric::getSignedDistanceAboveSurface() const
{
    return this->signedDistanceAboveSurface;
}

/**
 * Set the signed distance above the surface.
 * 
 * @param signedDistanceAboveSurface
 *    New value.
 */
void 
SurfaceProjectionBarycentric::setSignedDistanceAboveSurface(const float signedDistanceAboveSurface)
{
    this->signedDistanceAboveSurface = signedDistanceAboveSurface;
    this->setModified();
}

/**
 * @return The triangle nodes (3 elements).
 */
const int32_t* 
SurfaceProjectionBarycentric::getTriangleNodes() const
{
    return this->triangleNodes;
}

/**
 * Set the triangle nodes.
 * 
 * @param triangleNodes
 *    New values for nodes.
 */
void 
SurfaceProjectionBarycentric::setTriangleNodes(const int32_t triangleNodes[3])
{
    this->triangleNodes[0] = triangleNodes[0];
    this->triangleNodes[1] = triangleNodes[1];
    this->triangleNodes[2] = triangleNodes[2];
    this->setModified();
}

/**
 * @return The triangle areas (3 elements).
 */
const float* 
SurfaceProjectionBarycentric::getTriangleAreas() const
{
    return this->triangleAreas;
}

int32_t SurfaceProjectionBarycentric::getNodeWithLargestWeight() const
{
    int32_t ret = -1;
    float largestWeight = 0.0f;//there must be a positive weight
    for (int i = 0; i < 3; ++i)
    {
        if (triangleAreas[i] > largestWeight)
        {
            ret = triangleNodes[i];
            largestWeight = triangleAreas[i];
        }
    }
    return ret;
}

/**
 * Set the triangle areas.
 * 
 * @param triangleAreas
 *    New values for triangle areas.
 */
void 
SurfaceProjectionBarycentric::setTriangleAreas(const float triangleAreas[3])
{
    this->triangleAreas[0] = triangleAreas[0];
    this->triangleAreas[1] = triangleAreas[1];
    this->triangleAreas[2] = triangleAreas[2];
    this->setModified();
}

/**
 * Unproject to the surface using 'this' projection.
 *
 * @param surfaceFile
 *    Surface file used for unprojecting.
 * @param topologyHelperIn
 *    Topology helper.  If NULL, topology helper from surfaceFile
 *    will be used but frequent calls to get the topology helper
 *    may be slow.
 * @param xyzOut
 *    Output containing coordinate created by unprojecting.
 * @param offsetFromSurface
 *    If 'unprojectWithOffsetFromSurface' is true, unprojected
 *    position will be this distance above (negative=below)
 *    the surface.
 * @param unprojectWithOffsetFromSurface
 *    If true, ouput coordinate will be offset 'offsetFromSurface'
 *    distance from the surface.
 * @return
 *    True if unprojection was successful.
 */
bool
SurfaceProjectionBarycentric::unprojectToSurface(const SurfaceFile& surfaceFile,
                                                 const TopologyHelper* topologyHelperIn,
                                                 float xyzOut[3],
                                                 const float offsetFromSurface,
                                                 const bool unprojectWithOffsetFromSurface) const
{
    /*
     * Make sure projection surface number of nodes matches surface.
     */
    if (this->projectionSurfaceNumberOfNodes > 0) {
        if (surfaceFile.getNumberOfNodes() != this->projectionSurfaceNumberOfNodes) {
            return false;
        }
    }
    
    const int32_t n1 = this->triangleNodes[0];
    const int32_t n2 = this->triangleNodes[1];
    const int32_t n3 = this->triangleNodes[2];
    
    CaretAssert(n1 < surfaceFile.getNumberOfNodes());
    CaretAssert(n2 < surfaceFile.getNumberOfNodes());
    CaretAssert(n3 < surfaceFile.getNumberOfNodes());
    
    /*
     * All nodes MUST have neighbors (connected)
     */
    const TopologyHelper* topologyHelper = ((topologyHelperIn != NULL)
                                            ? topologyHelperIn
                                            : surfaceFile.getTopologyHelper().getPointer());
    if ((topologyHelper->getNodeHasNeighbors(n1) == false)
        || (topologyHelper->getNodeHasNeighbors(n2) == false)
        || (topologyHelper->getNodeHasNeighbors(n3) == false)) {
        return false;
    }
    
    const float* c1 = surfaceFile.getCoordinate(n1);
    const float* c2 = surfaceFile.getCoordinate(n2);
    const float* c3 = surfaceFile.getCoordinate(n3);
    
    float barycentricXYZ[3];
    float barycentricNormal[3];
    
    /*
     * If all the nodes are the same (object projects to a single node, not triangle)
     */
    if ((n1 == n2) &&
        (n2 == n3)) {
        /*
         * Use node's normal vector and position
         */
        barycentricXYZ[0] = c1[0];
        barycentricXYZ[1] = c1[1];
        barycentricXYZ[2] = c1[2];
        const float* nodeNormal = surfaceFile.getNormalVector(n1);
        barycentricNormal[0] = nodeNormal[0];
        barycentricNormal[1] = nodeNormal[1];
        barycentricNormal[2] = nodeNormal[2];
    }
    else {
        /*
         * Compute position using barycentric coordinates
         */
        float t1[3];
        float t2[3];
        float t3[3];
        for (int i = 0; i < 3; i++) {
            t1[i] = triangleAreas[0] * c1[i];
            t2[i] = triangleAreas[1] * c2[i];
            t3[i] = triangleAreas[2] * c3[i];
        }
        float area = (triangleAreas[0]
                      + triangleAreas[1]
                      + triangleAreas[2]);
        if (area != 0) {
            for (int i = 0; i < 3; i++) {
                barycentricXYZ[i] = (t1[i] + t2[i] + t3[i]) / area;
            }
        }
        else {
            return false;
        }
        
        if (MathFunctions::normalVector(c1, c2, c3, barycentricNormal) == false) {
            return false;
        }
    }
    
    /*
     * Set output coordinate, possibly offsetting from surface.
     */
    for (int j = 0; j < 3; j++) {
        if (unprojectWithOffsetFromSurface) {
            xyzOut[j] = (barycentricXYZ[j]
                         + (barycentricNormal[j] * offsetFromSurface));
        }
        else {
            xyzOut[j] = (barycentricXYZ[j]
                         + (barycentricNormal[j] * signedDistanceAboveSurface));
        }
    }
    
    return true;
}

/**
 * Unproject to surface
 * @param surfaceFile
 *    Unproject to this surface
 * @param barycentricAreas
 *    The barycentric areas
 * @param barycentricVertices
 *    The vertices
 * @param xyzOut
 *    Output with unprojected to XYZ coordinates
 * @return True if valid
 */
bool
SurfaceProjectionBarycentric::unprojectToSurface(const SurfaceFile* surfaceFile,
                                                 const float barycentricAreas[3],
                                                 const int32_t barycentricVertices[3],
                                                 float xyzOut[3])
{
    CaretAssert(surfaceFile);

    xyzOut[0] = 0.0;
    xyzOut[1] = 0.0;
    xyzOut[2] = 0.0;
    const int32_t n1(barycentricVertices[0]);
    const int32_t n2(barycentricVertices[1]);
    const int32_t n3(barycentricVertices[2]);
    
    if ((n1>= 0) && (n1 < surfaceFile->getNumberOfNodes())
        && (n2 >= 0) && (n2 < surfaceFile->getNumberOfNodes())
        && (n3 >= 0) && (n3 < surfaceFile->getNumberOfNodes())) {
        /*
         * All nodes MUST have neighbors (connected)
         */
        const TopologyHelper* topologyHelper = surfaceFile->getTopologyHelper().getPointer();
        if ((topologyHelper->getNodeHasNeighbors(n1) == false)
            || (topologyHelper->getNodeHasNeighbors(n2) == false)
            || (topologyHelper->getNodeHasNeighbors(n3) == false)) {
            return false;
        }
        
        const float* xyz1 = surfaceFile->getCoordinate(n1);
        const float* xyz2 = surfaceFile->getCoordinate(n2);
        const float* xyz3 = surfaceFile->getCoordinate(n3);

        /*
         * If all the nodes are the same (object projects to a single node, not triangle)
         */
        if ((n1 == n2) &&
            (n2 == n3)) {
            /*
             * Use node's normal vector and position
             */
            xyzOut[0] = xyz1[0];
            xyzOut[1] = xyz1[1];
            xyzOut[2] = xyz1[2];
        }
        else {
            /*
             * Compute position using barycentric coordinates
             */
            float t1[3];
            float t2[3];
            float t3[3];
            for (int i = 0; i < 3; i++) {
                t1[i] = barycentricAreas[0] * xyz1[i];
                t2[i] = barycentricAreas[1] * xyz2[i];
                t3[i] = barycentricAreas[2] * xyz3[i];
            }
            float area = (barycentricAreas[0]
                          + barycentricAreas[1]
                          + barycentricAreas[2]);
            if (area != 0) {
                for (int i = 0; i < 3; i++) {
                    xyzOut[i] = (t1[i] + t2[i] + t3[i]) / area;
                }
            }
            else {
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

/**
 * Unproject to the surface using 'this' projection.
 * 
 * @param surfaceFile
 *    Surface file used for unprojecting.
 * @param xyzOut
 *    Output containing coordinate created by unprojecting.
 * @param offsetFromSurface
 *    If 'unprojectWithOffsetFromSurface' is true, unprojected
 *    position will be this distance above (negative=below)
 *    the surface.
 * @param unprojectWithOffsetFromSurface
 *    If true, ouput coordinate will be offset 'offsetFromSurface' 
 *    distance from the surface.
 * @return
 *    True if unprojection was successful.
 */
bool 
SurfaceProjectionBarycentric::unprojectToSurface(const SurfaceFile& surfaceFile,
                                                 float xyzOut[3],
                                                 const float offsetFromSurface,
                                                 const bool unprojectWithOffsetFromSurface) const
{
    /*
     * Make sure projection surface number of nodes matches surface.
     */
    if (this->projectionSurfaceNumberOfNodes > 0) {
        if (surfaceFile.getNumberOfNodes() != this->projectionSurfaceNumberOfNodes) {
            return false;
        }
    }
    
    const int32_t n1 = this->triangleNodes[0];
    const int32_t n2 = this->triangleNodes[1];
    const int32_t n3 = this->triangleNodes[2];
    
    CaretAssert(n1 < surfaceFile.getNumberOfNodes());
    CaretAssert(n2 < surfaceFile.getNumberOfNodes());
    CaretAssert(n3 < surfaceFile.getNumberOfNodes());
    
    /*
     * All nodes MUST have neighbors (connected)
     */
    const TopologyHelper* topologyHelper = surfaceFile.getTopologyHelper().getPointer();
    if ((topologyHelper->getNodeHasNeighbors(n1) == false) 
        || (topologyHelper->getNodeHasNeighbors(n2) == false) 
        || (topologyHelper->getNodeHasNeighbors(n3) == false)) {
        return false;
    }
    
    const float* c1 = surfaceFile.getCoordinate(n1);
    const float* c2 = surfaceFile.getCoordinate(n2);
    const float* c3 = surfaceFile.getCoordinate(n3);
    
    float barycentricXYZ[3];
    float barycentricNormal[3];
    
    /*
     * If all the nodes are the same (object projects to a single node, not triangle)
     */
    if ((n1 == n2) &&
        (n2 == n3)) {
        /*
         * Use node's normal vector and position
         */
        barycentricXYZ[0] = c1[0];
        barycentricXYZ[1] = c1[1];
        barycentricXYZ[2] = c1[2];
        const float* nodeNormal = surfaceFile.getNormalVector(n1);
        barycentricNormal[0] = nodeNormal[0];
        barycentricNormal[1] = nodeNormal[1];
        barycentricNormal[2] = nodeNormal[2];
    }
    else {
        /*
         * Compute position using barycentric coordinates
         */
        float t1[3];
        float t2[3];
        float t3[3];
        for (int i = 0; i < 3; i++) {
            t1[i] = triangleAreas[0] * c1[i];
            t2[i] = triangleAreas[1] * c2[i];
            t3[i] = triangleAreas[2] * c3[i];
        }
        float area = (triangleAreas[0] 
                      + triangleAreas[1]
                      + triangleAreas[2]);
        if (area != 0) {
            for (int i = 0; i < 3; i++) {
                barycentricXYZ[i] = (t1[i] + t2[i] + t3[i]) / area;
            }
        }
        else {
            return false;
        }
        
        if (MathFunctions::normalVector(c1, c2, c3, barycentricNormal) == false) {
            return false;
        }
    }
    
    /*
     * Set output coordinate, possibly offsetting from surface.
     */
    for (int j = 0; j < 3; j++) {
        if (unprojectWithOffsetFromSurface) {
            xyzOut[j] = (barycentricXYZ[j]
                         + (barycentricNormal[j] * offsetFromSurface));
        }
        else {
            xyzOut[j] = (barycentricXYZ[j] 
                         + (barycentricNormal[j] * signedDistanceAboveSurface));
        }
    }
    
    return true;
}

/**
 * Reset the surface projection to its initial state.
 */
void 
SurfaceProjectionBarycentric::reset()
{
    this->resetAllValues();
}

/**
 * @return Is the projection valid?
 */
bool 
SurfaceProjectionBarycentric::isValid() const
{
    return this->projectionValid;
}

/**
 * Set the validity of the projection.
 * @param valid
 *    New validity status.
 */
void 
SurfaceProjectionBarycentric::setValid(const bool valid)
{
    this->projectionValid = valid;
}

/**
 * Set the projection is degenerate (on
 * an edge or just outside the edge).
 * @param degenerate
 *     New status.
 */
void
SurfaceProjectionBarycentric::setDegenerate(const bool degenerate)
{
    m_degenerate = degenerate;
}

/**
 * @return Is the projection degenerate (on
 * an edge or just outside the edge).
 */
bool
SurfaceProjectionBarycentric::isDegenerate() const
{
    return m_degenerate;
}

/**
 * Since reset overrides the 'super' class it should
 * never be called from a constructor.  So, this 
 * method does the actual reset, and since it does
 * not override a method from the 'super' class, it
 * may be called from this class' constructor.
 */
void 
SurfaceProjectionBarycentric::resetAllValues()
{
    this->projectionValid  = false;
    m_degenerate = false;
    
    this->triangleAreas[0] = -1.0;
    this->triangleAreas[1] = -1.0;
    this->triangleAreas[2] = -1.0;
    
    this->triangleNodes[0] = -1;
    this->triangleNodes[1] = -1;
    this->triangleNodes[2] = -1;
    
    this->signedDistanceAboveSurface = 0.0;
}

/**
 * Write the projection to XML.
 * @param xmlWriter
 *   The XML Writer.
 * @throw XmlException
 *   If an error occurs.
 */
void 
SurfaceProjectionBarycentric::writeAsXML(XmlWriter& xmlWriter)
{
    /*
     * Note: Degenerate status is not saved!
     */
    if (this->projectionValid) {
        xmlWriter.writeStartElement(XML_TAG_PROJECTION_BARYCENTRIC);
        xmlWriter.writeElementCharacters(XML_TAG_TRIANGLE_AREAS, this->triangleAreas, 3);
        xmlWriter.writeElementCharacters(XML_TAG_TRIANGLE_NODES, this->triangleNodes, 3);
        xmlWriter.writeElementCharacters(XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE, this->signedDistanceAboveSurface);
        xmlWriter.writeEndElement();
    }
}

void SurfaceProjectionBarycentric::readBorderFileXML1(QXmlStreamReader& xml)
{
    reset();
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("ProjectionBarycentric"));
    bool haveAreas = false, haveNodes = false, haveDist = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name == QLatin1String("TriangleAreas"))
                {
                    if (haveAreas) throw DataFileException("multiple TriangleAreas elements in one ProjectionBarycentric element");
                    QString text = xml.readElementText();//errors on unexpected element
                    if (xml.hasError()) throw DataFileException("XML parsing error in TriangleAreas: " + xml.errorString());
#if QT_VERSION >= 0x060000
                    QStringList areaStrings = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
#else
                    QStringList areaStrings = text.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
#endif
                    if (areaStrings.size() != 3) throw DataFileException("TriangleAreas element must contain 3 numbers separated by whitespace");
                    bool ok = false;
                    for (int i = 0; i < 3; ++i)
                    {
                        triangleAreas[i] = areaStrings[i].toFloat(&ok);
                        if (!ok) throw DataFileException("found non-numeric string in TriangleAreas: " + areaStrings[i]);
                    }
                    haveAreas = true;
                } else if (name == QLatin1String("TriangleNodes")) {
                    if (haveNodes) throw DataFileException("multiple TriangleNodes elements in one ProjectionBarycentric element");
                    QString text = xml.readElementText();//errors on unexpected element
                    if (xml.hasError()) throw DataFileException("XML parsing error in TriangleNodes: " + xml.errorString());
#if QT_VERSION >= 0x060000
                    QStringList nodeStrings = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
#else
                    QStringList nodeStrings = text.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
#endif
                    if (nodeStrings.size() != 3) throw DataFileException("TriangleNodes element must contain 3 integers separated by whitespace");
                    bool ok = false;
                    for (int i = 0; i < 3; ++i)
                    {
                        triangleNodes[i] = nodeStrings[i].toInt(&ok);
                        if (!ok) throw DataFileException("found non-integer string in TriangleNodes: " + nodeStrings[i]);
                    }
                    haveNodes = true;
                } else if (name == QLatin1String("SignedDistanceAboveSurface")) {
                    if (haveDist) throw DataFileException("multiple SignedDistanceAboveSurface elements in one ProjectionBarycentric element");
                    QString text = xml.readElementText();//errors on unexpected element
                    if (xml.hasError()) throw DataFileException("XML parsing error in SignedDistanceAboveSurface: " + xml.errorString());
                    bool ok = false;
                    signedDistanceAboveSurface = text.toFloat(&ok);
                    if (!ok) throw DataFileException("found non-numeric string in SignedDistanceAboveSurface: " + text);
                    haveDist = true;
                } else {
                    throw DataFileException("unexpected element in ProjectionBarycentric: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (xml.hasError()) throw DataFileException("XML parsing error in ProjectionBarycentric: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("ProjectionBarycentric"));
    if (!haveAreas || !haveNodes)//ignore missing distance? should always be zero for BorderFile anyway
    {
        throw DataFileException("SurfaceProjectionBarycentric element missing TriangleNodes and/or TriangleAreas");
    }
    projectionValid = true;
}
