
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

#define __ANNOTATION_META_DATA_DECLARE__
#include "AnnotationMetaData.h"
#undef __ANNOTATION_META_DATA_DECLARE__

#include <QDate>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiMetaDataXmlElements.h"
#include "HemisphereEnum.h"

using namespace caret;
    
/**
 * \class caret::AnnotationMetaData 
 * \brief Extends GiftiMetaData with Annotation specialization
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param annotationType
 *    Type of annotation
 */
AnnotationMetaData::AnnotationMetaData(const AnnotationTypeEnum::Enum annotationType)
: GiftiMetaData(),
m_annotationType(annotationType)
{
    
}

/**
 * Destructor.
 */
AnnotationMetaData::~AnnotationMetaData()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationMetaData::AnnotationMetaData(const AnnotationMetaData& obj)
: GiftiMetaData(obj)
{
    this->copyHelperAnnotationMetaData(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationMetaData&
AnnotationMetaData::operator=(const AnnotationMetaData& obj)
{
    if (this != &obj) {
        GiftiMetaData::operator=(obj);
        this->copyHelperAnnotationMetaData(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationMetaData::copyHelperAnnotationMetaData(const AnnotationMetaData& obj)
{
    m_annotationType = obj.m_annotationType;
}

/**
 * @return A copy of this instance that should be overridden by subclasses
 * so that the copy is of the same type as 'this'.
 */
GiftiMetaData*
AnnotationMetaData::clone() const
{
    return new AnnotationMetaData(*this);
}


///**
// * Equality operator.
// * @param obj
// *    Instance compared to this for equality.
// * @return 
// *    True if this instance and 'obj' instance are considered equal.
// */
//bool
//AnnotationMetaData::operator==(const AnnotationMetaData& obj) const
//{
//    return GiftiMetaData::operator==(obj);
//}

/**
 * Performs any needed processing after metadata is read.
 * Intended for overrding by subclasses.
 */
void
AnnotationMetaData::afterReadingProcessing()
{
    GiftiMetaData::afterReadingProcessing();
    
    /*
     * Only update a samples files (samples file is derived
     * from annotation file)
     */
    if (m_annotationType == AnnotationTypeEnum::POLYHEDRON) {
        updatePolyhedronMetaData();
    }
}

/**
 * Update the polyhedron's metadata
 */
void
AnnotationMetaData::updatePolyhedronMetaData()
{
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
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_CASE_ID,
                               GiftiMetaDataXmlElements::SAMPLES_SUBJECT_NAME);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_DONOR_ID,
                               GiftiMetaDataXmlElements::SAMPLES_ALLEN_LOCAL_NAME);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_BICAN_DONOR_ID,
                               GiftiMetaDataXmlElements::SAMPLES_BICAN_DONOR_ID);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_SLAB_ID,
                               GiftiMetaDataXmlElements::SAMPLES_ALLEN_SLAB_NUMBER);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_SLAB_FACE,
                               GiftiMetaDataXmlElements::SAMPLES_SLAB_FACE);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_DISSECTION_DATE,
                               GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_SHORTHAND_ID,
                               GiftiMetaDataXmlElements::SAMPLES_DING_ABBREVIATION);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_DING_DESCRIPTION,
                               GiftiMetaDataXmlElements::SAMPLES_DING_FULL_NAME);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_SAMPLE_TYPE,
                               GiftiMetaDataXmlElements::SAMPLES_SAMPLE_TYPE);
        oldNewNamesMap.emplace(GiftiMetaDataXmlElements::SAMPLES_OBSOLETE_SAMPLE_ID,
                               GiftiMetaDataXmlElements::SAMPLES_SAMPLE_NUMBER);
    }
    
    updateMetaDataNames(oldNewNamesMap);
    
    if (exists(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE)) {
        /*
         * Hemisphere changed from (L, R) to (left, right, both)
         */
        AString hem(get(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE));
        if (hem == "L") {
            set(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE,
                "left");
        }
        else if (hem == "R") {
            set(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE,
                "right");
        }
    }
    
    if (exists(GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE)) {
        /*
         * Convert date from old to new format
         */
        const QString dateText(get(GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE));
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
                                                   GiftiMetaDataXmlElements::METADATA_OBSOLETE_QT_DATE_FORMAT));
                const QString newDateText(date.toString(GiftiMetaDataXmlElements::METADATA_QT_DATE_FORMAT));
                set(GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE,
                    newDateText);
            }
        }
    }
    
    /*
     * Metadata that has been removed
     */
    remove(GiftiMetaDataXmlElements::SAMPLES_REMOVED_ALT_ATLAS_DESCRIPTION);
    remove(GiftiMetaDataXmlElements::SAMPLES_REMOVED_ORIG_ATLAS_NAME);
    remove(GiftiMetaDataXmlElements::SAMPLES_REMOVED_ORIG_SHORTHAND_ID);
}

