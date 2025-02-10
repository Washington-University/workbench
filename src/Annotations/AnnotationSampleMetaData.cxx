
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ANNOTATION_SAMPLE_META_DATA_DECLARE__
#include "AnnotationSampleMetaData.h"
#undef __ANNOTATION_SAMPLE_META_DATA_DECLARE__

#include <QDate>

#include "AnnotationPolyhedron.h"
#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "HemisphereEnum.h"
#include "HtmlTableBuilder.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSampleMetaData 
 * \brief Wraps GIFTI metadata for use as metadata by an annotation sample
 * \ingroup Annotations
 */

/**
 * @return The first left hemisphere allen slab number
 */
int32_t 
AnnotationSampleMetaData::getFirstLeftHemisphereAllenSlabNumber()
{
    return s_firstLeftHemisphereAllenSlabNumber;
}
/**
 * @return The first right hemisphere allen slab number
 */
int32_t 
AnnotationSampleMetaData::getFirstRightHemisphereAllenSlabNumber()
{
    return s_firstRightHemisphereAllenSlabNumber;
}

/**
 * @return The last right hemisphere allen slab number
 */
int32_t 
AnnotationSampleMetaData::getLastRightHemisphereAllenSlabNumber()
{
    return s_lastRightHemisphereAllenSlabNumber;
}

/**
 * Set the first left hemisphere allen slab number
 * @param number
 *    New number
 */
void 
AnnotationSampleMetaData::setFirstLeftHemisphereAllenSlabNumber(const int32_t number)
{
    s_firstLeftHemisphereAllenSlabNumber = number;
}

/**
 * Set the first right hemisphere allen slab number
 * @param number
 *    New number
 */
void 
AnnotationSampleMetaData::setFirstRightHemisphereAllenSlabNumber(const int32_t number)
{
    s_firstRightHemisphereAllenSlabNumber = number;
}

/**
 * Set the last right hemisphere allen slab number
 * @param number
 *    New number
 */
void 
AnnotationSampleMetaData::setLastRightHemisphereAllenSlabNumber(const int32_t number)
{
    s_lastRightHemisphereAllenSlabNumber = number;
}

/**
 * Constructor.
 * @param metadata
 *    Metadata that this instance wraps
 */
AnnotationSampleMetaData::AnnotationSampleMetaData(GiftiMetaData* metadata)
: CaretObject(),
m_metadata(metadata)
{
    CaretAssert(m_metadata);
}

/**
 * Destructor.
 */
AnnotationSampleMetaData::~AnnotationSampleMetaData()
{
}

/**
 * Update the metadata that this instance wraps
 * @param metadata
 *    New metadata that is wrapped
 */
void
AnnotationSampleMetaData::updateMetaData(const GiftiMetaData* metadata) const
{
    m_metadata = const_cast<GiftiMetaData*>(metadata);
    CaretAssert(metadata);
}

/**
 * @return True if the metadata is valid
 * @param errorMessageOut
 *   Contains error information if validation fails
 */
