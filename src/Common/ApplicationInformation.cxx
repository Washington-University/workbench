
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

#include <QtGlobal>

#define __APPLICATION_INFORMATION_DECLARE__
#include "ApplicationInformation.h"
#undef __APPLICATION_INFORMATION_DECLARE__

using namespace caret;


    
/**
 * \class caret::ApplicationInformation 
 * \brief Provides application information.
 *
 * Provides application information (name, version, etc).
 */

/**
 * Constructor.
 */
ApplicationInformation::ApplicationInformation()
: CaretObject()
{
    this->name    = "Workbench";
    this->version = "Beta-1";
}

/**
 * Destructor.
 */
ApplicationInformation::~ApplicationInformation()
{
    
}

/**
 * @return Name of the application.
 */
AString 
ApplicationInformation::getName() const
{
    return this->name;
}

/**
 * @return Version of application.
 */
AString 
ApplicationInformation::getVersion() const
{
    return this->version;
}

/**
 * Get all information.
 * @param informationValues
 *    Output information.
 */
void 
ApplicationInformation::getAllInformation(std::vector<AString>& informationValues) const
{
    informationValues.clear();
    
    informationValues.push_back(this->name);
    
    informationValues.push_back("Version: " + this->version);
    
    informationValues.push_back("Qt Compiled Version: " + QString(QT_VERSION_STR));
    
    informationValues.push_back("Qt Runtime Version: " + QString(qVersion()));
}

