
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

#define __FOCUS_DECLARE__
#include "Focus.h"
#undef __FOCUS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "StudyMetaDataLinkSet.h"
#include "SurfaceProjectedItem.h"
#include "XmlAttributes.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::Focus 
 * \brief A Focus.
 */
/**
 * Constructor.
 */
Focus::Focus()
: CaretObjectTracksModification()
{
    m_studyMetaDataLinkSet = new StudyMetaDataLinkSet();
    clear();
}

/**
 * Destructor.
 */
Focus::~Focus()
{
    clear();
    removeAllProjections();
    delete m_studyMetaDataLinkSet;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Focus::Focus(const Focus& obj)
: CaretObjectTracksModification(obj)
{
    m_studyMetaDataLinkSet = new StudyMetaDataLinkSet();
    clear();
    this->copyHelperFocus(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Focus&
Focus::operator=(const Focus& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperFocus(obj);
    }
    return *this;    
}

/**
 * Clear the focus.
 */
void 
Focus::clear()
{
    m_area = "";
    m_className = "";
    m_comment = "";
    m_extent = 0.0;
    m_geography = "";
    m_name = "";
    m_regionOfInterest = "";
    m_searchXYZ[0] = 0.0;
    m_searchXYZ[1] = 0.0;
    m_searchXYZ[2] = 0.0;
    m_statistic = "";
    m_studyMetaDataLinkSet->clear();
    m_sumsIdNumber = "";
    m_sumsRepeatNumber = "";
    m_sumsParentFocusBaseId = "";
    m_sumsVersionNumber = "";
    m_sumsMSLID = "";
    m_attributeID = "";

    m_groupNameSelectionItem = NULL;
    
    m_nameRgbaColor[0] = 0.0;
    m_nameRgbaColor[1] = 0.0;
    m_nameRgbaColor[2] = 0.0;
    m_nameRgbaColor[3] = 1.0;
    m_nameRgbaColorValid = false;

    m_classRgbaColor[0] = 0.0;
    m_classRgbaColor[1] = 0.0;
    m_classRgbaColor[2] = 0.0;
    m_classRgbaColor[3] = 1.0;
    m_classRgbaColorValid = false;
    
    setNameOrClassModified(); // new name/class so modified
    
    removeAllProjections();
    addProjection(new SurfaceProjectedItem());
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Focus::copyHelperFocus(const Focus& focus)
{
    clear();
    
    m_area = focus.m_area;
    m_className = focus.m_className;
    m_comment = focus.m_comment;
    m_extent = focus.m_extent;
    m_geography = focus.m_geography;
    m_name = focus.m_name;
    m_regionOfInterest = focus.m_regionOfInterest;
    m_searchXYZ[0] = focus.m_searchXYZ[0];
    m_searchXYZ[1] = focus.m_searchXYZ[1];
    m_searchXYZ[2] = focus.m_searchXYZ[2];
    m_statistic = focus.m_statistic;
    delete m_studyMetaDataLinkSet;
    m_studyMetaDataLinkSet = new StudyMetaDataLinkSet(*focus.m_studyMetaDataLinkSet);
    m_sumsIdNumber = focus.m_sumsIdNumber;
    m_sumsRepeatNumber = focus.m_sumsRepeatNumber;
    m_sumsParentFocusBaseId = focus.m_sumsParentFocusBaseId;
    m_sumsVersionNumber = focus.m_sumsVersionNumber;
    m_sumsMSLID = focus.m_sumsMSLID;
    m_attributeID = focus.m_attributeID;
    
    this->removeAllProjections();
    
    const int numProj = focus.getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*focus.getProjection(i));
        this->addProjection(spi);
    }
    if (m_projections.empty()) {
        this->addProjection(new SurfaceProjectedItem());
    }
    
    setNameOrClassModified(); // new name/class so modified
}

/**
 * @return Class name
 */
AString
Focus::getClassName() const
{
    return m_className;
}

/**
 * Set the class name
 * @param className
 */
void
Focus::setClassName(const AString& className)
{
    if (m_className != className) {
        m_className = className;
        setNameOrClassModified();
        setModified();
    }
}

/**
 * @return Area
 */
AString
Focus::getArea() const
{
    return m_area;
}

/**
 * Set the area
 * @param area
 */
void
Focus::setArea(const AString& area)
{
    if (m_area != area) {
        m_area = area;
        setModified();
    }
}

/**
 * @return Comment
 */
AString
Focus::getComment() const
{
    return m_comment;
}

/**
 * Set the comment
 * @param 
 */
void
Focus::setComment(const AString& comment)
{
    if (m_comment != comment) {
        m_comment = comment;
        setModified();
    }
}

/**
 * @return Extent
 */
float
Focus::getExtent() const
{
    return m_extent;
}

/**
 * Set the extent
 * @param extent
 */
void
Focus::setExtent(const float extent)
{
    if (m_extent != extent) {
        m_extent = extent;
        setModified();
    }
}

/**
 * @return Geography
 */
AString
Focus::getGeography() const
{
    return m_geography;
}

/**
 * Set the geography.
 * @param geography
 */
void
Focus::setGeography(const AString& geography)
{
    m_geography = geography;
}

/**
 * @return Name
 */
AString
Focus::getName() const
{
    return m_name;
}

/**
 * Set the name
 * @param name
 */
void
Focus::setName(const AString& name)
{
    if (m_name != name) {
        m_name = name;
        setNameOrClassModified();
        setModified();
    }
}

/**
 * @return Region of interest
 */
AString
Focus::getRegionOfInterest() const
{
    return m_regionOfInterest;
}

/**
 * Set the region of interest
 * @param regionOfInterest
 */
void
Focus::setRegionOfInterest(const AString& regionOfInterest)
{
    if (m_regionOfInterest != regionOfInterest) {
        m_regionOfInterest = regionOfInterest;
        setModified();
    }
}

/**
 * @return Search coordinate
 */
const float*
Focus::getSearchXYZ() const
{
    /*
     * If not set, return stereotaxic coordinate
     */
    if ((m_searchXYZ[0] == 0.0)
        && (m_searchXYZ[1] == 0.0)
        && (m_searchXYZ[2] == 0.0)) {
        const float* stereoXYZ = m_projections[0]->getStereotaxicXYZ();
        return stereoXYZ;
    }
    
    return m_searchXYZ;
}

/**
 * Set the search XYZ
 * @param searchXYZ
 */
void
Focus::setSearchXYZ(const float searchXYZ[3])
{
    if ((m_searchXYZ[0] != searchXYZ[0])
        || (m_searchXYZ[1] != searchXYZ[1])
        || (m_searchXYZ[2] != searchXYZ[2])) {
        m_searchXYZ[0] = searchXYZ[0];
        m_searchXYZ[1] = searchXYZ[1];
        m_searchXYZ[2] = searchXYZ[2];
        setModified();
    }
}

/**
 * @return statistic
 */
AString
Focus::getStatistic() const
{
    return m_statistic;
}

/**
 * Set the statistic
 * @param statistic
 */
void
Focus::setStatistic(const AString& statistic)
{
    if (m_statistic != statistic) {
        m_statistic = statistic;
        setModified();
    }
}

/**
 * @return Is the class RGBA color valid?
 */
bool 
Focus::isClassRgbaValid() const
{
    return m_classRgbaColorValid;
}

/**
 * Set then class RGBA color invalid.
 */
void 
Focus::setClassRgbaInvalid()
{
    m_classRgbaColorValid = false;
}

/**
 * @return The class RGBA color components 
 * ranging zero to one.
 */
const float* 
Focus::getClassRgba() const
{
    return m_classRgbaColor;
}

/**
 * Get the class RGBA color components 
 * ranging zero to one.
 */
void
Focus::getClassRgba(float rgba[4]) const
{
    rgba[0] = m_classRgbaColor[0];
    rgba[1] = m_classRgbaColor[1];
    rgba[2] = m_classRgbaColor[2];
    rgba[3] = m_classRgbaColor[3];
}

/**
 * Set the RGBA color components assigned to the class.
 * @param rgba
 *     Red, green, blue, alpha ranging zero to one.
 */
void Focus::setClassRgba(const float rgba[4])
{
    m_classRgbaColor[0] = rgba[0];
    m_classRgbaColor[1] = rgba[1];
    m_classRgbaColor[2] = rgba[2];
    m_classRgbaColor[3] = rgba[3];
    m_classRgbaColorValid = true;
}

/**
 * @return Is the name RGBA color valid?
 */
bool 
Focus::isNameRgbaValid() const
{
    return m_nameRgbaColorValid;
}

/**
 * Set then name RGBA color invalid.
 */
void 
Focus::setNameRgbaInvalid()
{
    m_nameRgbaColorValid = false;
}

/**
 * @return The name RGBA color components 
 * ranging zero to one.
 */
const float* Focus::getNameRgba() const
{
    return m_nameRgbaColor;
}

/**
 * Get the name RGBA color components 
 * ranging zero to one.
 */
void
Focus::getNameRgba(float rgba[4]) const
{
    rgba[0] = m_nameRgbaColor[0];
    rgba[1] = m_nameRgbaColor[1];
    rgba[2] = m_nameRgbaColor[2];
    rgba[3] = m_nameRgbaColor[3];
}

/**
 * Set the RGBA color components assigned to the name.
 * @param rgba
 *     Red, green, blue, alpha ranging zero to one.
 */
void Focus::setNameRgba(const float rgba[4])
{
    m_nameRgbaColor[0] = rgba[0];
    m_nameRgbaColor[1] = rgba[1];
    m_nameRgbaColor[2] = rgba[2];
    m_nameRgbaColor[3] = rgba[3];
    m_nameRgbaColorValid = true;
}

/**
 * Set the selection item for the group/name hierarchy.
 *
 * @param item
 *     The selection item from the group/name hierarchy.
 */
void
Focus::setGroupNameSelectionItem(GroupAndNameHierarchyItem* item)
{
    m_groupNameSelectionItem = item;
}

/**
 * @return The selection item for the Group/Name selection hierarchy.
 *      May be NULL in some circumstances.
 */
const GroupAndNameHierarchyItem*
Focus::getGroupNameSelectionItem() const
{
    return m_groupNameSelectionItem;
}

/**
 * @return Sums ID Number
 */
AString
Focus::getSumsIdNumber() const
{
    return m_sumsIdNumber;
}

/**
 * Set the Sums ID Number
 * @param sumsIdNumber
 */
void
Focus::setSumsIdNumber(const AString& sumsIdNumber)
{
    if (m_sumsIdNumber != sumsIdNumber) {
        m_sumsIdNumber = sumsIdNumber;
        setModified();
    }
}

/**
 * @return Sums Repeat number
 */
AString
Focus::getSumsRepeatNumber() const
{
    return m_sumsRepeatNumber;
}

/**
 * Set the Sums Repeat Number
 * @param sumsRepeatNumber
 */
void
Focus::setSumsRepeatNumber(const AString& sumsRepeatNumber)
{
    if (m_sumsRepeatNumber != sumsRepeatNumber) {
        m_sumsRepeatNumber = sumsRepeatNumber;
        setModified();
    }
}

/**
 * @return Sums parent focus base id
 */
AString
Focus::getSumsParentFocusBaseId() const
{
    return m_sumsParentFocusBaseId;
}

/**
 * Set the Sums Parent Focus Base ID
 * @param sumsParentFocusBaseId
 */
void
Focus::setSumsParentFocusBaseId(const AString& sumsParentFocusBaseId)
{
    if (m_sumsParentFocusBaseId != sumsParentFocusBaseId) {
        m_sumsParentFocusBaseId = sumsParentFocusBaseId;
        setModified();
    }
}

/**
 * @return Sums version number
 */
AString
Focus::getSumsVersionNumber() const
{
    return m_sumsVersionNumber;
}

/**
 * Set the Sums version number
 * @param sumsVersionNumber
 */
void
Focus::setSumsVersionNumber(const AString& sumsVersionNumber)
{
    if (m_sumsVersionNumber != sumsVersionNumber) {
        m_sumsVersionNumber = sumsVersionNumber;
        setModified();
    }
}

/**
 * @return Sums MSLID
 */
AString
Focus::getSumsMSLID() const
{
    return m_sumsMSLID;
}

/**
 * Set the Sums MSLID
 * @param sumsMSLID
 */
void
Focus::setSumsMSLID(const AString& sumsMSLID)
{
    if (m_sumsMSLID != sumsMSLID) {
        m_sumsMSLID = sumsMSLID;
        setModified();
    }
}

/**
 * @return Sums attribute ID
 */
AString
Focus::getSumsAttributeID() const
{
    return m_attributeID;
}

/**
 * Set the Atribute ID
 * @param attributeID
 */
void
Focus::setSumsAttributeID(const AString& attributeID)
{
    if (m_attributeID != attributeID) {
        m_attributeID = attributeID;
        setModified();
    }
}

/**
 * @return Number of projections
 */
int32_t
Focus::getNumberOfProjections() const
{
    return m_projections.size();
}

/**
 * Get the projection at the given index.
 * Note: Index 0 will ALWAYS return a valid projection.
 *
 * @param indx
 *    Index of projection
 * @return
 *    Projection at given index.
 */
const SurfaceProjectedItem*
Focus::getProjection(const int32_t indx) const
{
    CaretAssertVectorIndex(m_projections, indx);
    return m_projections[indx];
}

/**
 * Get the projection at the given index.
 * Note: Index 0 will ALWAYS return a valid projection.
 *
 * @param indx
 *    Index of projection
 * @return
 *    Projection at given index.
 */
SurfaceProjectedItem*
Focus::getProjection(const int32_t indx)
{
    CaretAssertVectorIndex(m_projections, indx);
    return m_projections[indx];
}

/**
 * Add the projection.   Note: the focus
 * takes ownership of the projection and will
 * delete it.  After calling this method DO NOT
 * ever use the projection passed to this method.
 *
 * @param projection
 *    Projection that is added.
 */
void 
Focus::addProjection(SurfaceProjectedItem* projection)
{
    CaretAssert(projection);
    m_projections.push_back(projection);
}

/**
 * Remove all of the projections.  A focus always
 * has one projection but this method removes all
 * projections so caller will need to to add a 
 * projection.
 */
void 
Focus::removeAllProjections()
{
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        delete m_projections[i];
    }
    m_projections.clear();
}