bool
AnnotationSampleMetaData::validateMetaData(AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    int32_t allenSlabNumber(-1);
    bool validAllenSlabNumberFlag(false);
    AString allenSlabNumberText(getAllenSlabNumber().trimmed());
    if ( ! allenSlabNumberText.isEmpty()) {
        allenSlabNumber = allenSlabNumberText.toInt(&validAllenSlabNumberFlag);
    }
    

    if (getAllenLocalName().trimmed().isEmpty()) {
        errorMessageOut.appendWithNewLine("Allen Local Name is empty.");
    }
    if (getHemisphere().trimmed().isEmpty()) {
        errorMessageOut.appendWithNewLine("Hemisphere is invalid.");
    }
    if ( ! validAllenSlabNumberFlag) {
        errorMessageOut.appendWithNewLine("Allen Slab Number is invalid.");
    }
    if (getSampleType().trimmed().isEmpty()) {
        errorMessageOut.appendWithNewLine("Sample Type is empty.");
    }
    if ( getSampleNumber().trimmed().isEmpty()) {
        errorMessageOut.appendWithNewLine("Sample Number is empty.");
    }
    
    bool validHemFlag(false);
    HemisphereEnum::Enum hemisphere = HemisphereEnum::BOTH;
    const AString hemisphereString(getHemisphere().trimmed());
    if ( ! hemisphereString.isEmpty()) {
        hemisphere = HemisphereEnum::fromGuiName(hemisphereString,
                                                 &validHemFlag);
        if ( ! validHemFlag) {
            errorMessageOut.appendWithNewLine("Hemisphere is invalid.");
        }
    }
    
    if (validHemFlag
        && validAllenSlabNumberFlag) {
        AString hemMsg;
        switch (hemisphere) {
            case HemisphereEnum::BOTH:
                if ((allenSlabNumber < s_firstLeftHemisphereAllenSlabNumber)
                    || (allenSlabNumber > s_lastRightHemisphereAllenSlabNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(s_firstLeftHemisphereAllenSlabNumber)
                              + " and "
                              + AString::number(s_lastRightHemisphereAllenSlabNumber)
                              + " for BOTH hemispheres.");
                }
                break;
            case HemisphereEnum::LEFT:
                if ((allenSlabNumber < s_firstLeftHemisphereAllenSlabNumber)
                    || (allenSlabNumber >= s_firstRightHemisphereAllenSlabNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(s_firstLeftHemisphereAllenSlabNumber)
                              + " and "
                              + AString::number(s_firstRightHemisphereAllenSlabNumber)
                              + " for LEFT hemispheres.");
                }
                break;
            case HemisphereEnum::RIGHT:
                if ((allenSlabNumber < s_firstRightHemisphereAllenSlabNumber)
                    || (allenSlabNumber > s_lastRightHemisphereAllenSlabNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(s_firstRightHemisphereAllenSlabNumber)
                              + " and "
                              + AString::number(s_lastRightHemisphereAllenSlabNumber)
                              + " for RIGHT hemispheres.");
                }
                break;
        }
        
        if ( ! hemMsg.isEmpty()) {
            errorMessageOut.appendWithNewLine(hemMsg);
        }
    }
    
    
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    return true;
}

/**
 * @return All valid values for Allen Tissue Type
 */
std::vector<AString> 
AnnotationSampleMetaData::getAllValidAllenTissueTypeValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("BS");
    values.push_back("CB");
    values.push_back("CX");

    return values;
}

/**
 * @return All valid values for Hemisphere
 */
std::vector<AString>
AnnotationSampleMetaData::getAllValidHemisphereValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("left");
    values.push_back("right");
    values.push_back("both");

    return values;
}

/**
 * @return All valid values for Sample Type
 */

std::vector<AString> AnnotationSampleMetaData::getAllValidSampleTypeValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("Slab polyhedron");
    values.push_back("polygon");

    return values;
}

/**
 * @return All valid values for Slab Face
 */
std::vector<AString>
AnnotationSampleMetaData::getAllValidSlabFaceValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("Anterior");
    values.push_back("Posterior");
    values.push_back("Inferior");
    values.push_back("Superior");
    values.push_back("Left");
    values.push_back("Right");
    values.push_back("Other");

    return values;
}

/**
 * @return Date format for use in GUI
 */
AString
AnnotationSampleMetaData::getDateFormat()
{
    return SAMPLES_QT_DATE_FORMAT;
}

/**
 * Copy metadata from the given instance to me
 * @param obj
 *   Other instance from which metadata is copied
 */
void
AnnotationSampleMetaData::copyMetaData(const AnnotationSampleMetaData& obj)
{
    CaretAssert(m_metadata);
    CaretAssert(obj.m_metadata);
    *m_metadata = *obj.m_metadata;
    if (obj.m_metadata->isModified()) {
        m_metadata->setModified();
    }
}

/**
 * Copy metadata from the given instance to me for a new annotation
 * @param obj
 *   Other instance from which metadata is copied
 */
void
AnnotationSampleMetaData::copyMetaDataForNewAnnotation(const AnnotationSampleMetaData& obj)
{
    /*
     * Save dates and restore at end of this method
     */
    const AString actualDate(getActualSampleEditDate());
    const AString desiredDate(getDesiredSampleEditDate());
    
    copyMetaData(obj);
    
    for (int32_t i = 0; i < getNumberOfBorders(); i++) {
        setBorderName(i, "");
    }
    setComment("");
    for (int32_t i = 0; i < getNumberOfFoci(); i++) {
        setFocusName(i, "");
    }
    setHmbaParcelDingAbbreviation("");
    setHmbaParcelDingFullName("");
    setSampleNumber("");
    setSampleType("");
    
    setActualSampleEditDate(actualDate);
    setDesiredSampleEditDate(desiredDate);
    
    m_metadata->setModified();
}

