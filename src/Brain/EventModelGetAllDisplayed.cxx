
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __EVENT_MODEL_GET_ALL_DISPLAYED_DECLARE__
#include "EventModelGetAllDisplayed.h"
#undef __EVENT_MODEL_GET_ALL_DISPLAYED_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventModelGetAllDisplayed 
 * \brief Event to get the displayed models.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventModelGetAllDisplayed::EventModelGetAllDisplayed()
: Event(EventTypeEnum::EVENT_MODEL_GET_ALL_DISPLAYED)
{
    
}

/**
 * Destructor.
 */
EventModelGetAllDisplayed::~EventModelGetAllDisplayed()
{
}

/**
 * @return A set containing the displayed models.
 */
std::set<Model*>
EventModelGetAllDisplayed::getModels() const
{
    return m_models;
}

/**
 * Add a model to the displayed models.
 * This method may be called more than once with the same model.
 *
 * @param model
 *     The model.
 */
void
EventModelGetAllDisplayed::addModel(Model* model)
{
    if (model == NULL) {
        return;
    }
    m_models.insert(model);
}

