
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_DECLARE__
#include "GraphicsLineMeanDeviationSettings.h"
#undef __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsLineMeanDeviationSettings 
 * \brief Settings for altering a line chart
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsLineMeanDeviationSettings::GraphicsLineMeanDeviationSettings()
: CaretObject()
{
    reset();
}

/**
 * Destructor.
 */
GraphicsLineMeanDeviationSettings::~GraphicsLineMeanDeviationSettings()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsLineMeanDeviationSettings::GraphicsLineMeanDeviationSettings(const GraphicsLineMeanDeviationSettings& obj)
: CaretObject(obj)
{
    this->copyHelperGraphicsLineMeanDeviationSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
GraphicsLineMeanDeviationSettings&
GraphicsLineMeanDeviationSettings::operator=(const GraphicsLineMeanDeviationSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperGraphicsLineMeanDeviationSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsLineMeanDeviationSettings::copyHelperGraphicsLineMeanDeviationSettings(const GraphicsLineMeanDeviationSettings& obj)
{
    m_mapFile                   = obj.m_mapFile;
    m_mapIndex                  = obj.m_mapIndex;
    m_numberOfMaps              = obj.m_numberOfMaps;
    m_cartesianLineData         = obj.m_cartesianLineData;
    m_numberOfCartesianVertices = obj.m_numberOfCartesianVertices;
    
    m_newMeanValue              = obj.m_newMeanValue;
    m_newDeviationValue         = obj.m_newDeviationValue;
    m_newMeanEnabled            = obj.m_newMeanEnabled;
    m_newDeviationEnabled       = obj.m_newDeviationEnabled;
    m_absoluteValueEnabled      = obj.m_absoluteValueEnabled;
    m_addToMeanEnabled          = obj.m_addToMeanEnabled;
    m_addToMeanValue            = obj.m_addToMeanValue;
    m_multiplyDeviationEnabled  = obj.m_multiplyDeviationEnabled;
    m_multiplyDeviationValue    = obj.m_multiplyDeviationValue;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
GraphicsLineMeanDeviationSettings::operator==(const GraphicsLineMeanDeviationSettings& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* perform equality testing HERE and return true if equal ! */
    if (   (m_mapFile                   == obj.m_mapFile)
        && (m_mapIndex                  == obj.m_mapIndex)
        && (m_numberOfMaps              == obj.m_numberOfMaps)
        && (m_cartesianLineData         == obj.m_cartesianLineData)
        && (m_numberOfCartesianVertices == obj.m_numberOfCartesianVertices)
        
        && (m_newMeanValue              == obj.m_newMeanValue)
        && (m_newDeviationValue         == obj.m_newDeviationValue)
        && (m_newMeanEnabled            == obj.m_newMeanEnabled)
        && (m_newDeviationEnabled       == obj.m_newDeviationEnabled)
        && (m_absoluteValueEnabled      == obj.m_absoluteValueEnabled)
        && (m_addToMeanEnabled          == obj.m_addToMeanEnabled)
        && (m_addToMeanValue            == obj.m_addToMeanValue)
        && (m_multiplyDeviationEnabled  == obj.m_multiplyDeviationEnabled)
        && (m_multiplyDeviationValue    == obj.m_multiplyDeviationValue)) {
        return true;
    }

    return false;    
}

/**
 * @return True if any modifications to data are enabled
 */
bool
GraphicsLineMeanDeviationSettings::anyModificationEnabled() const
{
    if (m_newMeanEnabled ||
        m_newDeviationEnabled ||
        m_absoluteValueEnabled ||
        m_addToMeanEnabled ||
        m_multiplyDeviationEnabled) {
        return true;
    }

    return false;
}


/**
 * Reset to default values
 */
void
GraphicsLineMeanDeviationSettings::reset()
{
    m_mapFile                   = NULL;
    m_mapIndex                  = -1;
    m_numberOfMaps              = -1;
    m_cartesianLineData         = NULL;
    m_numberOfCartesianVertices = -1;
    
    m_newMeanValue              = 0.0;
    m_newDeviationValue         = 1.0;
    m_newMeanEnabled            = false;
    m_newDeviationEnabled       = false;
    m_absoluteValueEnabled      = false;
    m_addToMeanEnabled          = false;
    m_addToMeanValue            = 0.0;
    m_multiplyDeviationEnabled  = false;
    m_multiplyDeviationValue    = 1.0;
}

/**
 * Update the file settings
 * @param mapFile
 *    The map file
 * @param mapIndex
 *    The selected map index
 * @param numberOfMaps
 *    Number of maps in file
 * @param cartesianLineData
 *    The cartesian line data
 * @param numberOfCartesianVertices
 *    Number of vertices in the cartesian line data
 */
void
GraphicsLineMeanDeviationSettings::updateFileSettings(CaretMappableDataFile* mapFile,
                                                      int32_t mapIndex,
                                                      int32_t numberOfMaps,
                                                      ChartTwoDataCartesian* cartesianLineData,
                                                      int32_t numberOfCartesianVertices)
{
    m_mapFile                   = mapFile;
    m_mapIndex                  = mapIndex;
    m_numberOfMaps              = numberOfMaps;
    m_cartesianLineData         = cartesianLineData;
    m_numberOfCartesianVertices = numberOfCartesianVertices;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsLineMeanDeviationSettings::toString() const
{
    return "GraphicsLineMeanDeviationSettings";
}