/**
 * @return Value for metadata with the given name
 * @param currentMetaDataName
 *    Current name for metadata element
 * @param previousMetaDataNameOne
 *    A previous name for metadata element, use if currentMetaDataName not found
 * @param previousMetaDataNameTwo
 *    Another previous name for metadata element, use if currentMetaDataName and previousMetaDataNameOne not found
 */
AString
AnnotationSampleMetaData::get(const AString& currentMetaDataName,
                              const AString& previousMetaDataNameOne,
                              const AString& previousMetaDataNameTwo) const
{
    AString valueOut(m_metadata->get(currentMetaDataName).trimmed());
    if (valueOut.isEmpty()) {
        valueOut = m_metadata->get(previousMetaDataNameOne).trimmed();
        if (valueOut.isEmpty()) {
            valueOut = m_metadata->get(previousMetaDataNameTwo).trimmed();
        }
    }
    
    return valueOut;
}


/**
 * @return Value for metadata from the array with the given name and at the given index
 * @param index
 *    Index in the array
 * @param currentMetaDataName
 *    Current name for metadata element
 * @param previousMetaDataNameOne
 *    A previous name for metadata element, use if currentMetaDataName not found
 * @param previousMetaDataNameTwo
 *    Another previous name for metadata element, use if currentMetaDataName and previousMetaDataNameOne not found
 */
AString
AnnotationSampleMetaData::getArray(const int32_t index,
                                   const AString& currentMetaDataName,
                                   const AString& previousMetaDataNameOne,
                                   const AString& previousMetaDataNameTwo) const
{
    /*
     * Get with element index
     */
    AString value(get(createArrayElementName(currentMetaDataName, index),
                      createArrayElementName(previousMetaDataNameOne, index),
                      createArrayElementName(previousMetaDataNameTwo, index)));
    if (value.isEmpty()) {
        if (index == 0) {
            /*
             * If value not found for element index 0,
             * metadata may have been created with single element
             * before it became an array
             */
            value = get(currentMetaDataName,
                        previousMetaDataNameOne,
                        previousMetaDataNameTwo);
        }
    }
    
    return value;
}

/**
 * @return Metadata name for the given array and element index
 * @param arrayName
 *    Name of array
 * @param elementIndex
 *    Index of element
 */
AString
AnnotationSampleMetaData::createArrayElementName(const AString& arrayName,
                                                 const int32_t elementIndex) const
{
    return (arrayName
            + ":"
            + AString::number(elementIndex));
}


/**
 * Set the metadata with the given name
 * @param currentMetaDataName
 *    Name of metadata element
 * @param value
 *    Value of metadata element
 */
void
AnnotationSampleMetaData::set(const AString& currentMetaDataName,
                              const AString& value)
{
    m_metadata->set(currentMetaDataName,
                    value);
}

/**
 * Set the metadata with to the array with the given name and at the given index
 * @param index
 *    Index in the array7
 * @param currentMetaDataName
 *    Name of metadata element
 * @param value
 *    Value of metadata element
 */
void
AnnotationSampleMetaData::setArray(const int32_t index,
                                   const AString& currentMetaDataName,
                                   const AString& value)
{
    set(createArrayElementName(currentMetaDataName, index),
        value);
}

/**
 * @return Value for metadata with the given name as an Integer
 * @param currentMetaDataName
 *    Current name for metadata element
 * @param previousMetaDataNameOne
 *    A previous name for metadata element, use if currentMetaDataName not found
 * @param previousMetaDataNameTwo
 *    Another previous name for metadata element, use if currentMetaDataName and previousMetaDataNameOne not found
 */
int32_t
AnnotationSampleMetaData::getInt(const AString& currentMetaDataName,
               const AString& previousMetaDataNameOne,
               const AString& previousMetaDataNameTwo) const
{
    const AString textValue(get(currentMetaDataName,
                                previousMetaDataNameOne,
                                previousMetaDataNameTwo));
    
    int32_t valueOut(0);
    
    if ( ! textValue.isEmpty()) {
        bool validFlag(false);
        valueOut = textValue.toInt(&validFlag);
        if ( ! validFlag) {
            valueOut = 0;
        }
    }
    
    return valueOut;
}

/**
 * Set the metadata with the given name
 * @param currentMetaDataName
 *    Name of metadata element
 * @param value
 *    Integer alue of metadata element
 */
void
AnnotationSampleMetaData::setInt(const AString& currentMetaDataName,
                                 const int32_t value)
{
    const AString textValue(AString::number(value));
    set(currentMetaDataName,
        textValue);
}

/**
 * @return actual sample edit date
 */
