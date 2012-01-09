
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

