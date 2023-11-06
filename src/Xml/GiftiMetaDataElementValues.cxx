
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

#define __GIFTI_META_DATA_ELEMENT_VALUES_DECLARE__
#include "GiftiMetaDataElementValues.h"
#undef __GIFTI_META_DATA_ELEMENT_VALUES_DECLARE__

#include "CaretAssert.h"
#include "GiftiMetaDataXmlElements.h"
#include "HemisphereEnum.h"

using namespace caret;



/**
 * \class caret::GiftiMetaDataElementValues 
 * \brief Some metadata elements have a list of predfined values
 * \ingroup Xml
 */

GiftiMetaDataElementDataTypeEnum::Enum
GiftiMetaDataElementValues::getDataTypeForElement(const QString& metaDataName)
{
    GiftiMetaDataElementDataTypeEnum::Enum dataType(GiftiMetaDataElementDataTypeEnum::TEXT);
    
    if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_DISSECTION_DATE) {
        dataType = GiftiMetaDataElementDataTypeEnum::DATE;
    }
    else if (metaDataName == GiftiMetaDataXmlElements::METADATA_NAME_COMMENT) {
        dataType = GiftiMetaDataElementDataTypeEnum::COMMENT;
    }
    else if ((metaDataName == GiftiMetaDataXmlElements::SAMPLES_ALT_SHORTHAND_ID)
             || (metaDataName == GiftiMetaDataXmlElements::SAMPLES_ORIG_SHORTHAND_ID)) {
        dataType = GiftiMetaDataElementDataTypeEnum::LABEL_ID_NAME;
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_SHORTHAND_ID) {
        dataType = GiftiMetaDataElementDataTypeEnum::DING_ONTOLOGY_TERM;
    }
    else {
        const QStringList valuesList(getValuesForElement(metaDataName));
        if ( ! valuesList.isEmpty()) {
            dataType = GiftiMetaDataElementDataTypeEnum::LIST;
        }
    }
    return dataType;
}

/**
 * Any values defined here are automatically used in MetaDataCustomEditorWidget
 * and displayed as a list of radio buttons or a combo box depending upon the
 * number of elements.
 *
 * @return A list of values that are valid for the given element. If the element
 * does not support pre-defined values, an empty list is returned.  A QStringList
 * is used since it
 * @param metaDataName
 *    The metadata name
 */
QStringList
GiftiMetaDataElementValues::getValuesForElement(const QString& metaDataName)
{
    QStringList metaDataValues;
    if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_LOCATION_ID) {
        metaDataValues.push_back("Actual");
        metaDataValues.push_back("Desired");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_SAMPLE_TYPE) {
        metaDataValues.push_back("Anatomical ROI");
        metaDataValues.push_back("Tile");
    }
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_SAMPLE_SLAB_FACE) {
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
            metaDataValues.push_back(HemisphereEnum::toGuiAbbreviatedName(h));
        }
    }
    return metaDataValues;
}


/**
 * Constructor.
 */
GiftiMetaDataElementValues::GiftiMetaDataElementValues()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
GiftiMetaDataElementValues::~GiftiMetaDataElementValues()
{
}

/**
 * Process label's text to find the id and description
 * @param text
 *    The label's text
 * @param idOut
 *    Output with ID
 * @param descriptionOut
 *    Output with description
 *
 *
 From David's email (Nov 3, 2023):
     Unfortunately, we don’t currently have consistency across different labeling schemes. For example, the following four examples are each distinct
        For Ding ontology: <abbreviation>: <name>
           CaH: Head of Caudate
        For SARM-6: <abbreviation>
           CdH
           CdT
        For Saleem: <abbreviation> <name>
           cd caudate nucleus
        For Yerkes19_Parcellations: <shorthand_id>_<parcellation_id>
           3b_LV00
 
     Here’s my suggestion for handling such cases:
       - Have a ‘Choose’ option for just the ‘id’
       - If the id field contains only one character string without spaces,
        have the label fill in both the id and the name/description
       - If the id field contains multiple character strings with spaces:
          - Use the first character string for the shorthand_id but exclude any final character that is
           NOT an alphanumeric OR an underscore ‘_’
          - Use all subsequent text for the name/description
 *
     We may encounter label tables in the future that don’t behave well following these rules, but we can cross that bridge later.
 */
void
GiftiMetaDataElementValues::processLabelForIdDescription(const AString& labelText,
                                                         AString& idOut,
                                                         AString& descriptionOut)
{
    idOut          = "";
    descriptionOut = "";
    
    const AString text(labelText.trimmed());
    if ( ! labelText.isEmpty()) {
        const int32_t firstSpaceIndex(text.indexOf(" "));
        if (firstSpaceIndex > 0) {
            idOut = text.left(firstSpaceIndex);
            descriptionOut = text.mid(firstSpaceIndex + 1);
            
            const int32_t lastCharIndex(descriptionOut.length() - 1);
            if (lastCharIndex > 0) {
                const QChar lastChar(descriptionOut[lastCharIndex]);
                if (lastChar.isDigit()
                    || lastChar.isLetter()
                    || (lastChar == '_')) {
                    /* ok */
                }
                else {
                    descriptionOut.resize(lastCharIndex);
                }
            }
        }
        else {
            /*
             * No spaces so both id and description are the same
             */
            idOut = text;
            descriptionOut = text;
        }
    }
}