AString
AnnotationSampleMetaData::getActualSampleEditDate() const
{
    return get(SAMPLES_ACTUAL_SAMPLE_EDIT_DATE);
}

/**
 * Set actual sample edit date
 * @param value
 *    New value
 */
void 
AnnotationSampleMetaData::setActualSampleEditDate(const AString& value)
{
    set(SAMPLES_ACTUAL_SAMPLE_EDIT_DATE,
        value);
}

/**
 * @return allen local name
 */
AString
AnnotationSampleMetaData::getAllenLocalName() const
{
    return get(SAMPLES_ALLEN_LOCAL_NAME,
               SAMPLES_OBSOLETE_DONOR_ID);
}

/**
 * Set allen local name
 * @param value
 *    New value
 */
void 
AnnotationSampleMetaData::setAllenLocalName(const AString& value)
{
    set(SAMPLES_ALLEN_LOCAL_NAME,
        value);
}

/**
 * @return allen slab number
 */
AString
AnnotationSampleMetaData::getAllenSlabNumber() const
{
    return get(SAMPLES_ALLEN_SLAB_NUMBER,
               SAMPLES_OBSOLETE_SLAB_ID);
}

/**
 * Set allen slab number
 * @param value
 *    New value
 */
void 
AnnotationSampleMetaData::setAllenSlabNumber(const AString& value)
{
    set(SAMPLES_ALLEN_SLAB_NUMBER,
        value);
}

/**
 * @return allen tissue type
 */
AString
AnnotationSampleMetaData::getAllenTissueType() const
{
    return get(SAMPLES_ALLEN_TISSUE_TYPE);
}

/**
 * Set allen tissue type
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setAllenTissueType(const AString& value)
{
    set(SAMPLES_ALLEN_TISSUE_TYPE,
        value);
}

/**
 * @return alternate parcellation
 */
AString
AnnotationSampleMetaData::getAlternateParcellation() const
{
    return get(SAMPLES_ALTERNATE_PARCELLATION);
}


/**
 * Set alternate parcellation
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setAlternateParcellation(const AString& value)
{
    set(SAMPLES_ALTERNATE_PARCELLATION,
        value);
}

/**
 * @return alternate sample name
 */
AString
AnnotationSampleMetaData::getAlternateSampleName() const
{
    return get(SAMPLES_ALTERNATE_SAMPLE_NAME);
}


/**
 * Set alternate sample name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setAlternateSampleName(const AString& value)
{
    set(SAMPLES_ALTERNATE_SAMPLE_NAME,
        value);
}

/**
 * @return BICAN donor ID
 */
AString
AnnotationSampleMetaData::getBicanDonorID() const
{
    return get(SAMPLES_BICAN_DONOR_ID,
               SAMPLES_OBSOLETE_BICAN_DONOR_ID,
               SAMPLES_OBSOLETE_DONOR_ID);
}


/**
 * Set BICAN donor ID
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBicanDonorID(const AString& value)
{
    set(SAMPLES_BICAN_DONOR_ID,
        value);
}

/**
 * @return Comment
 */
AString
AnnotationSampleMetaData::getComment() const
{
    return get(SAMPLES_COMMENT);
}


/**
 * Set comment
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setComment(const AString& value)
{
    set(SAMPLES_COMMENT,
        value);
}

/**
 * @return desired sample edit date
 */
AString
AnnotationSampleMetaData::getDesiredSampleEditDate() const
{
    return get(SAMPLES_DESIRED_SAMPLE_ENTRY_DATE,
               SAMPLES_OBSOLETE_ENTRY_DATE,
               SAMPLES_OBSOLETE_DISSECTION_DATE);
}

/**
 * Set desired sample edit date
 * @param value
 *    New value
 */
void 
AnnotationSampleMetaData::setDesiredSampleEditDate(const AString& value)
{
    set(SAMPLES_DESIRED_SAMPLE_ENTRY_DATE,
        value);
}

/**
 * @return hemisphere
 */
AString
AnnotationSampleMetaData::getHemisphere() const
{
    return get(SAMPLES_HEMISPHERE);
}

/**
 * Set hemisphere
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setHemisphere(const AString& value)
{
    set(SAMPLES_HEMISPHERE,
        value);
}

/**
 * @return HMBA parcel Ding abbreviation
 */
AString
AnnotationSampleMetaData::getHmbaParcelDingAbbreviation() const
{
    return get(SAMPLES_DING_ABBREVIATION,
               SAMPLES_OBSOLETE_SHORTHAND_ID);
}

