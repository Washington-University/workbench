
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

#define __SELECTION_ITEM_FOCUS_DECLARE__
#include "SelectionItemFocus.h"
#undef __SELECTION_ITEM_FOCUS_DECLARE__

#include "Focus.h"
#include "FociFile.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class SelectionItemFocus
 * \brief Contains information about the selected focus.
 */
/**
 * Constructor.
 */
SelectionItemFocus::SelectionItemFocus()
: SelectionItem(SelectionItemDataTypeEnum::FOCUS_VOLUME)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemFocus::~SelectionItemFocus()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemFocus::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemFocus::resetPrivate()
{
    SelectionItem::reset();
    m_idType     = IdType::INVALID;
    m_surface    = NULL;
    m_volumeFile = NULL;
    m_histologySlicesFile = NULL;
    m_focus      = NULL;
    m_fociFile   = NULL;
    m_focusIndex = -1;
    m_focusProjectionIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemFocus::isValid() const
{
    return (m_idType != IdType::INVALID);
}

/**
 * @return Type of focus identification
 */
SelectionItemFocus::IdType
SelectionItemFocus::getIdType() const
{
    return m_idType;
}

/**
 * Set surface identification of a focus
 * @param surface
 *    Surface on which focus was identified
 * @param fociFile
 *    Foci file containing focus
 * @param focus
 *    The focus
 * @param focusIndex
 *    Index of focus in focus file
 * @param focusProjectionIndex
 *    Index of focus projection used to draw focus
 */
void
SelectionItemFocus::setSurfaceSelection(Surface* surface,
                                        FociFile* fociFile,
                                        Focus* focus,
                                        const int32_t focusIndex,
                                        const int32_t focusProjectionIndex)
{
    CaretAssert(surface);
    CaretAssert(fociFile);
    CaretAssert(focus);
    
    m_idType               = IdType::SURFACE;
    m_surface              = surface;
    m_fociFile             = fociFile;
    m_focus                = focus;
    m_focusIndex           = focusIndex;
    m_focusProjectionIndex = focusProjectionIndex;
}

/**
 * Set histology identification of a focus
 * @param histologySlicesFile
 *    Histology file on which focus was identified
 * @param fociFile
 *    Foci file containing focus
 * @param focus
 *    The focus
 * @param focusIndex
 *    Index of focus in focus file
 * @param focusProjectionIndex
 *    Index of focus projection used to draw focus
 */
void
SelectionItemFocus::setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                          FociFile* fociFile,
                                          Focus* focus,
                                          const int32_t focusIndex,
                                          const int32_t focusProjectionIndex)
{
    CaretAssert(histologySlicesFile);
    CaretAssert(fociFile);
    CaretAssert(focus);
    
    m_idType               = IdType::HISTOLOGY;
    m_histologySlicesFile  = histologySlicesFile;
    m_fociFile             = fociFile;
    m_focus                = focus;
    m_focusIndex           = focusIndex;
    m_focusProjectionIndex = focusProjectionIndex;
}

/**
 * Set surface identification of a focus
 * @param volumeMappableInterface
 *    volume on which focus was identified
 * @param fociFile
 *    Foci file containing focus
 * @param focus
 *    The focus
 * @param focusIndex
 *    Index of focus in focus file
 * @param focusProjectionIndex
 *    Index of focus projection used to draw focus
 */
void
SelectionItemFocus::setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                       FociFile* fociFile,
                                       Focus* focus,
                                       const int32_t focusIndex,
                                       const int32_t focusProjectionIndex)
{
    CaretAssert(volumeMappableInterface);
    CaretAssert(fociFile);
    CaretAssert(focus);
    
    m_idType               = IdType::VOLUME;
    m_volumeFile           = volumeMappableInterface;
    m_fociFile             = fociFile;
    m_focus                = focus;
    m_focusIndex           = focusIndex;
    m_focusProjectionIndex = focusProjectionIndex;
}

/**
 * @return Surface on which focus was drawn.
 */
const Surface*
SelectionItemFocus::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface on which focus was drawn.
 */
Surface*
SelectionItemFocus::getSurface()
{
    return m_surface;
}

/**
 * @return VolumeFile on which focus was drawn.
 */
const VolumeMappableInterface*
SelectionItemFocus::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * @return VolumeFile on which focus was drawn.
 */
VolumeMappableInterface*
SelectionItemFocus::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * @return The histology slices file
 */
HistologySlicesFile*
SelectionItemFocus::getHistologySlicesFile()
{
    return m_histologySlicesFile;
}

/**
 * @return The histology slices file
 */
const HistologySlicesFile*
SelectionItemFocus::getHistologySlicesFile() const
{
    return m_histologySlicesFile;
}

/**
 * @return The focus that was selected.
 */
const Focus* 
SelectionItemFocus::getFocus() const
{
    return m_focus;
}

/**
 * @return The focus that was selected.
 */
Focus* 
SelectionItemFocus::getFocus()
{
    return m_focus;
}

/**
 * @return The focus file containing focus that was selected.
 */
const FociFile* 
SelectionItemFocus::getFociFile() const
{
    return m_fociFile;
}

/**
 * @return The focus file containing focus that was selected.
 */
FociFile* 
SelectionItemFocus::getFociFile()
{
    return m_fociFile;
}

/**
 * return Index of selected focus.
 */
int32_t 
SelectionItemFocus::getFocusIndex() const
{
    return m_focusIndex;
}

/**
 * return Projection Index of selected focus.
 */
int32_t 
SelectionItemFocus::getFocusProjectionIndex() const
{
    return m_focusProjectionIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemFocus::toString() const
{
    AString name = "INVALID";
    if (m_volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(m_volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((m_surface != NULL) ? m_surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Volume File: " + name + "\n");
    text += ("Foci File: " + ((m_fociFile != NULL) ? m_fociFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Focus: " + ((m_focus != NULL) ? m_focus->getName() : "INVALID") + "\n");
    text += ("Focus Index: " + AString::number(m_focusIndex) + "\n");
    text += ("Focus Projection Index: " + AString::number(m_focusProjectionIndex) + "\n");
    return text;
}
