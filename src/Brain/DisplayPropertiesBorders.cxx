
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __DISPLAY_PROPERTIES_BORDERS_DECLARE__
#include "DisplayPropertiesBorders.h"
#undef __DISPLAY_PROPERTIES_BORDERS_DECLARE__

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesBorders 
 * \brief Contains display properties for borders.
 */

/**
 * Constructor.
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesBorders::DisplayPropertiesBorders(Brain* brain)
: DisplayProperties(brain)
{
    this->displayStatus = true;
    this->contralateralDisplayStatus = true;
}

/**
 * Destructor.
 */
DisplayPropertiesBorders::~DisplayPropertiesBorders()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesBorders::reset()
{
    this->displayStatus = true;
    this->contralateralDisplayStatus = true;
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesBorders::update()
{
    
}

/**
 * @return  Display status of borders.
 */
bool 
DisplayPropertiesBorders::isDisplayed() const
{
    return this->displayStatus;
}

/**
 * Set the display status for borders.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setDisplayed(const bool displayStatus)
{
    this->displayStatus = displayStatus;
}

/**
 * @return  Contralateral display status of borders.
 */
bool 
DisplayPropertiesBorders::isContralateralDisplayed() const
{
    return this->contralateralDisplayStatus;
}

/**
 * Set the contralateral display status for borders.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setContralateralDisplayed(const bool contralateralDisplayStatus)
{
    this->contralateralDisplayStatus = contralateralDisplayStatus;
}