/**
 * Set HMBA parcel Ding abbreviation
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setHmbaParcelDingAbbreviation(const AString& value)
{
    set(SAMPLES_DING_ABBREVIATION,
        value);
}


/**
 * @return HMBA parcel Ding full name
 */
AString
AnnotationSampleMetaData::getHmbaParcelDingFullName() const
{
    return get(SAMPLES_DING_FULL_NAME,
               SAMPLES_OBSOLETE_DING_DESCRIPTION);
}

/**
 * Set  HMBA parcel Ding full name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setHmbaParcelDingFullName(const AString& value)
{
    set(SAMPLES_DING_FULL_NAME,
        value);
}

/**
 * @return local sample ID that is a composite of other metadata items
 * @param polyhedron
 *    Polyhedron for getting type of polyhedron (actual / desired)
 */
AString
AnnotationSampleMetaData::getLocalSampleID(const AnnotationPolyhedron* polyhedron) const
{
    const AString separator(".");
    std::vector<AString> components;
    components.push_back(getLocalSlabID());
    components.push_back(get(SAMPLES_SAMPLE_NUMBER));
    switch (polyhedron->getPolyhedronType()) {
        case AnnotationPolyhedronTypeEnum::INVALID:
            break;
        case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
            components.push_back("A");
            break;
        case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
            components.push_back("D");
            break;
    }
    const AString valueOut = assembleCompositeElementComponents(components,
                                                                separator);
    return valueOut;
}

/**
 * @return local slab ID that is a composite of other metadata items
 */
AString
AnnotationSampleMetaData::getLocalSlabID() const
{
    const AString separator(".");
    std::vector<AString> components;
    components.push_back(get(SAMPLES_ALLEN_LOCAL_NAME));
    components.push_back(get(SAMPLES_ALLEN_TISSUE_TYPE));
    components.push_back(get(SAMPLES_ALLEN_SLAB_NUMBER));
    const AString valueOut = assembleCompositeElementComponents(components,
                                                                separator);
    return valueOut;
}

/**
 * @return NHash Slab ID
 */
AString
AnnotationSampleMetaData::getNHashSlabID() const
{
    return get(SAMPLES_NHASH_SLAB_ID);
}

/**
 * Set NHash Slab ID
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setNHashSlabID(const AString& value)
{
    set(SAMPLES_NHASH_SLAB_ID,
        value);
}


/**
 * @return primary parcellation
 */
AString
AnnotationSampleMetaData::getPrimaryParcellation() const
{
    return get(SAMPLES_PRIMARY_PARCELLATION,
               SAMPLES_OBSOLETE_ORIGINAL_PARCELLATION);
}

/**
 * Set  primary parcellation
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setPrimaryParcellation(const AString& value)
{
    set(SAMPLES_PRIMARY_PARCELLATION,
        value);
}

/**
 * @return sample name
 */
AString
AnnotationSampleMetaData::getSampleName() const
{
    return get(SAMPLES_SAMPLE_NAME);
}

/**
 * Set sample name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSampleName(const AString& value)
{
    set(SAMPLES_SAMPLE_NAME,
        value);
}

/**
 * @return sample number
 */
AString
AnnotationSampleMetaData::getSampleNumber() const
{
    return get(SAMPLES_SAMPLE_NUMBER,
               SAMPLES_OBSOLETE_SAMPLE_ID);
}

/**
 * Set sample number
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSampleNumber(const AString& value)
{
    set(SAMPLES_SAMPLE_NUMBER,
        value);
}

/**
 * @return sample type
 */
AString
AnnotationSampleMetaData::getSampleType() const
{
    AString txt(get(SAMPLES_SAMPLE_TYPE,
                    SAMPLES_OBSOLETE_SAMPLE_TYPE));
    if (txt == "Slap polyhedron") { /* correct spelling in old values */
        txt = "Slab polyhedron";
    }
    return txt;
}

/**
 * Set sample type
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSampleType(const AString& value)
{
    set(SAMPLES_SAMPLE_TYPE,
        value);
}


/**
 * @return slab face
 */
AString
AnnotationSampleMetaData::getSlabFace() const
{
    return get(SAMPLES_SLAB_FACE,
               SAMPLES_OBSOLETE_SLAB_FACE);
}

/**
 * Set slab face
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSlabFace(const AString& value)
{
    set(SAMPLES_SLAB_FACE,
        value);
}

/**
 * @return subject name
 */
AString
AnnotationSampleMetaData::getSubjectName() const
{
    return get(SAMPLES_SUBJECT_NAME);
}

