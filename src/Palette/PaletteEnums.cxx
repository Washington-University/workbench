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


#define __PALETTE_ENUMS_DECLARE__
#include "PaletteEnums.h"
#undef __PALETTE_ENUMS_DECLARE__

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
PaletteScaleModeEnum::PaletteScaleModeEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const AString& name,
                   const AString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
PaletteScaleModeEnum::~PaletteScaleModeEnum()
{
}

void
PaletteScaleModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteScaleModeEnum(MODE_AUTO_SCALE, 0, "MODE_AUTO_SCALE", "Auto Scale"));
    enumData.push_back(PaletteScaleModeEnum(MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE, 1, "MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE", "Auto Scale - Absolute Percentage"));
    enumData.push_back(PaletteScaleModeEnum(MODE_AUTO_SCALE_PERCENTAGE, 2, "MODE_AUTO_SCALE_PERCENTAGE", "Auto Scale - Percentage"));
    enumData.push_back(PaletteScaleModeEnum(MODE_USER_SCALE, 3, "MODE_USER_SCALE", "User Scale"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteScaleModeEnum*
PaletteScaleModeEnum::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteScaleModeEnum* d = &enumData[i];
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
AString 
PaletteScaleModeEnum::toName(Enum e) {
    initialize();
    
    const PaletteScaleModeEnum* psm = findData(e);
    return psm->name;
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
PaletteScaleModeEnum::Enum 
PaletteScaleModeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = MODE_AUTO_SCALE;
    
    for (std::vector<PaletteScaleModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteScaleModeEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type PaletteScaleModeEnum"));
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
AString 
PaletteScaleModeEnum::toGuiName(Enum e) {
    initialize();
    
    const PaletteScaleModeEnum* psm = findData(e);
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
PaletteScaleModeEnum::Enum 
PaletteScaleModeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = MODE_AUTO_SCALE;
    
    for (std::vector<PaletteScaleModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteScaleModeEnum& d = *iter;
        if (d.guiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName \"" + s + " \"failed to match enumerated value for type PaletteScaleModeEnum"));
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
PaletteScaleModeEnum::toIntegerCode(Enum e)
{
    initialize();
    const PaletteScaleModeEnum* nsu = findData(e);
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
PaletteScaleModeEnum::Enum 
PaletteScaleModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = MODE_AUTO_SCALE;
    
    for (std::vector<PaletteScaleModeEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteScaleModeEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("integerCode \"" + AString::number(integerCode) + " \"failed to match enumerated value for type PaletteScaleModeEnum"));
    }
    return e;
}

/**
 * Get all enums for the data type.
 * @param enumsOut
 *   Loaded with all enums for this data type.
 */
void PaletteScaleModeEnum::getAllEnums(std::vector< PaletteScaleModeEnum::Enum >& enumsOut)
{
    initialize();
    enumsOut.resize(enumData.size());
    for (int i = 0; i < (int)enumData.size(); ++i)
    {
        enumsOut[i] = enumData[i].e;
    }
}


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
                   const AString& name,
                   const AString& guiName)
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

    enumData.push_back(PaletteThresholdTestEnum(THRESHOLD_TEST_SHOW_OUTSIDE, 0, "THRESHOLD_TEST_SHOW_OUTSIDE", "Show Data Outside Thresholds"));
    enumData.push_back(PaletteThresholdTestEnum(THRESHOLD_TEST_SHOW_INSIDE, 1, "THRESHOLD_TEST_SHOW_INSIDE", "Show Data Below Threshold"));
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
AString 
PaletteThresholdTestEnum::toName(Enum e) {
    initialize();
    
    const PaletteThresholdTestEnum* ptt = findData(e);
    return ptt->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param sin
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
PaletteThresholdTestEnum::Enum 
PaletteThresholdTestEnum::fromName(const AString& sin, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = THRESHOLD_TEST_SHOW_OUTSIDE;
    
    AString s = sin;
    if (s == "THRESHOLD_TEST_SHOW_ABOVE") {
        s = "THRESHOLD_TEST_SHOW_OUTSIDE";
    }
    else if (s == "THRESHOLD_TEST_SHOW_BELOW") {
        s = "THRESHOLD_TEST_SHOW_INSIDE";
    }
    
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
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type PaletteThresholdTestEnum"));
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
AString 
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
PaletteThresholdTestEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = THRESHOLD_TEST_SHOW_OUTSIDE;
    
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
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName \"" + s + " \"failed to match enumerated value for type PaletteThresholdTestEnum"));
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
    Enum e = THRESHOLD_TEST_SHOW_OUTSIDE;
    
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
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("integerCode \"" + AString::number(integerCode) + " \"failed to match enumerated value for type PaletteThresholdTestEnum"));
    }
    return e;
}

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
PaletteThresholdTypeEnum::PaletteThresholdTypeEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const AString& name,
                   const AString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
