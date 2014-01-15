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

#include "EventModelGetAll.h"
#include "ModelSurface.h"

using namespace caret;

/**
 * Constructor.
 */
EventModelGetAll::EventModelGetAll()
: Event(EventTypeEnum::EVENT_MODEL_GET_ALL)
{
}

/**
 * Destructor.
 */
EventModelGetAll::~EventModelGetAll()
{
    
}

/**
 * Add models.
 
 * @param modelsToAdd
 *    These model added.
 */
void 
EventModelGetAll::addModels(
        const std::vector<Model*>& modelsToAdd)
{
    m_models.insert(m_models.end(),
                    modelsToAdd.begin(),
                    modelsToAdd.end());
}

/**
 * Get the model.
 *
 * @return vector containing the model.
 */
const std::vector<Model*> 
EventModelGetAll::getModels() const
{
    return this->m_models;
}

/**
 * Is a model valid?
 *
 * @param model
 *    Model that is checked for validity.
 * 
 * @return true if valid, else false.
 */
bool 
EventModelGetAll::isModelValid(
        const Model* model) const
{
    if (std::find(this->m_models.begin(), 
                  this->m_models.end(), 
                  model) != m_models.end()) {
        return true;
    }
    
    return false;
}

/**
 * Get the first model.
 * 
 * @return Pointer to first model or
 *    NULL if there are no model.
 */
Model* 
EventModelGetAll::getFirstModel() const
{
    if (m_models.empty() == false) {
        return m_models[0];
    }
    
    return NULL;
}

/**
 * Get the first model  surface.
 * 
 * @return Pointer to first model surface or
 *    NULL if there are no model surfaces.
 */
ModelSurface* 
EventModelGetAll::getFirstModelSurface() const
{
    ModelSurface* surfaceModelOut = NULL;
    
    const int32_t numModels = static_cast<int32_t>(m_models.size());
    for (int32_t i = 0; i < numModels; i++) {
        surfaceModelOut = dynamic_cast<ModelSurface*>(m_models[i]);
        if (surfaceModelOut != NULL) {
            break;
        }
    }
    
    return surfaceModelOut;
}