/**
 * Set subject name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSubjectName(const AString& value)
{
    set(SAMPLES_SUBJECT_NAME,
        value);
}

/**
 * Number of borders
 */
int32_t
AnnotationSampleMetaData::getNumberOfBorders() const
{
    return s_numberOfBorders;
}

/**
 * @return border file name
 * @param borderIndex
 *    Index of the border
 */
AString
AnnotationSampleMetaData::getBorderFileName(const int32_t borderIndex) const
{
    return getArray(borderIndex,
                    SAMPLES_BORDER_FILENAME);
}

/**
 * Set border file name
 * @param borderIndex
 *    Index of the border
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderFileName(const int32_t borderIndex,
                                            const AString& value)
{
    setArray(borderIndex,
             SAMPLES_BORDER_FILENAME,
             value);
}

/**
 * @return border class
 * @param borderIndex
 *    Index of the border
 */
AString
AnnotationSampleMetaData::getBorderClass(const int32_t borderIndex) const
{
    return getArray(borderIndex,
                    SAMPLES_BORDER_CLASS);
}

/**
 * Set border class
 * @param borderIndex
 *    Index of the border
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderClass(const int32_t borderIndex,
                                         const AString& value)
{
    setArray(borderIndex,
             SAMPLES_BORDER_CLASS,
             value);
}

/**
 * @return border name
 * @param borderIndex
 *    Index of the border
 */
AString
AnnotationSampleMetaData::getBorderName(const int32_t borderIndex) const
{
    return getArray(borderIndex,
                    SAMPLES_BORDER_NAME);
}

/**
 * Set border name
 * @param borderIndex
 *    Index of the border
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderName(const int32_t borderIndex,
                                        const AString& value)
{
    setArray(borderIndex,
             SAMPLES_BORDER_NAME,
             value);
}

/**
 * @return border id
 * @param borderIndex
 *    Index of the border
 */
AString
AnnotationSampleMetaData::getBorderID(const int32_t borderIndex) const
{
    return getArray(borderIndex,
                    SAMPLES_BORDER_ID);
}

/**
 * Set border id
 * @param borderIndex
 *    Index of the border
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderID(const int32_t borderIndex,
                                      const AString& value)
{
    setArray(borderIndex,
             SAMPLES_BORDER_ID,
             value);
}

/**
 * @return The number of foci
 */
int32_t
AnnotationSampleMetaData::getNumberOfFoci() const
{
    return s_numberOfFoci;
}

/**
 * @return focus file name
 * @param focusIndex
 *    Index of the focus
 */
AString
AnnotationSampleMetaData::getFocusFileName(const int32_t focusIndex) const
{
    return getArray(focusIndex,
                    SAMPLES_FOCUS_FILENAME);
}

/**
 * Set focus file name
 * @param focusIndex
 *    Index of the focus
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusFileName(const int32_t focusIndex,
                                           const AString& value)
{
    setArray(focusIndex,
             SAMPLES_FOCUS_FILENAME,
             value);
}

/**
 * @return focus class
 * @param focusIndex
 *    Index of the focus
 */
AString
AnnotationSampleMetaData::getFocusClass(const int32_t focusIndex) const
{
    return getArray(focusIndex,
                    SAMPLES_FOCUS_CLASS);
}

/**
 * Set focus class
 * @param focusIndex
 *    Index of the focus
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusClass(const int32_t focusIndex,
                                        const AString& value)
{
    setArray(focusIndex,
             SAMPLES_FOCUS_CLASS,
             value);
}

/**
 * @return focus id
 * @param focusIndex
 *    Index of the focus
 */
AString
AnnotationSampleMetaData::getFocusID(const int32_t focusIndex) const
{
    return getArray(focusIndex,
                    SAMPLES_FOCUS_ID);
}

/**
 * Set focus id
 * @param focusIndex
 *    Index of the focus
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusID(const int32_t focusIndex,
                                     const AString& value)
{
    setArray(focusIndex,
             SAMPLES_FOCUS_ID,
             value);
}


/**
 * @return focus name
 * @param focusIndex
 *    Index of the focus
 */
AString
AnnotationSampleMetaData::getFocusName(const int32_t focusIndex) const
{
    return getArray(focusIndex,
                    SAMPLES_FOCUS_NAME);
}

/**
 * Set focus name
 * @param focusIndex
 *    Index of the focus
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusName(const int32_t focusIndex,
                                       const AString& value)
{
    setArray(focusIndex,
             SAMPLES_FOCUS_NAME,
             value);
}

/**
 * Assemble components for a composite metadata element
 * @param components
 *    The components assembled into the composite item
 * @param separator
 *    The separator between components
 * @return
 *    The composed data
 */
