
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

#define __OME_ATTRS_V0P4_JSON_FILE_DECLARE__
#include "OmeAttrsV0p4JsonFile.h"
#undef __OME_ATTRS_V0P4_JSON_FILE_DECLARE__

#include <iostream>

#include <nlohmann/json.hpp>

#include "CaretAssert.h"
#include "OmeAxis.h"
#include "OmeDataSet.h"

using namespace caret;


    
/**
 * \class caret::OmeAttrsV0p4JsonFile 
 * \brief .zattrs file version 0.4 (zero point four) for OmeZarr
 * \ingroup OmeZarr
 *
 * The .zattrs file describes how OME organizes a ZARR file
 *  https://ngff.openmicroscopy.org/0.4/index.html
 */

/**
 * Constructor.
 */
OmeAttrsV0p4JsonFile::OmeAttrsV0p4JsonFile()
: ZarrJsonFileBase()
{
    
}

/**
 * Destructor.
 */
OmeAttrsV0p4JsonFile::~OmeAttrsV0p4JsonFile()
{
}

/**
 * Clear the file
 */
void
OmeAttrsV0p4JsonFile::clear()
{
    ZarrJsonFileBase::clear();
    m_name = "";
    m_version = OmeVersionEnum::UNKNOWN;
    m_axes.clear();
    m_dataSets.clear();
    
    m_dimensionIndices = OmeDimensionIndices();
}

/**
 * @return The name
 */
QString
OmeAttrsV0p4JsonFile::getName() const
{
    return m_name;
}

/**
 * @return The version
 */
OmeVersionEnum::Enum
OmeAttrsV0p4JsonFile::getVersion() const
{
    return m_version;
}

/**
 * @return The number of axes.
 */
int32_t
OmeAttrsV0p4JsonFile::getNumberOfAxes() const
{
    return m_axes.size();
}

/**
 * @return Axis at the given index
 * @param index
 *    Index of axis
 */
OmeAxis*
OmeAttrsV0p4JsonFile::getAxis(const int32_t index)
{
    CaretAssertVectorIndex(m_axes, index);
    return &m_axes[index];
}

/**
 * @return Axis at the given index
 * @param index
 *    Index of axis
 */
const OmeAxis*
OmeAttrsV0p4JsonFile::getAxis(const int32_t index) const
{
    CaretAssertVectorIndex(m_axes, index);
    return &m_axes[index];
}

/**
 * @return Number of data sets
 */
int32_t
OmeAttrsV0p4JsonFile::getNumberOfDataSets() const
{
    return m_dataSets.size();
}

/**
 * @return Data set at the given index
 * @param index
 *    Index of the data set
 */
OmeDataSet*
OmeAttrsV0p4JsonFile::getDataSet(const int32_t index)
{
    CaretAssertVectorIndex(m_dataSets, index);
    return m_dataSets[index].get();
}

/**
 * @return Data set at the given index
 * @param index
 *    Index of the data set
 */
const OmeDataSet*
OmeAttrsV0p4JsonFile::getDataSet(const int32_t index) const
{
    CaretAssertVectorIndex(m_dataSets, index);
    return m_dataSets[index].get();
}

/**
 * @return The dimension indices
 */
const OmeDimensionIndices&
OmeAttrsV0p4JsonFile::getDimensionIndices() const
{
    return m_dimensionIndices;
}

/**
 * Must be override by child classes to read the file's content from JSON
 * @param json
 *    The JSON containing the file's content
 * @return
 *    Function result with Ok/Error.
 */
FunctionResult
OmeAttrsV0p4JsonFile::readContentFromJson(const nlohmann::json& json)
{
    /*
     * Find 'multiscales' array
     */
    if ( ! json.contains("multiscales")) {
        return FunctionResult::error("multiscales not found in .attrs file");
    }
    const auto multiscalesJson = json.at("multiscales")[0];
    
    /*
     * Find 'version'
     */
    const std::string versionText(multiscalesJson.at("version").get<std::string>());
    bool versionValidFlag(false);
    m_version = OmeVersionEnum::fromGuiName(versionText,
                                            &versionValidFlag);
    if ( ! versionValidFlag) {
        return FunctionResult::error(("OME-NGFF version"
                                      + versionText
                                      + " is not supported"));
    }
    
    /*
     * Read 'axes'
     */
    if (multiscalesJson.contains("axes")) {
        FunctionResult result(parseZattsAxesJson(multiscalesJson.at("axes")));
        if (result.isError()) {
            return result;
        }
    }
    
    /*
     * Read 'datasets'
     */
    if (multiscalesJson.contains("datasets")) {
        FunctionResult result(parseZattsDatasetsJson(multiscalesJson.at("datasets")));
        if (result.isError()) {
            return result;
        }
    }
    
    return FunctionResult::ok();
}

