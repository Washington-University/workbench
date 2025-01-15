
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
    copyMetaData(obj);
    
    setBorderName("");
    setComment("");
    setFocusName("");
    setHmbaParcelDingAbbreviation("");
    setHmbaParcelDingFullName("");
    setSampleNumber("");
    setSampleType("");
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
 */
AString
AnnotationSampleMetaData::getLocalSampleID() const
{
    const AString separator(".");
    std::vector<AString> components;
    components.push_back(getLocalSlabID());
    components.push_back(get(SAMPLES_SAMPLE_NUMBER));
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
 * @return The text name for the given sample location
 * @param location
 *   The location
 */
AString
AnnotationSampleMetaData::getSampleLocationText(AnnotationSampleMetaData::LocationEnum location)
{
    AString text("Invalid");
    switch (location) {
        case LocationEnum::ACTUAL:
            text = "Actual";
            break;
        case LocationEnum::DESIRED:
            text = "Desired";
            break;
        case LocationEnum::UNKNOWN:
            text = "Unknown";
            break;
    }
    return text;
}

/**
 * @return sample location
 */
AnnotationSampleMetaData::LocationEnum
AnnotationSampleMetaData::getSampleLocation() const
{
    LocationEnum location(LocationEnum::UNKNOWN);
    
    const AString locationText(get(SAMPLES_LOCATION).toLower());
    if (locationText == "actual") {
        location = LocationEnum::ACTUAL;
    }
    else if (locationText == "desired") {
        location = LocationEnum::DESIRED;
    }
    
    return location;
}

/**
 * Set sample location
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setSampleLocation(const LocationEnum value)
{
    AString locationText("");
    switch (value) {
        case LocationEnum::ACTUAL:
            locationText = "Actual";
            break;
        case LocationEnum::DESIRED:
            locationText = "Desired";
            break;
        case LocationEnum::UNKNOWN:
            break;
    }
    set(SAMPLES_LOCATION,
        locationText);
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
 * @return border file name
 */
AString
AnnotationSampleMetaData::getBorderFileName() const
{
    return get(SAMPLES_BORDER_FILENAME);
}

/**
 * Set border file name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderFileName(const AString& value)
{
    set(SAMPLES_BORDER_FILENAME,
        value);
}

/**
 * @return border class
 */
AString
AnnotationSampleMetaData::getBorderClass() const
{
    return get(SAMPLES_BORDER_CLASS);
}

/**
 * Set border class
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderClass(const AString& value)
{
    set(SAMPLES_BORDER_CLASS,
        value);
}

/**
 * @return border name
 */
AString
AnnotationSampleMetaData::getBorderName() const
{
    return get(SAMPLES_BORDER_NAME);
}

/**
 * Set border name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderName(const AString& value)
{
    set(SAMPLES_BORDER_NAME,
        value);
}

/**
 * @return border id
 */
AString
AnnotationSampleMetaData::getBorderID() const
{
    return get(SAMPLES_BORDER_ID);
}

/**
 * Set border id
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setBorderID(const AString& value)
{
    set(SAMPLES_BORDER_ID,
        value);
}

/**
 * @return focus file name
 */
AString
AnnotationSampleMetaData::getFocusFileName() const
{
    return get(SAMPLES_FOCUS_FILENAME);
}

/**
 * Set focus file name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusFileName(const AString& value)
{
    set(SAMPLES_FOCUS_FILENAME,
        value);
}

/**
 * @return focus class
 */
AString
AnnotationSampleMetaData::getFocusClass() const
{
    return get(SAMPLES_FOCUS_CLASS);
}

/**
 * Set focus class
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusClass(const AString& value)
{
    set(SAMPLES_FOCUS_CLASS,
        value);
}

/**
 * @return focus id
 */
AString
AnnotationSampleMetaData::getFocusID() const
{
    return get(SAMPLES_FOCUS_ID);
}

/**
 * Set focus id
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusID(const AString& value)
{
    set(SAMPLES_FOCUS_ID,
        value);
}


/**
 * @return focus name
 */
AString
AnnotationSampleMetaData::getFocusName() const
{
    return get(SAMPLES_FOCUS_NAME);
}

/**
 * Set focus name
 * @param value
 *    New value
 */
void
AnnotationSampleMetaData::setFocusName(const AString& value)
{
    set(SAMPLES_FOCUS_NAME,
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
 */
AString
AnnotationSampleMetaData::toFormattedHtml() const
{
    const int32_t numberOfColumns(2);
    HtmlTableBuilder tableBuilder(HtmlTableBuilder::V4_01,
                                  numberOfColumns);
    tableBuilder.addRow("Metadata");
    
    
    std::vector<std::pair<AString, AString>> namesAndValues;
    getAllMetaDataNamesAndValues(namesAndValues);
    
    for (const auto& nv : namesAndValues) {
        tableBuilder.addRow((nv.first + ": "), nv.second);
    }

    return tableBuilder.getAsHtmlTable();
}

/**
 * Get all metadata names and values
 * @param namesAndValuesOut
 *    Pairs with names and values
 */
void
AnnotationSampleMetaData::getAllMetaDataNamesAndValues(std::vector<std::pair<AString, AString>>& namesAndValuesOut) const
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
    
    namesAndValuesOut.emplace_back(getLocationLabelText(), getSampleLocationText(getSampleLocation()));
    
    namesAndValuesOut.emplace_back(getDesiredSampleEditDateLabelText(), getDesiredSampleEditDate());
    
    namesAndValuesOut.emplace_back(getActualSampleEditDateLabelText(), getActualSampleEditDate());
    
    namesAndValuesOut.emplace_back(getHmbaParcelDingAbbreviationLabelText(), getHmbaParcelDingAbbreviation());
    
    namesAndValuesOut.emplace_back(getHmbaParcelDingFullNameLabelText(), getHmbaParcelDingFullName());
    
    namesAndValuesOut.emplace_back(getSampleNameLabelText(), getSampleName());
    
    namesAndValuesOut.emplace_back(getAlternativeSampleNameLabelText(), getAlternateSampleName());
    
    namesAndValuesOut.emplace_back(getSampleNumberLabelText(), getSampleNumber());
    
    namesAndValuesOut.emplace_back(getLocalSampleIdLabelText(), getLocalSampleID());
    
    namesAndValuesOut.emplace_back(getPrimaryParcellationLabelText(), getPrimaryParcellation());
    
    namesAndValuesOut.emplace_back(getAlternativeParcellationLabelText(), getAlternateParcellation());
    
    namesAndValuesOut.emplace_back(getCommentLabelText(), getComment());
    
    namesAndValuesOut.emplace_back(getBorderFileNameLabelText(), getBorderFileName());
    
    namesAndValuesOut.emplace_back(getBorderIdLabelText(), getBorderID());
    
    namesAndValuesOut.emplace_back(getBorderNameLabelText(), getBorderName());
    
    namesAndValuesOut.emplace_back(getBorderClassLabelText(), getBorderClass());
    
    namesAndValuesOut.emplace_back(getFocusIdLabelText(), getFocusID());
    
    namesAndValuesOut.emplace_back(getFocusFileNameLabelText(), getFocusFileName());
    
    namesAndValuesOut.emplace_back(getFocusNameLabelText(), getFocusName());
    
    namesAndValuesOut.emplace_back(getFocusClassLabelText(), getFocusClass());
    
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