/**
 * Remove all but the first projection.
 */
void
Focus::removeExtraProjections()
{
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 1; i < numProj; i++) {
        delete m_projections[i];
    }
    m_projections.resize(1);
}

/**
 * @return The study meta data link set.
 */
StudyMetaDataLinkSet*
Focus::getStudyMetaDataLinkSet()
{
    return m_studyMetaDataLinkSet;
}

/**
 * @return The study meta data link set.
 */
const StudyMetaDataLinkSet*
Focus::getStudyMetaDataLinkSet() const
{
    return m_studyMetaDataLinkSet;
}

/**
 * Write the focus to XML.
 * @param xmlWriter
 *    Writer to which focus is written.
 * @param focusIndex
 *    Index of the focus.
 */
void 
Focus::writeAsXML(XmlWriter& xmlWriter,
                  const int32_t focusIndex)
{
    XmlAttributes atts;
    atts.addAttribute(XML_ATTRIBUTE_FOCUS_INDEX, focusIndex);
    xmlWriter.writeStartElement(XML_TAG_FOCUS,
                                atts);
    
    xmlWriter.writeElementCData(XML_TAG_AREA, m_area);
    xmlWriter.writeElementCData(XML_TAG_CLASS_NAME, m_className);
    xmlWriter.writeElementCData(XML_TAG_COMMENT, m_comment);
    xmlWriter.writeElementCharacters(XML_TAG_EXTENT, m_extent);
    xmlWriter.writeElementCData(XML_TAG_GEOGRAPHY, m_geography);
    xmlWriter.writeElementCData(XML_TAG_NAME, m_name);
    xmlWriter.writeElementCData(XML_TAG_REGION_OF_INTEREST, m_regionOfInterest);
    xmlWriter.writeElementCharacters(XML_TAG_SEARCH_XYZ, m_searchXYZ, 3);
    xmlWriter.writeElementCData(XML_TAG_STATISTIC, m_statistic);
    xmlWriter.writeElementCData(XML_TAG_SUMS_ID_NUMBER, m_sumsIdNumber);
    xmlWriter.writeElementCData(XML_TAG_SUMS_REPEAT_NUMBER, m_sumsRepeatNumber);
    xmlWriter.writeElementCData(XML_TAG_SUMS_PARENT_FOCUS_BASE_ID, m_sumsParentFocusBaseId);
    xmlWriter.writeElementCData(XML_TAG_SUMS_VERSION_NUMBER, m_sumsVersionNumber);
    xmlWriter.writeElementCData(XML_TAG_SUMS_MSLID, m_sumsMSLID);
    xmlWriter.writeElementCData(XML_TAG_SUMS_ATTRIBUTE_ID, m_attributeID);
    
    m_studyMetaDataLinkSet->writeXML(xmlWriter);
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        m_projections[i]->writeAsXML(xmlWriter);
    }
    
    xmlWriter.writeEndElement();
}