/**
 * Read the 'axes' from the .zattrs file jason
 * @param json
 *    The JSON
 * @return
 *    Result with success or error
 */
FunctionResult
OmeAttrsV0p4JsonFile::parseZattsAxesJson(const nlohmann::json& json)
{
    for (const auto& axis : json) {
        const AString axisDimName(AString::fromStdString(axis.at("name")));
        const AString axisTypeName(AString::fromStdString(axis.at("type")));
        const AString axisUnitName(AString::fromStdString(axis.contains("unit")
                                                          ? axis.at("unit")
                                                          : ""));
        
        AString errorMessage;
        
        OmeAxis omeAxis;
        
        if (axisDimName.isEmpty()) {
            errorMessage.appendWithNewLine("axes name is empty.");
        }
        else {
            omeAxis.setName(axisDimName);
        }
        
        bool axisTypeValidFlag(false);
        OmeAxisTypeEnum::Enum axisType(OmeAxisTypeEnum::fromLowerCaseName(axisTypeName,
                                                                          &axisTypeValidFlag));
        if (axisTypeValidFlag) {
            omeAxis.setAxisType(axisType);
        }
        else {
            errorMessage.appendWithNewLine("axes type invalid='" + axisTypeName + "'");
        }
        
        switch (axisType) {
            case OmeAxisTypeEnum::UNKNOWN:
                break;
            case OmeAxisTypeEnum::CHANNEL:
                break;
            case OmeAxisTypeEnum::SPACE:
            {
                bool spaceUnitValidFlag(false);
                OmeSpaceUnitEnum::Enum spaceUnit(OmeSpaceUnitEnum::fromLowerCaseName(axisUnitName,
                                                                                     &spaceUnitValidFlag));
                if (spaceUnitValidFlag) {
                    omeAxis.setSpaceUnit(spaceUnit);
                }
                else {
                    errorMessage.appendWithNewLine("axes space unit invalid='" + axisUnitName + "'");
                }
            }
                break;
            case OmeAxisTypeEnum::TIME:
                bool timeUnitValidFlag(false);
                OmeTimeUnitEnum::Enum timeUnit(OmeTimeUnitEnum::fromLowerCaseName(axisUnitName,
                                                                                  &timeUnitValidFlag));
                if (timeUnitValidFlag) {
                    omeAxis.setTimeUnit(timeUnit);
                }
                else {
                    errorMessage.appendWithNewLine("axes time unit invalid='" + axisUnitName + "'");
                }
                break;
        }
        
        if ( ! errorMessage.isEmpty()) {
            return FunctionResult::error(errorMessage);
        }
        
        m_axes.push_back(omeAxis);
    }
    
    int64_t xDimensionIndex(-1);
    int64_t yDimensionIndex(-1);
    int64_t zDimensionIndex(-1);
    int64_t timeDimensionIndex(-1);
    int64_t channelDimensionIndex(-1);

    int32_t dimensionIndex(0);
    for (const auto& axis : m_axes) {
        switch (axis.getAxisType()) {
            case OmeAxisTypeEnum::UNKNOWN:
                return FunctionResult::error("Axis dimension index="
                                             + AString::number(dimensionIndex)
                                             + " is of UNKNOWN type.");
                break;
            case OmeAxisTypeEnum::CHANNEL:
                if (channelDimensionIndex >= 0) {
                    return FunctionResult::error("More than one dimension is of type channel. "
                                                 "This is not allowed.");
                }
                else {
                    channelDimensionIndex = dimensionIndex;
                }
                break;
            case OmeAxisTypeEnum::TIME:
                if (timeDimensionIndex >= 0) {
                    return FunctionResult::error("More than one dimension is of type type. "
                                                 "This is not allowed.");
                }
                else {
                    timeDimensionIndex = dimensionIndex;
                }
                break;
            case OmeAxisTypeEnum::SPACE:
            {
                if (axis.getName().toLower() == "x") {
                    if (xDimensionIndex >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'x'");
                    }
                    else {
                        xDimensionIndex = dimensionIndex;
                    }
                }
                else if (axis.getName().toLower() == "y") {
                    if (yDimensionIndex >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'y'");
                    }
                    else {
                        yDimensionIndex = dimensionIndex;
                    }
                }
                else if (axis.getName().toLower() == "z") {
                    if (zDimensionIndex >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'z'");
                    }
                    else {
                        zDimensionIndex = dimensionIndex;
                    }
                }
                else {
                    return FunctionResult::error(axis.getName()
                                                 + " is not a recognized as a spatial index for an axis");
                }
            }
        }
        
        ++dimensionIndex;
    }
    
    if (xDimensionIndex < 0) {
        return FunctionResult::error("Dimension for 'X' not found.");
    }
    if (yDimensionIndex < 0) {
        return FunctionResult::error("Dimension for 'Y' not found.");
    }
    if (zDimensionIndex < 0) {
        return FunctionResult::error("Dimension for 'Z' not found.");
    }
    
    m_dimensionIndices = OmeDimensionIndices(xDimensionIndex,
                                             yDimensionIndex,
                                             zDimensionIndex,
                                             timeDimensionIndex,
                                             channelDimensionIndex);
    return FunctionResult::ok();
}

