
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

#define __SELECTION_ITEM_FEATURE_DECLARE__
#include "SelectionItemFeature.h"
#undef __SELECTION_ITEM_FEATURE_DECLARE__

#include "HistologySlicesFile.h"
#include "FeatureItem.h"
#include "FeatureFile.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class SelectionItemFeature
 * \brief Contains information about the selected feature.
 */
/**
 * Constructor.
 */
SelectionItemFeature::SelectionItemFeature()
: SelectionItem(SelectionItemDataTypeEnum::FOCUS_VOLUME)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemFeature::~SelectionItemFeature()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemFeature::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemFeature::resetPrivate()
{
    SelectionItem::reset();
    m_idType     = IdType::INVALID;
    m_surface    = NULL;
    m_volumeFile = NULL;
    m_histologySlicesFile = NULL;
    m_featureItem      = NULL;
    m_featureFile   = NULL;
    m_featureIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemFeature::isValid() const
{
    return (m_idType != IdType::INVALID);
}

/**
 * @return Type of feature identification
 */
SelectionItemFeature::IdType
SelectionItemFeature::getIdType() const
{
    return m_idType;
}

/**
 * Set surface identification of a feature
 * @param surface
 *    Surface on which feature was identified
 * @param featureFile
 *    feature file containing feature item
 * @param featureItem
 *    The feature item
 * @param featureIndex
 *    Index of feature in feature file
 */
void
SelectionItemFeature::setSurfaceSelection(const Surface* surface,
                                          FeatureFile* featureFile,
                                        FeatureItem* featureItem,
                                        const int32_t featureIndex)
{
    CaretAssert(surface);
    CaretAssert(featureFile);
    CaretAssert(featureItem);
    
    m_idType               = IdType::SURFACE;
    m_surface              = surface;
    m_featureFile             = featureFile;
    m_featureItem                = featureItem;
    m_featureIndex           = featureIndex;
}

/**
 * Set whole brain identification of a feature
 * @param featureFile
 *    feature file containing feature item
 * @param feature
 *    The feature
 * @param featureIndex
 *    Index of feature in feature file
 */
void
SelectionItemFeature::setWholeBrainSelection(FeatureFile* featureFile,
                                                            FeatureItem* featureItem,
                                                            const int32_t featureIndex)
{
    CaretAssert(featureFile);
    CaretAssert(featureItem);
    
    m_idType        = IdType::WHOLE_BRAIN;
    m_featureFile  = featureFile;
    m_featureItem  = featureItem;
    m_featureIndex = featureIndex;
}



/**
 * Set histology identification of a feature
 * @param histologySlicesFile
 *    Histology file on which feature was identified
 * @param featureFile
 *    feature file containing feature item
 * @param featureItem
 *    The feature
 * @param featureIndex
 *    Index of feature in feature file
 */
void
SelectionItemFeature::setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                            FeatureFile* featureFile,
                                          FeatureItem* featureItem,
                                          const int32_t featureIndex)
{
    CaretAssert(histologySlicesFile);
    CaretAssert(featureFile);
    CaretAssert(featureItem);
    
    m_idType               = IdType::HISTOLOGY;
    m_histologySlicesFile  = histologySlicesFile;
    m_featureFile             = featureFile;
    m_featureItem                = featureItem;
    m_featureIndex           = featureIndex;
}

/**
 * Set surface identification of a feature
 * @param volumeMappableInterface
 *    volume on which feature was identified
 * @param featureFile
 *    feature file containing feature item
 * @param featureItem
 *    The feature
 * @param featureIndex
 *    Index of feature in feature file
 */
void
SelectionItemFeature::setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                         FeatureFile* featureFile,
                                       FeatureItem* featureItem,
                                       const int32_t featureIndex)
{
    CaretAssert(volumeMappableInterface);
    CaretAssert(featureFile);
    CaretAssert(featureItem);
    
    m_idType               = IdType::VOLUME;
    m_volumeFile           = volumeMappableInterface;
    m_featureFile             = featureFile;
    m_featureItem               = featureItem;
    m_featureIndex           = featureIndex;
}

/**
 * @return Surface on which feature was drawn.
 */
const Surface*
SelectionItemFeature::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface on which feature was drawn.
 */
Surface*
SelectionItemFeature::getSurface()
{
    return const_cast<Surface*>(m_surface);
}

/**
 * @return VolumeFile on which feature was drawn.
 */
const VolumeMappableInterface*
SelectionItemFeature::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * @return VolumeFile on which feature was drawn.
 */
VolumeMappableInterface*
SelectionItemFeature::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * @return The histology slices file
 */
HistologySlicesFile*
SelectionItemFeature::getHistologySlicesFile()
{
    return m_histologySlicesFile;
}

/**
 * @return The histology slices file
 */
const HistologySlicesFile*
SelectionItemFeature::getHistologySlicesFile() const
{
    return m_histologySlicesFile;
}

/**
 * @return The feature that was selected.
 */
const FeatureItem* 
SelectionItemFeature::getFeatureItem() const
{
    return m_featureItem;
}

/**
 * @return The feature that was selected.
 */
FeatureItem* 
SelectionItemFeature::getFeatureItem()
{
    return m_featureItem;
}

/**
 * @return The feature file containing feature that was selected.
 */
const FeatureFile*
SelectionItemFeature::getFeatureFile() const
{
    return m_featureFile;
}

/**
 * @return The feature file containing feature that was selected.
 */
FeatureFile*
SelectionItemFeature::getFeatureFile()
{
    return m_featureFile;
}

/**
 * return Index of selected feature.
 */
int32_t 
SelectionItemFeature::getFeatureItemIndex() const
{
    return m_featureIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemFeature::toString() const
{
    AString name = "INVALID";
    if (m_volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(m_volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    AString text = SelectionItem::toString();
    text += ("Histology: " + ((m_histologySlicesFile != NULL) ? m_histologySlicesFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Surface: " + ((m_surface != NULL) ? m_surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Volume File: " + name + "\n");
    text += ("Feature File: " + ((m_featureFile != NULL) ? m_featureFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("FeatureItem: " + ((m_featureItem != NULL) ? m_featureItem->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Feature Index: " + AString::number(m_featureIndex) + "\n");
    return text;
}