AString
AnnotationSampleMetaData::assembleCompositeElementComponents(const std::vector<AString>& components,
                                                             const AString& separator) const
{
    AString dataValue;
    
    for (const auto& comp : components) {
        if ( ! dataValue.isEmpty()) {
            dataValue.append(separator);
        }
        if (comp.isEmpty()) {
            dataValue.append("???");
        }
        else {
            dataValue.append(comp);
        }
    }
    
    return dataValue;
}

/**
 * Update the polyhedron's metadata
 */
void
AnnotationSampleMetaData::updateMetaDataWithNameChanges()
{
    if (m_metaDataHasBeenUpdatedFlag) {
        return;
    }
    m_metaDataHasBeenUpdatedFlag = true;
    
    if (m_metadata->exists(SAMPLES_OBSOLETE_ENTRY_DATE)) {
        /*
         * Convert date from old to new format
         */
        const QString dateText(m_metadata->get(SAMPLES_OBSOLETE_ENTRY_DATE));
        if ( ! dateText.isEmpty()) {
            const QString lowerText(dateText.toLower());
            if (lowerText.contains("jan")
                || lowerText.contains("feb")
                || lowerText.contains("mar")
                || lowerText.contains("apr")
                || lowerText.contains("may")
                || lowerText.contains("jun")
                || lowerText.contains("jul")
                || lowerText.contains("aug")
                || lowerText.contains("sep")
                || lowerText.contains("oct")
                || lowerText.contains("nov")
                || lowerText.contains("dec")) {
                const QDate date(QDate::fromString(dateText,
                                                   SAMPLES_OBSOLETE_QT_DATE_FORMAT));
                const QString newDateText(date.toString(SAMPLES_QT_DATE_FORMAT));
                m_metadata->set(SAMPLES_OBSOLETE_ENTRY_DATE,
                                newDateText);
            }
        }
    }
    
    /*
     * Renames elements
     */
    static std::map<AString, AString> oldNewNamesMap;
    if (oldNewNamesMap.empty()) {
        /*
         * Names of sample metadata have changed several times so update
         * old names to new names
         * Use static so only one map for all instances
         */
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_CASE_ID,
                               SAMPLES_SUBJECT_NAME);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_DONOR_ID,
                               SAMPLES_ALLEN_LOCAL_NAME);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_BICAN_DONOR_ID,
                               SAMPLES_BICAN_DONOR_ID);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_SLAB_ID,
                               SAMPLES_ALLEN_SLAB_NUMBER);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_SLAB_FACE,
                               SAMPLES_SLAB_FACE);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_DISSECTION_DATE,
                               SAMPLES_ACTUAL_SAMPLE_EDIT_DATE);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_SHORTHAND_ID,
                               SAMPLES_DING_ABBREVIATION);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_DING_DESCRIPTION,
                               SAMPLES_DING_FULL_NAME);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_SAMPLE_TYPE,
                               SAMPLES_SAMPLE_TYPE);
        oldNewNamesMap.emplace(SAMPLES_OBSOLETE_SAMPLE_ID,
                               SAMPLES_SAMPLE_NUMBER);
    }
    
    m_metadata->updateMetaDataNames(oldNewNamesMap);
    
    if (m_metadata->exists(SAMPLES_HEMISPHERE)) {
        /*
         * Hemisphere changed from (L, R) to (left, right, both)
         */
        AString hem(m_metadata->get(SAMPLES_HEMISPHERE));
        if (hem == "L") {
            m_metadata->set(SAMPLES_HEMISPHERE,
                "left");
        }
        else if (hem == "R") {
            m_metadata->set(SAMPLES_HEMISPHERE,
                "right");
        }
    }
}

/**
 * @return The metadata  in an HTML table format
 * @param polyhedron
 *    Polyhedron for getting type of polyhedron (actual / desired)
 */
AString
AnnotationSampleMetaData::toFormattedHtml(const AnnotationPolyhedron* polyhedron) const
{
    const int32_t numberOfColumns(2);
    HtmlTableBuilder tableBuilder(HtmlTableBuilder::V4_01,
                                  numberOfColumns);
    tableBuilder.addRow("Metadata");
    
    
    std::vector<std::pair<AString, AString>> namesAndValues;
    getAllMetaDataNamesAndValues(polyhedron,
                                 namesAndValues);
    
    for (const auto& nv : namesAndValues) {
        tableBuilder.addRow((nv.first + ": "), nv.second);
    }

    return tableBuilder.getAsHtmlTable();
}

