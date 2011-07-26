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
#include "NiftiDataType.h"
#undef __NIFTI_DATA_TYPE_DECLARE__


using namespace caret;

NiftiDataType::NiftiDataType()
{
    
}

NiftiDataType::NiftiDataType(Enum e, const std::string& name)
{
    this->e = e;
    this->name = name;
}

NiftiDataType::~NiftiDataType()
{
    
}

void 
NiftiDataType::createDataTypes()
{
    if (dataTypesCreatedFlag) {
        return;
    }
    dataTypesCreatedFlag = true;
    
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_INVALID,
                                     "NIFTI_DATA_TYPE_INVALID"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_UINT8,
                                     "NIFTI_TYPE_UINT8"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_INT16,
                                     "NIFTI_TYPE_INT16"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_INT32,
                                     "NIFTI_TYPE_INT32"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_FLOAT32,
                                     "NIFTI_TYPE_FLOAT32"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_COMPLEX64,
                                     "NIFTI_TYPE_COMPLEX64"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_FLOAT64,
                                     "NIFTI_TYPE_FLOAT64"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_RGB24,
                                     "NIFTI_TYPE_RGB24"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_INT8,
                                     "NIFTI_TYPE_INT8"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_UINT16,
                                     "NIFTI_TYPE_UINT16"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_UINT32,
                                     "NIFTI_TYPE_UINT32"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_INT64,
                                     "NIFTI_TYPE_INT64"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_UINT64,
                                     "NIFTI_TYPE_UINT64"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_FLOAT128,
                                     "NIFTI_TYPE_FLOAT128"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_COMPLEX128,
                                     "NIFTI_TYPE_COMPLEX128"));
    dataTypes.push_back(NiftiDataType(NIFTI_TYPE_COMPLEX256,
                                     "NIFTI_TYPE_COMPLEX256"));
}

/**
 * Get a string representition of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
NiftiDataType::toString(Enum e, bool* isValidOut) {
    createDataTypes();
    
    std::string s;
    
    for (std::vector<NiftiDataType>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataType& ndt = *iter;
        if (ndt.e == e) {
            s = ndt.name;
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
NiftiDataType::Enum 
NiftiDataType::fromString(const std::string& s, bool* isValidOut)
{
    createDataTypes();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiDataType>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataType& ndt = *iter;
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
