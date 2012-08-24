
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __IDENTIFICATION_ITEM_FOCUS_VOLUME_DECLARE__
#include "IdentificationItemFocusVolume.h"
#undef __IDENTIFICATION_ITEM_FOCUS_VOLUME_DECLARE__

using namespace caret;


    
/**
 * \class IdentificationItemFocusVolume 
 * \brief Contains information about the identified focus.
 */
/**
 * Constructor.
 */
IdentificationItemFocusVolume::IdentificationItemFocusVolume()
: IdentificationItem(IdentificationItemDataTypeEnum::FOCUS_VOLUME)
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
IdentificationItemFocusVolume::~IdentificationItemFocusVolume()
{
}

/**
 * Reset this selection item. 
 */
void 
IdentificationItemFocusVolume::reset()
{
    IdentificationItem::reset();
    this->volumeFile = NULL;
    this->focus = NULL;
    this->fociFile = NULL;
    this->focusIndex = -1;
    this->focusProjectionIndex = -1;
}

/**
 * @return Is this identified item valid?
 */
bool 
IdentificationItemFocusVolume::isValid() const
{
    return (this->focus != NULL);
}

/**
 * @return VolumeFile on which focus was drawn.
 */
const VolumeFile*
IdentificationItemFocusVolume::getVolumeFile() const
{
    return this->volumeFile;
}

/**
 * @return VolumeFile on which focus was drawn.
 */
VolumeFile* 
IdentificationItemFocusVolume::getVolumeFile()
{
    return this->volumeFile;
}

/**
 * Set the volume file on which focus was drawn.
 * @param volumeFile
 *    New value for volumeFile.
 */
void 
IdentificationItemFocusVolume::setVolumeFile(VolumeFile* volumeFile)
{
    this->volumeFile = volumeFile;
}

/**
 * @return The focus that was identified.
 */
const Focus* 
IdentificationItemFocusVolume::getFocus() const
{
    return this->focus;
}

/**
 * @return The focus that was identified.
 */
Focus* 
IdentificationItemFocusVolume::getFocus()
{
    return this->focus;
}

/**
 * Set the focus that was identified.
 * @param focus
 *    New value for focus.
 */
void 
IdentificationItemFocusVolume::setFocus(Focus* focus)
{
    this->focus = focus;
}

/**
 * @return The focus file containing focus that was identified.
 */
const FociFile* 
IdentificationItemFocusVolume::getFociFile() const
{
    return this->fociFile;
}

/**
 * @return The focus file containing focus that was identified.
 */
FociFile* 
IdentificationItemFocusVolume::getFociFile()
{
    return this->fociFile;
}

/**
 * Set the focus file containing focus that was identified.
 * @param fociFile
 *    New value for focus file.
 */
void 
IdentificationItemFocusVolume::setFociFile(FociFile* fociFile)
{
    this->fociFile = fociFile;
}

/**
 * return Index of selected focus.
 */
int32_t 
IdentificationItemFocusVolume::getFocusIndex() const
{
    return this->focusIndex;
}

/**
 * Set index of selected focus.
 * @param focusIndex
 *    New value for focus index.
 */
void 
IdentificationItemFocusVolume::setFocusIndex(const int32_t focusIndex)
{
    this->focusIndex = focusIndex;
}

/**
 * return Projection Index of selected focus.
 */
int32_t 
IdentificationItemFocusVolume::getFocusProjectionIndex() const
{
    return this->focusProjectionIndex;
}

/**
 * Set projection index of selected focus.
 * @param focusProjectionIndex
 *    New value for focus index.
 */
void 
IdentificationItemFocusVolume::setFocusProjectionIndex(const int32_t focusProjectionIndex)
{
    this->focusProjectionIndex = focusProjectionIndex;
}

