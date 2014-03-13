
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

#define __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__
#include "SelectionItemFocusVolume.h"
#undef __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__

#include "Focus.h"
#include "FociFile.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class SelectionItemFocusVolume 
 * \brief Contains information about the selected focus.
 */
/**
 * Constructor.
 */
SelectionItemFocusVolume::SelectionItemFocusVolume()
: SelectionItem(SelectionItemDataTypeEnum::FOCUS_VOLUME)
{
    this->volumeFile = NULL;
    this->focus = NULL;
    this->fociFile = NULL;
    this->focusIndex = -1;
    this->focusProjectionIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemFocusVolume::~SelectionItemFocusVolume()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemFocusVolume::reset()
{
    SelectionItem::reset();
    this->volumeFile = NULL;
    this->focus = NULL;
    this->fociFile = NULL;
    this->focusIndex = -1;
    this->focusProjectionIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemFocusVolume::isValid() const
{
    return (this->focus != NULL);
}

/**
 * @return VolumeFile on which focus was drawn.
 */
const VolumeMappableInterface*
SelectionItemFocusVolume::getVolumeFile() const
{
    return this->volumeFile;
}

/**
 * @return VolumeFile on which focus was drawn.
 */
VolumeMappableInterface* 
SelectionItemFocusVolume::getVolumeFile()
{
    return this->volumeFile;
}

/**
 * Set the volume file on which focus was drawn.
 * @param volumeFile
 *    New value for volumeFile.
 */
void 
SelectionItemFocusVolume::setVolumeFile(VolumeMappableInterface* volumeFile)
{
    this->volumeFile = volumeFile;
}

/**
 * @return The focus that was selected.
 */
const Focus* 
SelectionItemFocusVolume::getFocus() const
{
    return this->focus;
}

/**
 * @return The focus that was selected.
 */
Focus* 
SelectionItemFocusVolume::getFocus()
{
    return this->focus;
}

/**
 * Set the focus that was selected.
 * @param focus
 *    New value for focus.
 */
void 
SelectionItemFocusVolume::setFocus(Focus* focus)
{
    this->focus = focus;
}

/**
 * @return The focus file containing focus that was selected.
 */
const FociFile* 
SelectionItemFocusVolume::getFociFile() const
{
    return this->fociFile;
}

/**
 * @return The focus file containing focus that was selected.
 */
FociFile* 
SelectionItemFocusVolume::getFociFile()
{
    return this->fociFile;
}

/**
 * Set the focus file containing focus that was selected.
 * @param fociFile
 *    New value for focus file.
 */
void 
SelectionItemFocusVolume::setFociFile(FociFile* fociFile)
{
    this->fociFile = fociFile;
}

/**
 * return Index of selected focus.
 */
int32_t 
SelectionItemFocusVolume::getFocusIndex() const
{
    return this->focusIndex;
}

/**
 * Set index of selected focus.
 * @param focusIndex
 *    New value for focus index.
 */
void 
SelectionItemFocusVolume::setFocusIndex(const int32_t focusIndex)
{
    this->focusIndex = focusIndex;
}

/**
 * return Projection Index of selected focus.
 */
int32_t 
SelectionItemFocusVolume::getFocusProjectionIndex() const
{
    return this->focusProjectionIndex;
}

/**
 * Set projection index of selected focus.
 * @param focusProjectionIndex
 *    New value for focus index.
 */
void 
SelectionItemFocusVolume::setFocusProjectionIndex(const int32_t focusProjectionIndex)
{
    this->focusProjectionIndex = focusProjectionIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemFocusVolume::toString() const
{
    AString name = "INVALID";
    if (volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    AString text = SelectionItem::toString();
    text += ("Volume File: " + name + "\n");
    text += ("Foci File: " + ((fociFile != NULL) ? fociFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Focus: " + ((focus != NULL) ? focus->getName() : "INVALID") + "\n");
    text += ("Focus Index: " + AString::number(focusIndex) + "\n");
    text += ("Focus Projection Index: " + AString::number(focusProjectionIndex) + "\n");
    return text;
}
