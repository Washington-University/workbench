#ifndef __WHOLE_BRAIN_SURFACE_SETTINGS_H__
#define __WHOLE_BRAIN_SURFACE_SETTINGS_H__

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


#include "CaretObject.h"

#include "SceneableInterface.h"

namespace caret {
    class SceneClassAssistant;

    class WholeBrainSurfaceSettings : public CaretObject, public SceneableInterface {
        
    public:
        WholeBrainSurfaceSettings();
        
        virtual ~WholeBrainSurfaceSettings();
        
        WholeBrainSurfaceSettings(const WholeBrainSurfaceSettings& obj);

        WholeBrainSurfaceSettings& operator=(const WholeBrainSurfaceSettings& obj);
        
        bool isLeftEnabled() const;
        
        void setLeftEnabled(const bool enabled);
        
        bool isRightEnabled() const;
        
        void setRightEnabled(const bool enabled);
        
        bool isCerebellumEnabled() const;
        
        void setCerebellumEnabled(const bool enabled);
        
        bool isHippocampusEnabled() const;
        
        void setHippocampusEnabled(const bool enabled);
        
        bool isHippocampusLeftEnabled() const;
        
        void setHippocampusLeftEnabled(const bool enabled);
        
        bool isHippocampusRightEnabled() const;
        
        void setHippocampusRightEnabled(const bool enabled);
        
        bool isDentateHippocampusLeftEnabled() const;
        
        void setDentateHippocampusLeftEnabled(const bool enabled);
        
        bool isDentateHippocampusRightEnabled() const;
        
        void setDentateHippocampusRightEnabled(const bool enabled);

        float getLeftRightSeparation() const;
        
        void setLeftRightSeparation(const float separation);
        
        float getCerebellumSeparation() const;
        
        void setCerebellumSeparation(const float separation);
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

    private:
        void copyHelperWholeBrainSurfaceSettings(const WholeBrainSurfaceSettings& obj);

        SceneClassAssistant* m_sceneAssistant;

        bool m_leftEnabled;
        
        bool m_rightEnabled;
        
        bool m_cerebellumEnabled;
        
        bool m_hippocampusEnabled;
        
        bool m_hippocampusLeftEnabled;
        
        bool m_hippocampusRightEnabled;
        
        bool m_dentateHippocampusLeftEnabled;
        
        bool m_dentateHippocampusRightEnabled;
        
        float m_leftRightSeparation;
        
        float m_cerebellumSeparation;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WHOLE_BRAIN_SURFACE_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WHOLE_BRAIN_SURFACE_SETTINGS_DECLARE__

} // namespace
#endif  //__WHOLE_BRAIN_SURFACE_SETTINGS_H__