PaletteThresholdTypeEnum::~PaletteThresholdTypeEnum()
{
}

void
PaletteThresholdTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_OFF, 0, "THRESHOLD_TYPE_OFF", "Off"));
    if (PaletteThresholdTypeEnum::mappedThresholdsEnabled) {
        enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_NORMAL, 1, "THRESHOLD_TYPE_NORMAL", "Normal"));
        enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_MAPPED, 2, "THRESHOLD_TYPE_MAPPED", "Mapped"));
        enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_MAPPED_AVERAGE_AREA, 3, "THRESHOLD_TYPE_MAPPED_AVERAGE_AREA", "Mapped Average Area"));
    }
    else {
        enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_NORMAL, 1, "THRESHOLD_TYPE_NORMAL", "Self"));
        enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_FILE, 2, "THRESHOLD_TYPE_FILE", "File"));
    }
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteThresholdTypeEnum*
PaletteThresholdTypeEnum::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteThresholdTypeEnum* d = &enumData[i];
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
AString 
PaletteThresholdTypeEnum::toName(Enum e) {
    initialize();
    
    const PaletteThresholdTypeEnum* ptt = findData(e);
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
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = THRESHOLD_TYPE_OFF;
    
    for (std::vector<PaletteThresholdTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    PaletteThresholdTypeEnum::handleDisabledThresholdTypes(e);
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type PaletteThresholdTypeEnum"));
    }
    return e;
}

/**
 * If the mapped threshold types are disabled, convert
 * them to normal thresholding.
 * @param e
 *    Thresholding type that may be changed.
 */
void 
PaletteThresholdTypeEnum::handleDisabledThresholdTypes(Enum& e)
{
    if (PaletteThresholdTypeEnum::mappedThresholdsEnabled == false) {
        if ((e == THRESHOLD_TYPE_MAPPED)
            || (e == THRESHOLD_TYPE_MAPPED_AVERAGE_AREA)) {
            e = THRESHOLD_TYPE_NORMAL;
        }
    }
}

/**
 * Get a gui name representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
PaletteThresholdTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const PaletteThresholdTypeEnum* psm = findData(e);
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
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = THRESHOLD_TYPE_OFF;
    
    for (std::vector<PaletteThresholdTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& d = *iter;
        if (d.guiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    PaletteThresholdTypeEnum::handleDisabledThresholdTypes(e);
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName \"" + s + " \"failed to match enumerated value for type PaletteThresholdTypeEnum"));
    }
    return e;
}

/**
 * Get the integer code associated with a threshold type.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with a threshold type.
 */
int32_t 
PaletteThresholdTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const PaletteThresholdTypeEnum* nsu = findData(e);
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
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = THRESHOLD_TYPE_OFF;
    
    for (std::vector<PaletteThresholdTypeEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }
    
    PaletteThresholdTypeEnum::handleDisabledThresholdTypes(e);
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("integerCode \"" + AString::number(integerCode) + " \"failed to match enumerated value for type PaletteThresholdTypeEnum"));
    }
    return e;
}

/**
 * Get all enums for the data type.
 * @param enumsOut
 *   Loaded with all enums for this data type.
 */
void PaletteThresholdTestEnum::getAllEnums(std::vector<Enum>& enumsOut)
{
    initialize();
    enumsOut.resize(enumData.size());
    for (int i = 0; i < (int)enumData.size(); ++i)
    {
        enumsOut[i] = enumData[i].e;
    }
}

/**
 * Get all enums for the data type.
 * @param enumsOut
 *   Loaded with all enums for this data type.
 */
void PaletteThresholdTypeEnum::getAllEnums(std::vector<Enum>& enumsOut)
{
    initialize();
    enumsOut.resize(enumData.size());
    for (int i = 0; i < (int)enumData.size(); ++i)
    {
        enumsOut[i] = enumData[i].e;
    }
}


