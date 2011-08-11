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

#define __NIFTI_DATA_TYPE_DECLARE__
#include "NiftiDataTypeEnum.h"
#undef __NIFTI_DATA_TYPE_DECLARE__


using namespace caret;

NiftiDataTypeEnum::NiftiDataTypeEnum()
{
    
}

NiftiDataTypeEnum::NiftiDataTypeEnum(Enum e, const AString& name, const int32_t integerCode)
{
    this->e = e;
    this->name = name;
    this->integerCode = integerCode;
}

NiftiDataTypeEnum::~NiftiDataTypeEnum()
{
    
}

void 
NiftiDataTypeEnum::createDataTypes()
{
    if (dataTypesCreatedFlag) {
        return;
    }
    dataTypesCreatedFlag = true;
    
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INVALID,
                                     "NIFTI_DATA_TYPE_NONE",
                                      0));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT8,
                                     "NIFTI_TYPE_UINT8",
                                      2));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT16,
                                     "NIFTI_TYPE_INT16",
                                      4));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT32,
                                     "NIFTI_TYPE_INT32",
                                      8));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT32,
                                     "NIFTI_TYPE_FLOAT32",
                                      16));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX64,
                                     "NIFTI_TYPE_COMPLEX64",
                                      32));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT64,
                                     "NIFTI_TYPE_FLOAT64",
                                     64));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_RGB24,
                                     "NIFTI_TYPE_RGB24",
                                      128));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT8,
                                     "NIFTI_TYPE_INT8",
                                      256));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT16,
                                     "NIFTI_TYPE_UINT16",
                                      512));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT32,
                                     "NIFTI_TYPE_UINT32",
                                      768));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT64,
                                     "NIFTI_TYPE_INT64",
                                      1024));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT64,
                                     "NIFTI_TYPE_UINT64",
                                      1280));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT128,
                                     "NIFTI_TYPE_FLOAT128",
                                      1792));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX128,
                                     "NIFTI_TYPE_COMPLEX128",
                                      1792));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX256,
                                     "NIFTI_TYPE_COMPLEX256",
                                      2048));
}

/**
 * Get a string representition of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
NiftiDataTypeEnum::toName(Enum e) {
    createDataTypes();

    const NiftiDataTypeEnum* ndt = findData(e);
    return ndt->name;
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
NiftiDataTypeEnum::Enum 
NiftiDataTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    createDataTypes();
    
    bool validFlag = false;
    Enum e = NIFTI_TYPE_FLOAT32;
    
    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
        if (ndt.name == s) {
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

/**
 * Find the Data Type using the enum.
 *
 * @return 
 *    Object using enum or NULL if not found.
 */
const NiftiDataTypeEnum* 
NiftiDataTypeEnum::findData(Enum e)
{
    createDataTypes();
    
    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
        return &ndt;
    }    
    return NULL;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t 
NiftiDataTypeEnum::toIntegerCode(Enum e)
{
    createDataTypes();
    const NiftiDataTypeEnum* ndt = findData(e);
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
NiftiDataTypeEnum::Enum 
NiftiDataTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    createDataTypes();
    
    bool validFlag = false;
    Enum e = NIFTI_TYPE_FLOAT32;
    
    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
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
