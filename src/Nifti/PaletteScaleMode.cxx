/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#define __PALETTE_SCALE_DECLARE__
#include "PaletteScaleMode.h"
#undef __PALETTE_SCALE_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
PaletteScaleMode::PaletteScaleMode(
                   const Enum e,
                   const int32_t integerCode,
                   const std::string& name,
                   const std::string& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
PaletteScaleMode::~PaletteScaleMode()
{
}

void
PaletteScaleMode::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteScaleMode(MODE_AUTO_SCALE, 1, "MODE_AUTO_SCALE", "Auto Scale"));
    enumData.push_back(PaletteScaleMode(MODE_AUTO_SCALE_PERCENTAGE, 1, "MODE_AUTO_SCALE_PERCENTAGE", "Auto Scale - Percentage"));
    enumData.push_back(PaletteScaleMode(MODE_USER_SCALE, 1, "MODE_USER_SCALE", "User Scale"));
}

/**
 * Get the enum value for this enumerated item.
 * @return the value for this enumerated item.
 */
PaletteScaleMode::Enum 
PaletteScaleMode::getEnum() const
{
    return this->e;
}

/**
 * Get the integer code for this enumerated item.
 * @return the integer code for this enumerated item.
 */
int32_t 
PaletteScaleMode::getIntegerCode() const
{
    return this->integerCode;
}

/**
 * Get the enum name for this enumerated item.
 * @return the name for this enumerated item.
 */
std::string
PaletteScaleMode::getName() const
{
    return this->name;
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteScaleMode*
PaletteScaleMode::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteScaleMode* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
PaletteScaleMode::toString(Enum e, bool* isValidOut) {
    initialize();
    
    std::string s;
    
    for (std::vector<PaletteScaleMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteScaleMode& d = *iter;
        if (d.e == e) {
            s = d.name;
            break;
        }
    }

    if (isValidOut != NULL) {
        *isValidOut = (s.size() > 0);
    }
    
    return s;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
PaletteScaleMode::Enum 
PaletteScaleMode::fromString(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteScaleMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteScaleMode& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}