/**
 * Get all metadata names and values
 * @param polyhedron
 *    Polyhedron for getting type of polyhedron (actual / desired)
 * @param namesAndValuesOut
 *    Pairs with names and values
 */
void
AnnotationSampleMetaData::getAllMetaDataNamesAndValues(const AnnotationPolyhedron* polyhedron,
                                                       std::vector<std::pair<AString, AString>>& namesAndValuesOut) const
{
    namesAndValuesOut.clear();
    
    namesAndValuesOut.emplace_back(getSubjectNameLabelText(), getSubjectName());
    
    namesAndValuesOut.emplace_back(getAllenLocalNameLabelText(), getAllenLocalName());
    
    namesAndValuesOut.emplace_back(getHemisphereLabelText(), getHemisphere());
    
    namesAndValuesOut.emplace_back(getAllenTissueTypeLabelText(), getAllenTissueType());
    
    namesAndValuesOut.emplace_back(getAllenSlabNumberLabelText(), getAllenSlabNumber());
    
    namesAndValuesOut.emplace_back(getLocalSlabIdLabelText(), getLocalSlabID());
    
    namesAndValuesOut.emplace_back(getSlabFaceLabelText(), getSlabFace());
    
    namesAndValuesOut.emplace_back(getSampleTypeLabelText(), getSampleType());
    
    namesAndValuesOut.emplace_back(getDesiredSampleEditDateLabelText(), getDesiredSampleEditDate());
    
    namesAndValuesOut.emplace_back(getActualSampleEditDateLabelText(), getActualSampleEditDate());
    
    namesAndValuesOut.emplace_back(getHmbaParcelDingAbbreviationLabelText(), getHmbaParcelDingAbbreviation());
    
    namesAndValuesOut.emplace_back(getHmbaParcelDingFullNameLabelText(), getHmbaParcelDingFullName());
    
    namesAndValuesOut.emplace_back(getSampleNameLabelText(), getSampleName());
    
    namesAndValuesOut.emplace_back(getAlternativeSampleNameLabelText(), getAlternateSampleName());
    
    namesAndValuesOut.emplace_back(getSampleNumberLabelText(), getSampleNumber());
    
    namesAndValuesOut.emplace_back(getLocalSampleIdLabelText(), getLocalSampleID(polyhedron));
    
    namesAndValuesOut.emplace_back(getPrimaryParcellationLabelText(), getPrimaryParcellation());
    
    namesAndValuesOut.emplace_back(getAlternativeParcellationLabelText(), getAlternateParcellation());
    
    namesAndValuesOut.emplace_back(getCommentLabelText(), getComment());
    
    for (int32_t i = 0; i < getNumberOfBorders(); i++) {
        namesAndValuesOut.emplace_back(getBorderFileNameLabelText(), getBorderFileName(i));
        
        namesAndValuesOut.emplace_back(getBorderIdLabelText(), getBorderID(i));
        
        namesAndValuesOut.emplace_back(getBorderNameLabelText(), getBorderName(i));
        
        namesAndValuesOut.emplace_back(getBorderClassLabelText(), getBorderClass(i));
    }
    
    for (int32_t i = 0; i < getNumberOfFoci(); i++) {
        namesAndValuesOut.emplace_back(getFocusIdLabelText(), getFocusID(i));
        
        namesAndValuesOut.emplace_back(getFocusFileNameLabelText(), getFocusFileName(i));
        
        namesAndValuesOut.emplace_back(getFocusNameLabelText(), getFocusName(i));
        
        namesAndValuesOut.emplace_back(getFocusClassLabelText(), getFocusClass(i));
    }
    
    namesAndValuesOut.emplace_back(getBicanDonorIdLabelText(), getBicanDonorID());
    
    namesAndValuesOut.emplace_back(getNhashSlabIdLabelText(), getNHashSlabID());
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationSampleMetaData::toString() const
{
    return "AnnotationSampleMetaData";
}

/**
 * @return Current date in a string
 */
AString
AnnotationSampleMetaData::getCurrentDateInString()
{
    return QDate::currentDate().toString(AnnotationSampleMetaData::getDateFormat());
}

/**
 * @return Invalid date in a string
 */
AString
AnnotationSampleMetaData::getInvalidDateInString()
{
    const int year(1970);
    const int month(1);
    const int day(1);
    return QDate(year, month, day).toString(AnnotationSampleMetaData::getDateFormat());
}