/**
 * @return The data type for metadata with the given name.  Default is TEXT.
 * This method is intended for overriding by subclasses.
 * @param metaDataName
 *    Name of metadata element.
 */
GiftiMetaDataElementDataTypeEnum::Enum
AnnotationMetaData::getDataTypeForMetaDataName(const QString& metaDataName) const
{
    if (s_metaDataNameToDataTypeMap.empty()) {
        /*
         * Use static so only one map for all instances
         */
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE,
                                            GiftiMetaDataElementDataTypeEnum::DATE);
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT,
                                            GiftiMetaDataElementDataTypeEnum::COMMENT);
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::SAMPLES_ALT_SHORTHAND_ID,
                                            GiftiMetaDataElementDataTypeEnum::LABEL_ID_NAME);
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::SAMPLES_DING_ABBREVIATION,
                                            GiftiMetaDataElementDataTypeEnum::DING_ONTOLOGY_TERM);
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::SAMPLES_ALTERNATE_PARCELLATION,
                                            GiftiMetaDataElementDataTypeEnum::LABEL_FILE_AND_MAP);
        s_metaDataNameToDataTypeMap.emplace(GiftiMetaDataXmlElements::SAMPLES_ORIGINAL_PARCELLATION,
                                            GiftiMetaDataElementDataTypeEnum::LABEL_FILE_AND_MAP);
    }
    GiftiMetaDataElementDataTypeEnum::Enum dataType(GiftiMetaDataElementDataTypeEnum::TEXT);
    
    const auto iter(s_metaDataNameToDataTypeMap.find(metaDataName));
    if (iter != s_metaDataNameToDataTypeMap.end()) {
        dataType = iter->second;
    }
    else {
        const QStringList valuesList(getValidValuesListForMetaDataName(metaDataName));
        if ( ! valuesList.isEmpty()) {
            dataType = GiftiMetaDataElementDataTypeEnum::LIST;
        }
    }

    return dataType;
}

/**
 * @return Some metadata items may have a specific list of valid values.  Default is empty list.
 * This method is intended for overriding by subclasses.
 * @param metaDataName
 *    Name of metadata element.
 */
QStringList
AnnotationMetaData::getValidValuesListForMetaDataName(const QString& metaDataName) const
{
    /*
     * While these could be put in static storage, they are called
     * only when the user is editing samples metadata where they
     * have no noticable effect on performance.
     */
    QStringList metaDataValues;
    if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_ALLEN_TISSUE_TYPE) {
        metaDataValues.push_back("BS");
        metaDataValues.push_back("CB");
        metaDataValues.push_back("CX");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_LOCATION) {
        metaDataValues.push_back("Actual");
        metaDataValues.push_back("Desired");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_SAMPLE_TYPE) {
        metaDataValues.push_back("Slab polyhedron");
        metaDataValues.push_back("polygon");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_SLAB_FACE) {
        metaDataValues.push_back("Anterior");
        metaDataValues.push_back("Posterior");
        metaDataValues.push_back("Inferior");
        metaDataValues.push_back("Superior");
        metaDataValues.push_back("Left");
        metaDataValues.push_back("Right");
        metaDataValues.push_back("Other");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE) {
        std::vector<HemisphereEnum::Enum> hemisphereEnums;
        HemisphereEnum::getAllEnums(hemisphereEnums);
        for (const HemisphereEnum::Enum h : hemisphereEnums) {
            metaDataValues.push_back(HemisphereEnum::toLowerCaseGuiName(h));
        }
    }
    return metaDataValues;
}

/**
 * Get metadata names for display in a metadata editor.
 * This method is intended for overriding by subclasses.
 * @param metaDataNames
 *    Names of elements that are to be displayed in a metadata editor.
 * @param requiredMetaDataNames
 *    Names of elements that are required and the editor should require the user to enter values.
 *    This is a subset of metaDataNames
 */