/**
 * Set modification status of name/class to modified.
 * 
 * Name/Class modification status is used
 * by the selection controls that display
 * borders based upon selected classes and
 * names.
 */
void 
Focus::setNameOrClassModified()
{
    m_groupNameSelectionItem = NULL;
    m_nameRgbaColorValid = false;
    m_classRgbaColorValid = false;
}

/**
 * Set an element from its XML tag name
 * @param elementName
 *    Name of XML element.
 * @param textValue
 *    Value of element's text.
 * @return
 *    True if element was valid, else false.
 */
bool 
Focus::setElementFromText(const AString& elementName,
                          const AString& textValue)
{
    if (elementName == Focus::XML_TAG_AREA) {
        m_area = textValue;
    }
    else if (elementName == Focus::XML_TAG_CLASS_NAME) {
        m_className = textValue;
    }
    else if (elementName == "Color") {
        // obsolete element
    }
    else if (elementName == Focus::XML_TAG_COMMENT) {
        m_comment = textValue;
    }
    else if (elementName == Focus::XML_TAG_EXTENT) {
        m_extent = textValue.toFloat();
    }
    else if (elementName == Focus::XML_TAG_GEOGRAPHY) {
        m_geography = textValue;
    }
    else if (elementName == Focus::XML_TAG_NAME) {
        m_name = textValue;
    }
    else if (elementName == Focus::XML_TAG_REGION_OF_INTEREST) {
        m_regionOfInterest = textValue;
    }
    else if (elementName == Focus::XML_TAG_SEARCH_XYZ) {
        std::vector<float> xyz;
        AString::toNumbers(textValue, xyz);
        if (xyz.size() == 3) {
            m_searchXYZ[0] = xyz[0];
            m_searchXYZ[1] = xyz[1];
            m_searchXYZ[2] = xyz[2];
        }
    }
    else if (elementName == Focus::XML_TAG_STATISTIC) {
        m_statistic = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_ID_NUMBER) {
        m_sumsIdNumber = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_REPEAT_NUMBER) {
        m_sumsRepeatNumber = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_PARENT_FOCUS_BASE_ID) {
        m_sumsParentFocusBaseId = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_VERSION_NUMBER) {
        m_sumsVersionNumber = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_MSLID) {
        m_sumsMSLID = textValue;
    }
    else if (elementName == Focus::XML_TAG_SUMS_ATTRIBUTE_ID) {
        m_attributeID = textValue;
    }
    else {
        return false;
    }

    return true;
}

/**
 * Clear the modification status of the focus.
 */
void
Focus::clearModified()
{
    CaretObjectTracksModification::clearModified();
    
    const int numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        SurfaceProjectedItem* spi = getProjection(i);
        spi->clearModified();
    }
}

/**
 * @return The modification status.
 */
bool
Focus::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    const int numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        const SurfaceProjectedItem* spi = getProjection(i);
        if (spi->isModified()) {
            return true;
        }
    }
    
    return false;
}

