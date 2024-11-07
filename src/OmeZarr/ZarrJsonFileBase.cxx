
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

#define __ZARR_JSON_FILE_BASE_DECLARE__
#include "ZarrJsonFileBase.h"
#undef __ZARR_JSON_FILE_BASE_DECLARE__


#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::ZarrJsonFileBase 
 * \brief Base class for reading various ZARR Json files
 * \ingroup OmeZarr
 */

/**
 * Constructor.
 */
ZarrJsonFileBase::ZarrJsonFileBase()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ZarrJsonFileBase::~ZarrJsonFileBase()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ZarrJsonFileBase::ZarrJsonFileBase(const ZarrJsonFileBase& obj)
: CaretObject(obj)
{
    this->copyHelperZarrJsonFileBase(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ZarrJsonFileBase&
ZarrJsonFileBase::operator=(const ZarrJsonFileBase& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperZarrJsonFileBase(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ZarrJsonFileBase::copyHelperZarrJsonFileBase(const ZarrJsonFileBase& /*obj*/)
{
    
}

/**
 * Clear the file
 */
void
ZarrJsonFileBase::clear() 
{
    
}

/**
 * @return The JSON filename assembled from input paths
 */
std::string
ZarrJsonFileBase::getJsonFilename() const
{
    return m_jsonFilename;
}

/**
 * @return The ZARR path
 */
AString 
ZarrJsonFileBase::getZarrPath() const
{
    return m_zarrPath;
}

/**
 * Read the file
 * @param driverType
 *    The driver type
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @param jsonFileRelativePath
 *    Relative path to JSON file for reading in relation to 'zarrPath'
 * @return
 *    Function result with ok/error.
 */
FunctionResult
ZarrJsonFileBase::readFile(const ZarrDriverTypeEnum::Enum driverType,
                           const AString& zarrPath,
                           const AString& jsonFileRelativePath)
{
    m_zarrPath = zarrPath;
    
    FunctionResultValue<nlohmann::json> jsonResult(readJsonFromFile(driverType,
                                                                    zarrPath,
                                                                    jsonFileRelativePath));
    if (jsonResult.isError()) {
        return FunctionResult::error(jsonResult.getErrorMessage());
    }
    
    try {
        return readContentFromJson(jsonResult.getValue());
    }
    catch (const nlohmann::json::exception& e) {
        return FunctionResult::error("nlohmann::exception message: "
                                     + AString(e.what()));
    }
}

/**
 * Read JSON from the file
 * @param driverType
 *    The driver type
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @param jsonFileRelativePath
 *    Relative path to JSON file for reading in relation to 'zarrPath'
 * @return
 *    Function result with ok/error.
 */
FunctionResultValue<nlohmann::json>
ZarrJsonFileBase::readJsonFromFile(const ZarrDriverTypeEnum::Enum driverType,
                                   const AString& zarrPath,
                                   const AString& jsonFileRelativePath)
{
    if (jsonFileRelativePath.isEmpty()) {
        return jsonError("Filename is empty for reading JSON.");
    }
    
    m_jsonFilename = (zarrPath.toStdString()
                      + "/"
                      + jsonFileRelativePath.toStdString());
    
    switch (driverType) {
        case ZarrDriverTypeEnum::INVALID:
            return jsonError("Driver type for reading JSON is invalid.");
            break;
        case ZarrDriverTypeEnum::LOCAL_FILE:
        {
            if ( ! std::filesystem::exists(m_jsonFilename)) {
                return jsonError("Filename does not exist");
            }
            
            try {
                std::filesystem::path filePath(m_jsonFilename);
                const std::uintmax_t numberOfBytes(std::filesystem::file_size(filePath));
                
                if (numberOfBytes <= 0) {
                    return jsonError("File is empty (0 bytes).");
                }
                
                std::ifstream inputStream(m_jsonFilename);
                if ( ! inputStream.is_open()) {
                    return jsonError("Unable to open file.");
                }
                
                std::vector<char> jsonCharacters(numberOfBytes);
                inputStream.seekg(0);
                if ( ! inputStream.read(&jsonCharacters[0], numberOfBytes)) {
                    return jsonError("Reading all content of file.");
                }
                
                const char* ptrStart(&jsonCharacters[0]);
                const char* ptrEnd(&jsonCharacters[jsonCharacters.size()]);
                
                try {
                    const nlohmann::json::parser_callback_t callbackFunction = nullptr;
                    const bool allowExceptions = true;
                    nlohmann::json jsonRead = nlohmann::json::parse(ptrStart,
                                                                    ptrEnd,
                                                                    callbackFunction,
                                                                    allowExceptions);
                    /*
                     * Success, return the JSON that was read
                     */
                    const AString errorMessage("");
                    const bool okFlag(true);
                    return FunctionResultValue<nlohmann::json>(jsonRead,
                                                               errorMessage,
                                                               okFlag);
                }
                catch (const nlohmann::json::parse_error& e)
                {
                    return jsonError("Parse error: "
                                     + AString(e.what()));
                }
            }
            catch (std::filesystem::filesystem_error& e) {
                return jsonError("Filesystem error: "
                                  + AString(e.what()));
            }
        }
            break;
        case ZarrDriverTypeEnum::LOCAL_ZIP_FILE:
            return jsonError("Zip files not supported yet.");
            break;
    }
}

/**
 * @return Simplifies returning an an error message for a JSON error
 * @param errorMessage
 *    The error message
 */
FunctionResultValue<nlohmann::json>
ZarrJsonFileBase::jsonError(const AString& errorMessage) const
{
    AString filename(getJsonFilename());
    if (filename.isEmpty()) {
        filename = "invalid/unknown";
    }
    const AString text = ("File: " + filename
                          + " Error: " + errorMessage);

    const bool okFlag(false);
    return FunctionResultValue<nlohmann::json>(nlohmann::json::object_t(),
                                               text,
                                               okFlag);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ZarrJsonFileBase::toString() const
{
    return "ZarrJsonFileBase";
}

