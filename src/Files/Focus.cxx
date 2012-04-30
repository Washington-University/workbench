
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
    m_selectionClassNameModificationStatus = true; // name/class is new!!
    clear();
}

/**
 * Destructor.
 */
Focus::~Focus()
{
    clear();
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
    m_selectionClassNameModificationStatus = true; // name/class is new!!
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
    
    m_selectionClassKey = -1;
    m_selectionNameKey  = -1;
    
    setNameOrClassModified(); // new name/class so modified
    
    removeAllProjections();
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
    m_studyMetaDataLinkSet = new StudyMetaDataLinkSet(*m_studyMetaDataLinkSet);
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
    return m_nameRgbaColorValid;
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
void Focus::setClassRgba(const float rgba[3])
{
    m_classRgbaColor[0] = rgba[0];
    m_classRgbaColor[1] = rgba[1];
    m_classRgbaColor[2] = rgba[2];
    m_classRgbaColor[3] = rgba[2];
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
    m_nameRgbaColor[2] = rgba[2];
    m_nameRgbaColorValid = true;
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
 * Remove all of the projections.
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
                  const int32_t focusIndex) throw (XmlException)
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
    m_selectionClassNameModificationStatus = true;
}

/**
 * @return Is name/class modified?
 * 
 * Name/Class modification status is used
 * by the selection controls that display
 * borders based upon selected classes and
 * names.
 */
bool 
Focus::isSelectionClassOrNameModified() const
{
    return m_selectionClassNameModificationStatus;
}

/**
 * Set the keys for the border's name and class
 * used in the class and name hierarchy.
 * 
 * Name/Class modification status is used
 * by the selection controls that display
 * borders based upon selected classes and
 * names.
 *
 * @param selectionClassKey
 *     Key for the border class.
 * @param selectionNameKey
 *     Key for the border name.
 */
void
Focus::setSelectionClassAndNameKeys(const int32_t selectionClassKey,
                                    const int32_t selectionNameKey)
{
    m_selectionClassKey = selectionClassKey;
    m_selectionNameKey  = selectionNameKey;
    
    /*
     * Clear status 
     */
    m_selectionClassNameModificationStatus = false;
}

/**
 * @return The key for the border's name in the class and name hierarchy.
 */
int32_t
Focus::getSelectionNameKey() const
{
    return m_selectionNameKey;
}

/**
 * @return The key for the border's class in the class and name hierarchy.
 */
int32_t
Focus::getSelectionClassKey() const
{
    return m_selectionClassKey;
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


