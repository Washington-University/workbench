
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ZARR_GROUP_V2_JSON_FILE_DECLARE__
#include "ZarrV2GroupJsonFile.h"
#undef __ZARR_GROUP_V2_JSON_FILE_DECLARE__

#include <iostream>

#include <nlohmann/json.hpp>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ZarrV2GroupJsonFile 
 * \brief Group JSON file for Zarr Version 2
 * \ingroup OmeZarr
 *
 * https://zarr-specs.readthedocs.io/en/latest/v2/v2.0.html
 */

/**
 * Constructor.
 */
ZarrV2GroupJsonFile::ZarrV2GroupJsonFile()
: ZarrJsonFileBase()
{
    
}

/**
 * Destructor.
 */
ZarrV2GroupJsonFile::~ZarrV2GroupJsonFile()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ZarrV2GroupJsonFile::ZarrV2GroupJsonFile(const ZarrV2GroupJsonFile& obj)
: ZarrJsonFileBase(obj)
{
    this->copyHelperZarrV2GroupJsonFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ZarrV2GroupJsonFile&
ZarrV2GroupJsonFile::operator=(const ZarrV2GroupJsonFile& obj)
{
    if (this != &obj) {
        ZarrJsonFileBase::operator=(obj);
        this->copyHelperZarrV2GroupJsonFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ZarrV2GroupJsonFile::copyHelperZarrV2GroupJsonFile(const ZarrV2GroupJsonFile& obj)
{
    m_zarrFormat = obj.m_zarrFormat;
}

/**
 * Clear the file
 */
void
ZarrV2GroupJsonFile::clear()
{
    ZarrJsonFileBase::clear();
    m_zarrFormat = -1;
}

/**
 * @return The ZARR format
 */
int32_t
ZarrV2GroupJsonFile::getZarrFormat() const
{
    return m_zarrFormat;
}

/**
 * Must be override by child classes to read the file's content from JSON
 * @param json
 *    The JSON containing the file's content
 * @return
 *    Function result with Ok/Error.
 */
FunctionResult
ZarrV2GroupJsonFile::readContentFromJson(const nlohmann::json& json)
{
    const nlohmann::json::object_t jsonObject = json;
    
    clear();
    
    m_zarrFormat = -1;
    
    bool haveFormatFlag(false);
    for (auto& elem : jsonObject) {
        if (elem.first == "zarr_format") {
            if (elem.second.is_number_integer()) {
                haveFormatFlag = true;
                m_zarrFormat = elem.second;
            }
            else {
                return FunctionResult::error("zarr_format is not a number");
            }
        }
        else {
            std::cerr << "Unrecognized ZarrGroup child " << elem.first << std::endl;
        }
    }
    
    AString errorMessage;
    if ( ! haveFormatFlag) {
        errorMessage = "Did not read zarr_format child of or it is invalid in .zgroup file.";
    }
    
    return FunctionResult(errorMessage,
                          haveFormatFlag);
}
