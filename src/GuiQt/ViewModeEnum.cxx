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


#define __VIEW_MODE_DECLARE__
#include "ViewModeEnum.h"
#undef __VIEW_MODE_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
ViewModeEnum::ViewModeEnum(const Enum e,
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
ViewModeEnum::~ViewModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
ViewModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ViewModeEnum(VIEW_MODE_INVALID, 
                                    0, 
                                    "VIEW_MODE_INVALID", 
                                    "Invalid"));
    
    enumData.push_back(ViewModeEnum(VIEW_MODE_SURFACE, 
                                    1, 
                                    "VIEW_MODE_SURFACE", 
                                    "Surface"));
    
    enumData.push_back(ViewModeEnum(VIEW_MODE_VOLUME_SLICES, 
                                    2, 
                                    "VIEW_MODE_VOLUME_SLICES", 
                                    "Volume"));
    
    enumData.push_back(ViewModeEnum(VIEW_MODE_WHOLE_BRAIN, 
                                    3, 
                                    "VIEW_MODE_WHOLE_BRAIN", 
                                    "Whole Brain"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ViewModeEnum*
ViewModeEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const ViewModeEnum* d = &enumData[i];
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
 * @return 
 *     String representing enumerated value.
 */
QString 
ViewModeEnum::toName(Enum e) {
    initialize();
    
    const ViewModeEnum* gaio = findData(e);
    return gaio->name;
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
ViewModeEnum::Enum 
ViewModeEnum::fromName(const QString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<ViewModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ViewModeEnum& d = *iter;
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
 * Get a GUI string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
QString 
ViewModeEnum::toGuiName(Enum e) {
    initialize();
    
    const ViewModeEnum* gaio = findData(e);
    return gaio->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
ViewModeEnum::Enum 
ViewModeEnum::fromGuiName(const QString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<ViewModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ViewModeEnum& d = *iter;
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
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
ViewModeEnum::toIntegerCode(Enum e)
{
    initialize();
    const ViewModeEnum* ndt = findData(e);
    return ndt->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
ViewModeEnum::Enum
ViewModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = VIEW_MODE_INVALID;
    
    for (std::vector<ViewModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ViewModeEnum& ndt = *iter;
        if (ndt.integerCode == integerCode) {
            e = ndt.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

