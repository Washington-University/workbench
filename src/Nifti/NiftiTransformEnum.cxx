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


#define __NIFTITRANSFORM_DECLARE__
#include "NiftiTransformEnum.h"
#undef __NIFTITRANSFORM_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
NiftiTransformEnum::NiftiTransformEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const AString& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

/**
 * Destructor.
 */
NiftiTransformEnum::~NiftiTransformEnum()
{
}

void
NiftiTransformEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_UNKNOWN, 0,"NIFTI_XFORM_UNKNOWN"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_SCANNER_ANAT, 1,"NIFTI_XFORM_SCANNER_ANAT"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_ALIGNED_ANAT, 2,"NIFTI_XFORM_ALIGNED_ANAT"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_TALAIRACH, 3,"NIFTI_XFORM_TALAIRACH"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_MNI_152, 4,"NIFTI_XFORM_MNI_152"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const NiftiTransformEnum*
NiftiTransformEnum::findData(const Enum e)
{
    initialize();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const NiftiTransformEnum* d = &enumData[i];
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
AString 
NiftiTransformEnum::toName(Enum e) {
    initialize();
    
    const NiftiTransformEnum* nt = findData(e);
    return nt->name;
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
NiftiTransformEnum::Enum 
NiftiTransformEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiTransformEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTransformEnum& d = *iter;
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
 * Get the integer code associated with a transform.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with a transform.
 */
int32_t 
NiftiTransformEnum::toIntegerCode(Enum e)
{
    initialize();
    const NiftiTransformEnum* nsu = findData(e);
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
NiftiTransformEnum::Enum 
NiftiTransformEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiTransformEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTransformEnum& nsu = *iter;
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


