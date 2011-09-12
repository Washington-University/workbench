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

#include <algorithm>

#include "EventModelDisplayControllerGetAll.h"

using namespace caret;

/**
 * Constructor.
 */
EventModelDisplayControllerGetAll::EventModelDisplayControllerGetAll()
: Event(EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_GET_ALL)
{
}

/**
 * Destructor.
 */
EventModelDisplayControllerGetAll::~EventModelDisplayControllerGetAll()
{
    
}

/**
 * Add model display controllers.
 
 * @param modelDisplayControllersToAdd
 *    These model display controllers are added.
 */
void 
EventModelDisplayControllerGetAll::addModelDisplayControllers(
        const std::vector<ModelDisplayController*>& modelDisplayControllersToAdd)
{
    this->modelDisplayControllers.insert(this->modelDisplayControllers.end(),
                                         modelDisplayControllersToAdd.begin(),
                                         modelDisplayControllersToAdd.end());
}

/**
 * Get the model display controllers. 
 *
 * @return vector containing the model display controllers.
 */
const std::vector<ModelDisplayController*> 
EventModelDisplayControllerGetAll::getModelDisplayControllers() const
{
    return this->modelDisplayControllers;
}

/**
 * Is a model display controller valid?
 *
 * @param modelDisplayController
 *    Model display controller that is checked for validity.
 * 
 * @return true if valid, else false.
 */
bool 
EventModelDisplayControllerGetAll::isModelDisplayControllerValid(
        const ModelDisplayController* modelDisplayController) const
{
    if (std::find(this->modelDisplayControllers.begin(), 
                  this->modelDisplayControllers.end(), 
                  modelDisplayController) != this->modelDisplayControllers.end()) { 
        return true;
    }
    
    return false;
}

/**
 * Get the first model display controller.
 * 
 * @return Pointer to first model controller or
 *    NULL if there are no model controllers.
 */
ModelDisplayController* 
EventModelDisplayControllerGetAll::getFirstModelDisplayController() const
{
    if (this->modelDisplayControllers.empty() == false) {
        return this->modelDisplayControllers[0];
    }
    
    return NULL;
}

