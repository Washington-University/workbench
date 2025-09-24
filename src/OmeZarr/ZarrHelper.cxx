
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

#define __ZARR_HELPER_DECLARE__
#include "ZarrHelper.h"
#undef __ZARR_HELPER_DECLARE__

#include "CaretAssert.h"
#include "OmeAttrsV0p4JsonFile.h"
#include "ZarrV2ArrayJsonFile.h"
#include "ZarrV2GroupJsonFile.h"

#include <blosc.h>
#include <nlohmann/json.hpp>
#include <xtensor/core/xtensor_config.hpp>
#include <xtl/xtl_config.hpp>
#include <z5/metadata.hxx>

using namespace caret;
    
/**
 * \class caret::ZarrHelper 
 * \brief Helps with ZARR files
 * \ingroup OmeZarr
 */

/**
 * Constructor.
 */
ZarrHelper::ZarrHelper()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ZarrHelper::~ZarrHelper()
{
}

/**
 * @return Version info about ZARR and its dependent libraries
 */
std::vector<AString>
ZarrHelper::getVersionInfoStrings()
{
    std::vector<AString> txt;
    
    txt.push_back("BLOSC Version: "
                  + AString(blosc_get_version_string()));
    
    txt.push_back("nlohman json version: "
                  + AString(std::to_string(NLOHMANN_JSON_VERSION_MAJOR) + "." +
                            std::to_string(NLOHMANN_JSON_VERSION_MINOR) + "." +
                            std::to_string(NLOHMANN_JSON_VERSION_PATCH)));
    
    txt.push_back("xtensor version: "
                  + AString(std::to_string(XTENSOR_VERSION_MAJOR) + "." +
                            std::to_string(XTENSOR_VERSION_MINOR) + "." +
                            std::to_string(XTENSOR_VERSION_PATCH)));
    
    txt.push_back("xtl version: "
                  + AString(std::to_string(XTL_VERSION_MAJOR) + "." +
                            std::to_string(XTL_VERSION_MINOR) + "." +
                            std::to_string(XTL_VERSION_PATCH)));
    
    z5::DatasetMetadata z5metaData;
    txt.push_back("Z5 Version: "
                  + AString(z5metaData.n5Format()));
    
    return txt;
}


/**
 * Read the Zarr V2 .zarray file
 *
 * @param driverType
 *    The driver type
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @param relativePath
 *    Path within the zarr path
 * @return
 *    Result with ZarrV2GroupJsonFile
 */

FunctionResultValue<ZarrV2ArrayJsonFile*>
ZarrHelper::readZarrV2ArrayJsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                         const AString& zarrPath,
                                         const AString& relativePath)
{
    ZarrV2ArrayJsonFile* zarrArrayFile(new ZarrV2ArrayJsonFile());
    const AString zarrayName(relativePath
                             + "/.zarray");
    FunctionResult result(zarrArrayFile->readFile(driverType,
                                                  zarrPath,
                                                  zarrayName));
    if (result.isError()) {
        delete zarrArrayFile;
        zarrArrayFile = NULL;
    }
    
    return FunctionResultValue<ZarrV2ArrayJsonFile*>(zarrArrayFile,
                                                     result.getErrorMessage(),
                                                     result.isOk());
}

/**
 * @return Type of  driver appropriate for the given filename or error
 * @param filename
 *    Name of file
 */
FunctionResultValue<ZarrDriverTypeEnum::Enum>
ZarrHelper::getDriverTypeFromFilename(const AString& filename)
{
    ZarrDriverTypeEnum::Enum driverType(ZarrDriverTypeEnum::INVALID);
    
    AString errorMessage;
    
    const int32_t urlIndex(filename.indexOf("://"));
    if (urlIndex >= 0) {
        const AString protocol(filename.left(urlIndex));
        errorMessage = ("Protocol \"" + protocol + "\" not supported for reading ZARR files.");
    }
    else if (filename.endsWith(".zip")) {
        driverType = ZarrDriverTypeEnum::LOCAL_ZIP_FILE;
        
        /* Remove thenext two line when ZIP support is added */
        errorMessage = ("Reading from ZIP file not supported.");
        driverType   = ZarrDriverTypeEnum::INVALID;
    }
    else if ( ! filename.isEmpty()) {
        driverType = ZarrDriverTypeEnum::LOCAL_FILE;
    }
    else {
        errorMessage = ("Filename is empty.");
    }
    
    FunctionResultValue<ZarrDriverTypeEnum::Enum> result(driverType,
                                                         errorMessage,
                                                         errorMessage.isEmpty());
    return result;
}

/**
 * @return Simplifies returning an an error message for a JSON error
 * @param filename
 *    Name of the file (may be empty)
 * @param errorMessage
 *    The error message
 */
FunctionResultValue<nlohmann::json>
ZarrHelper::jsonError(const AString& filename,
                           const AString& errorMessage)
{
    AString text;
    if (filename.isEmpty()) {
        text = errorMessage;
    }
    else {
        text = ("File: " + filename
                + " Error: " + errorMessage);
    }
    const bool okFlag(false);
    return FunctionResultValue<nlohmann::json>(nlohmann::json::object_t(),
                                               text,
                                               okFlag);
}

/**
 * Read the Zarr V2 .group file
 *
 * @param driverType
 *    The driver type
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @return
 *    Result with ZarrV2GroupJsonFile
 */
FunctionResultValue<ZarrV2GroupJsonFile*>
ZarrHelper::readZarrV2GroupJsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                         const AString& zarrPath)
{
    ZarrV2GroupJsonFile* zarrGroupFile(new ZarrV2GroupJsonFile());
    const AString zgroupName(".zgroup");
    FunctionResult result(zarrGroupFile->readFile(driverType,
                                                  zarrPath,
                                                  zgroupName));
    if (result.isError()) {
        delete zarrGroupFile;
        zarrGroupFile = NULL;
    }
    
    return FunctionResultValue<ZarrV2GroupJsonFile*>(zarrGroupFile,
                                                     result.getErrorMessage(),
                                                     result.isOk());
}

/**
 * Read the OME ZARR .attrs  file
 *
 * @param driverType
 *    The driver type
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @return
 *    Result with OmeAttrsV0p4JsonFile
 */
FunctionResultValue<OmeAttrsV0p4JsonFile*>
ZarrHelper::readOmeAttrsV0p4JsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                              const AString& zarrPath)
{
    OmeAttrsV0p4JsonFile* zarrAttrsFile(new OmeAttrsV0p4JsonFile());
    const AString zattrsName(".zattrs");
    FunctionResult result(zarrAttrsFile->readFile(driverType,
                                                  zarrPath,
                                                  zattrsName));
    if (result.isError()) {
        delete zarrAttrsFile;
        zarrAttrsFile = NULL;
    }
    
    return FunctionResultValue<OmeAttrsV0p4JsonFile*>(zarrAttrsFile,
                                                          result.getErrorMessage(),
                                                          result.isOk());
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ZarrHelper::toString() const
{
    return "ZarrHelper";
}

