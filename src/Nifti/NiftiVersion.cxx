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


#define __NIFTIVERSION_DECLARE__
#include "NiftiVersion.h"
#undef __NIFTIVERSION_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
NiftiVersion::NiftiVersion(
                   const Enum e,
                   const int32_t integerCode,
                   const std::string& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

/**
 * Destructor.
 */
NiftiVersion::~NiftiVersion()
{
}

void
NiftiVersion::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NiftiVersion(NIFTI_VERSION_1, 348, "NIFTI_VERSION_1"));
    enumData.push_back(NiftiVersion(NIFTI_VERSION_2, 540, "NIFTI_VERSION_2"));
}

/**
 * Get the enum value for this enumerated item.
 * @return the value for this enumerated item.
 */
NiftiVersion::Enum 
NiftiVersion::getEnum() const
{
    return this->e;
}

/**
 * Get the integer code for this enumerated item.
 * @return the integer code for this enumerated item.
 */
int32_t 
NiftiVersion::getIntegerCode() const
{
    return this->integerCode;
}

/**
 * Get the enum name for this enumerated item.
 * @return the name for this enumerated item.
 */
std::string
NiftiVersion::getName() const
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
const NiftiVersion*
NiftiVersion::findData(const Enum e)
{
    initialize();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const NiftiVersion* d = &enumData[i];
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
NiftiVersion::toString(Enum e, bool* isValidOut) {
    initialize();
    
    std::string s;
    
    for (std::vector<NiftiVersion>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiVersion& d = *iter;
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
NiftiVersion::Enum 
NiftiVersion::fromString(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiVersion>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiVersion& d = *iter;
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