/**
 * Read the 'datasets' from the .zattrs file jason
 * @param json
 *    The JSON
 * @return
 *    Result with success or error
 */
FunctionResult
OmeAttrsV0p4JsonFile::parseZattsDatasetsJson(const nlohmann::json& json)
{
    for (const auto& dataset : json) {
        const AString pathName(AString::fromStdString(dataset.at("path")));
        
        std::unique_ptr<OmeDataSet> dataSet(new OmeDataSet());
        dataSet->setRelativePath(pathName);
        
        const auto coordTransArray(dataset.at("coordinateTransformations"));
        for (const auto& coordTrans : coordTransArray) {
            if (coordTrans.contains("type")) {
                OmeCoordinateTransformationTypeEnum::Enum octType(OmeCoordinateTransformationTypeEnum::INVALID);
                std::vector<float> values;
                const auto typeName(coordTrans.at("type"));
                if (typeName == "scale") {
                    if (coordTrans.contains("scale")) {
                        const auto scaleArray(coordTrans.at("scale"));
                        for (const auto& v : scaleArray) {
                            values.push_back(v.get<float>());
                        }
                        octType = OmeCoordinateTransformationTypeEnum::SCALE;
                    }
                }
                else if (typeName == "translate") {
                    if (coordTrans.contains("translate")) {
                        const auto transArray(coordTrans.at("translate"));
                        for (const auto& v : transArray) {
                            values.push_back(v.get<float>());
                        }
                        octType = OmeCoordinateTransformationTypeEnum::TRANSLATE;
                    }
                }
                else {
                    return FunctionResult::error("Unrecognized coordinateTransformations::type: "
                                                 + typeName.get<std::string>());
                }
                
                if ((octType != OmeCoordinateTransformationTypeEnum::INVALID)
                    & ( ! values.empty())) {
                    OmeCoordinateTransformations oct(octType,
                                                     values);
                    dataSet->addCoordinateTransformation(oct);
                }
                else {
                    return FunctionResult::error("Reading datasets");
                }
            }
        }
        
        m_dataSets.push_back(std::move(dataSet));
    }
    
    return FunctionResult::ok();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
OmeAttrsV0p4JsonFile::toString() const
{
    AString txt;
    
    txt.appendWithNewLine("Name: " + m_name);
    txt.appendWithNewLine("Version: " + OmeVersionEnum::toGuiName(m_version));
    txt.appendWithNewLine("Indices: " + m_dimensionIndices.toString());
    
    txt.appendWithNewLine("Axes: ");
    for (const auto& axis : m_axes) {
        txt.appendWithNewLine(axis.toString());
    }
    
    txt.appendWithNewLine("DataSets:");
    for (const auto& ds : m_dataSets) {
        txt.appendWithNewLine(ds->toString());
    }
    return txt;
}

