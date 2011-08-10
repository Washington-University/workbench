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


#define __PALETTE_THRESHOLD_TEST_DECLARE__
#include "PaletteThresholdTestEnum.h"
#undef __PALETTE_THRESHOLD_TEST_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
PaletteThresholdTestEnum::PaletteThresholdTestEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const QString& name,
                   const QString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
PaletteThresholdTestEnum::~PaletteThresholdTestEnum()
{
}

void
PaletteThresholdTestEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteThresholdTestEnum(THRESHOLD_TEST_SHOW_ABOVE, 1, "THRESHOLD_TEST_SHOW_ABOVE", "Show Data Above Threshold"));
    enumData.push_back(PaletteThresholdTestEnum(THRESHOLD_TEST_SHOW_BELOW, 1, "THRESHOLD_TEST_SHOW_BELOW", "Show Data Below Threshold"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteThresholdTestEnum*
PaletteThresholdTestEnum::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteThresholdTestEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    CaretAssertMessage(0, "Threshold Test enum failed to match.");
    
    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
QString 
PaletteThresholdTestEnum::toName(Enum e) {
    initialize();
    
    const PaletteThresholdTestEnum* ptt = findData(e);
    return ptt->name;
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
PaletteThresholdTestEnum::Enum 
PaletteThresholdTestEnum::fromName(const QString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTestEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTestEnum& d = *iter;
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

/**
 * Get a gui name representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
QString 
PaletteThresholdTestEnum::toGuiName(Enum e) {
    initialize();
    
    const PaletteThresholdTestEnum* psm = findData(e);
    return psm->guiName;
}

/**
 * Get an enumerated value corresponding to its gui name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
PaletteThresholdTestEnum::Enum 
PaletteThresholdTestEnum::fromGuiName(const QString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTestEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTestEnum& d = *iter;
        if (d.guiName == s) {
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

/**
 * Get the integer code associated with a scale mode.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with a scale mode.
 */
int32_t 
PaletteThresholdTestEnum::toIntegerCode(Enum e)
{
    initialize();
    const PaletteThresholdTestEnum* nsu = findData(e);
    return nsu->integerCode;
}

/**
 * Find enum corresponding to integer code.
 * @param integerCode
 *    The integer code.
 * @param isValidOut
 *    If not NULL, on exit it indicates valid integer code.
 * @return
 *    Enum corresponding to integer code.
 */
PaletteThresholdTestEnum::Enum 
PaletteThresholdTestEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTestEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTestEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}