void
AnnotationMetaData::getMetaDataNamesForEditor(std::vector<AString>& metaDataNamesOut,
                                              std::vector<AString>& requiredMetaDataNamesOut) const
{
    metaDataNamesOut.clear();
    requiredMetaDataNamesOut.clear();
    
    switch (m_annotationType) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            if (s_polyhedronEditorMetaDataNames.empty()) {
                /*
                 * Use static so only one vector for all instances
                 */
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SUBJECT_NAME);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_LOCAL_NAME);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_BICAN_DONOR_ID);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_TISSUE_TYPE);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_SLAB_NUMBER);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_LOCAL_SLAB_ID);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_NHASH_SLAB_ID);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SLAB_FACE);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_LOCATION);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ENTRY_DATE);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_DING_ABBREVIATION);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_DING_FULL_NAME);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALT_SHORTHAND_ID);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SAMPLE_TYPE);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SAMPLE_NUMBER);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_LOCAL_SAMPLE_ID);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ORIGINAL_PARCELLATION);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALTERNATE_PARCELLATION);
                s_polyhedronEditorMetaDataNames.push_back(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT);
            }
            
            if (s_polyhedronEditorRequiredMetaDataNames.empty()) {
                /*
                 * Use static so only one vector for all instances
                 */
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_LOCAL_NAME);
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE);
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_TISSUE_TYPE);
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_ALLEN_SLAB_NUMBER);
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SAMPLE_TYPE);
                s_polyhedronEditorRequiredMetaDataNames.push_back(GiftiMetaDataXmlElements::SAMPLES_SAMPLE_NUMBER);
            }
            
            metaDataNamesOut         = s_polyhedronEditorMetaDataNames;
            requiredMetaDataNamesOut = s_polyhedronEditorRequiredMetaDataNames;
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
}

/**
 * @return Tooltip for the metadata name (empty if not tooltip available).
 * This method is intended for overriding by subclasses.
 * @param metaDataNames
 *    Names of elements that are to be displayed in a metadata editor.
 */
AString
AnnotationMetaData::getToolTipForMetaDataName(const QString& metaDataName) const
{
    AString tooltip;
    
    if (s_metaDataNameToolTips.empty()) {
        /*
         * Use static so only one map for all instances
         */
        /*
         * s_metaDataNameToolTips.emplace(<meta data name: eg GiftiMetaDataXmlElements::SAMPLES_HEMISPHERE>,
         *                                "Text for tooltip");
         */
    }
    
    const auto iter(s_metaDataNameToolTips.find(metaDataName));
    if (iter != s_metaDataNameToolTips.end()) {
        tooltip = iter->second;
    }
    
    return tooltip;
}

/**
 * @return True if the metadata name is a composite element, meaning, it is not stored
 * in the metadata but is composed of other metadata elements.
 * This method is intended for overriding by subclasses.
 * @param metaDataName
 *    Names of element
 */
bool
AnnotationMetaData::isCompositeMetaDataName(const QString& metaDataName) const
{
    if (s_compositeMetaDataNames.empty()) {
        /*
         * Use static so only one map for all instances
         */
        s_compositeMetaDataNames.insert(GiftiMetaDataXmlElements::SAMPLES_LOCAL_SAMPLE_ID);
        s_compositeMetaDataNames.insert(GiftiMetaDataXmlElements::SAMPLES_LOCAL_SLAB_ID);
    }
    
    if (s_compositeMetaDataNames.find(metaDataName) != s_compositeMetaDataNames.end()) {
        return true;
    }

    return false;
}

/**
 * Get the value for a composite metadata element
 * This method is intended for overriding by subclasses.
 * @param metaDataName
 *    Names of element
 * @return
 *    Value for composite metadata element or empty string if element is not a composite element.
 */
AString
AnnotationMetaData::getCompositeMetaDataValue(const QString& metaDataName) const
{
    AString dataValueOut;
    
    const AString separator(".");
    
    if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_LOCAL_SAMPLE_ID) {
        std::vector<AString> components;
        components.push_back(get(GiftiMetaDataXmlElements::SAMPLES_LOCAL_SLAB_ID));
        components.push_back(get(GiftiMetaDataXmlElements::SAMPLES_SAMPLE_NUMBER));
        dataValueOut = assembleCompositeElementComponents(components,
                                                          separator);
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_LOCAL_SLAB_ID) {
        std::vector<AString> components;
        components.push_back(get(GiftiMetaDataXmlElements::SAMPLES_ALLEN_LOCAL_NAME));
        components.push_back(get(GiftiMetaDataXmlElements::SAMPLES_ALLEN_TISSUE_TYPE));
        components.push_back(get(GiftiMetaDataXmlElements::SAMPLES_ALLEN_SLAB_NUMBER));
        dataValueOut = assembleCompositeElementComponents(components,
                                                          separator);
    }
    else if (isCompositeMetaDataName(metaDataName)) {
        CaretLogWarning("Composite metadata item never set (program error): "
                        + metaDataName);
    }
    return dataValueOut;
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
AnnotationMetaData::assembleCompositeElementComponents(const std::vector<AString>& components,
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



