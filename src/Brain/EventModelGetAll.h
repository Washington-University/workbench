#ifndef __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__
#define __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <vector>

#include "Event.h"

namespace caret {

    class Model;
    class ModelSurface;
    
    
    /// Event for getting models
    class EventModelGetAll : public Event {
        
    public:
        EventModelGetAll();
        
        virtual ~EventModelGetAll();
        
        void addModels(const std::vector<Model*>& modelsToAdd);

        const std::vector<Model*> getModels() const;
        
        bool isModelValid(const Model* model) const;
        
        Model* getFirstModel() const;
        
        ModelSurface* getFirstModelSurface() const;
        
    private:
        EventModelGetAll(const EventModelGetAll&);
        
        EventModelGetAll& operator=(const EventModelGetAll&);
        
        std::vector<Model*> m_models;
    };

} // namespace

#endif // __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__
