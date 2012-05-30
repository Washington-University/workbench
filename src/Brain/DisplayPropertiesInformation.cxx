
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __DISPLAY_PROPERTIES_INFORMATION_DECLARE__
#include "DisplayPropertiesInformation.h"
#undef __DISPLAY_PROPERTIES_INFORMATION_DECLARE__

using namespace caret;


    
/**
 * \class DisplayPropertiesInformation 
 * \brief Display properties for information.
 */

/**
 * Constructor.
 */
DisplayPropertiesInformation::DisplayPropertiesInformation(Brain* brain)
: DisplayProperties(brain)
{
    this->contralateralIdentificationEnabled = false;
    this->volumeIdentificationEnabled = true;
    this->identificationSymbolColor = CaretColorEnum::GREEN;
    this->identificationContralateralSymbolColor = CaretColorEnum::BLUE;
    this->identifcationSymbolSize = 3.5;
}

/**
 * Destructor.
 */
DisplayPropertiesInformation::~DisplayPropertiesInformation()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesInformation::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesInformation::update()
{
}

/**
 * @return Status of contralateral identification.
 */
bool 
DisplayPropertiesInformation::isContralateralIdentificationEnabled() const
{
    return this->contralateralIdentificationEnabled;
}
        
/**
 * Set status of contralateral identification.
 * @param 
 *    New status.
 */
void 
DisplayPropertiesInformation::setContralateralIdentificationEnabled(const bool enabled)
{
    this->contralateralIdentificationEnabled = enabled;
}

/**
 * @return Status of volume identification.
 */
bool 
DisplayPropertiesInformation::isVolumeIdentificationEnabled() const
{
    return this->volumeIdentificationEnabled;
}

/**
 * Set status of volume identification.
 * @param 
 *    New status.
 */
void 
DisplayPropertiesInformation::setVolumeIdentificationEnabled(const bool enabled)
{
    this->volumeIdentificationEnabled = enabled;
}

/**
 * @return The size of the identification symbol
 */
float 
DisplayPropertiesInformation::getIdentificationSymbolSize() const
{
    return this->identifcationSymbolSize;
}

/**
 * Set the size of the identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void 
DisplayPropertiesInformation::setIdentificationSymbolSize(const float symbolSize)
{
    this->identifcationSymbolSize = symbolSize;
}

/**
 * @return The color of the identification symbol.
 */
CaretColorEnum::Enum 
DisplayPropertiesInformation::getIdentificationSymbolColor() const
{
    return this->identificationSymbolColor;
}

/**
 * Set the color of the identification symbol.
 * @param color
 *    New color.
 */
void 
DisplayPropertiesInformation::setIdentificationSymbolColor(const CaretColorEnum::Enum color)
{
    this->identificationSymbolColor = color;
}

/**
 * @return The color of the contralateral identification symbol.
 */
CaretColorEnum::Enum 
DisplayPropertiesInformation::getIdentificationContralateralSymbolColor() const
{
    return this->identificationContralateralSymbolColor;
}

/**
 * Set the color of the contralateral identification symbol.
 * @param color
 *    New color.
 */
void 
DisplayPropertiesInformation::setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color)
{
    this->identificationContralateralSymbolColor = color;
}


