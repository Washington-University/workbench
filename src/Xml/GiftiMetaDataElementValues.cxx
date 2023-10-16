
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
#include "StructureEnum.h"

using namespace caret;



/**
 * \class caret::GiftiMetaDataElementValues 
 * \brief Some metadata elements have a list of predfined values
 * \ingroup Xml
 */

/**
 * @return True if the metadata element with the given name supports pre-defined values
 */
bool
GiftiMetaDataElementValues::hasValuesForElement(const QString& metaDataName)
{
    const QStringList valuesList(getValuesForElement(metaDataName));
    return ( ! valuesList.isEmpty());
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
    else if (metaDataName == GiftiMetaDataXmlElements::SAMPLES_STRUCTURE) {
        std::vector<StructureEnum::Enum> structureEnums;
        StructureEnum::getAllEnums(structureEnums);
        for (const StructureEnum::Enum s : structureEnums) {
            metaDataValues.push_back(StructureEnum::toGuiName(s));
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
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GiftiMetaDataElementValues::toString() const
{
    return "GiftiMetaDataElementValues";
}

