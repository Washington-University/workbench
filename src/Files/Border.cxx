
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

#define __BORDER_DECLARE__
#include "Border.h"
#undef __BORDER_DECLARE__

#include "CaretAssert.h"
#include "SurfaceProjectedItem.h"
#include "XmlWriter.h"

using namespace caret;
    
/**
 * \class caret::Border 
 * \brief A border is a connected line segment on a source or volume.
 */

/**
 * Constructor.
 */
Border::Border()
: CaretObjectTracksModification()
{
    
}

/**
 * Destructor.
 */
Border::~Border()
{
    this->clear();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Border::Border(const Border& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperBorder(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Border&
Border::operator=(const Border& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperBorder(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Border::copyHelperBorder(const Border& obj)
{
    this->clear();
    
    const int32_t numPoints = obj.getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*obj.points[i]);
        this->addPoint(spi);
    }
    
    this->name = obj.name;
    
    this->clearModified();
}

/**
 * Clear the border.
 */
void 
Border::clear()
{
    const int32_t numPoints = this->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        delete this->points[i];
    }
    this->points.clear();
    
    this->name = "";
    
    this->setModified();
}

/**
 * @return the name of the border.
 */
AString 
Border::getName() const
{
    return this->name;
}

/**
 * Set the name of the border.
 * @param name
 *   New name for border.
 */
void 
Border::setName(const AString& name)
{
    this->name = name;
    this->setModified();
}

/**
 * @return Number of points in the border.
 */
int32_t
Border::getNumberOfPoints() const
{
    return this->points.size();
}

/**
 * Get the border point at the given index.
 * @param indx
 *   Index of desired border point.
 * @return
 *   Pointer to border point.
 */
const SurfaceProjectedItem* 
Border::getPoint(const int32_t indx) const
{
    CaretAssertVectorIndex(this->points, indx);
    return this->points[indx];
}

/**
 * Get the border point at the given index.
 * @param indx
 *   Index of desired border point.
 * @return
 *   Pointer to border point.
 */
SurfaceProjectedItem* 
Border::getPoint(const int32_t indx)
{
    CaretAssertVectorIndex(this->points, indx);
    return this->points[indx];
}

/**
 * Add a point to the border.  NOTE: the border
 * takes ownership of the point and will delete
 * it.  After calling this method DO NOT ever
 * use the point passed to this method.
 *
 * @param point
 *    Point that is added to the border.
 */
void 
Border::addPoint(SurfaceProjectedItem* point)
{
    this->points.push_back(point);
    this->setModified();
}

/**
 * Remove the point at the given index.
 * @param indx
 *    Index of point for removal.
 */
void 
Border::removePoint(const int32_t indx)
{
    CaretAssertVectorIndex(this->points, indx);
    delete this->points[indx];
    this->points.erase(this->points.begin() + indx);
    this->setModified();
}

/**
 * Remove the last point from the border.
 */
void 
Border::removeLastPoint()
{
    const int numPoints = this->getNumberOfPoints();
    if (numPoints > 0) {
        this->removePoint(numPoints - 1);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Border::toString() const
{
    return "Border";
}

/**
 * Write the border to the XML Writer.
 * @param xmlWriter
 *   Writer for XML output.
 */
void 
Border::writeAsXML(XmlWriter& xmlWriter) throw (XmlException)
{
    xmlWriter.writeStartElement(XML_TAG_BORDER);
    
    xmlWriter.writeElementCharacters(XML_TAG_NAME, this->name);
    
    const int32_t numPoints = this->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        this->points[i]->writeAsXML(xmlWriter);    
    }
    
    xmlWriter.writeEndElement();
}

