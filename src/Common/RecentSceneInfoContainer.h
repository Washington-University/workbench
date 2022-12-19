#ifndef __RECENT_SCENE_INFO_CONTAINER_H__
#define __RECENT_SCENE_INFO_CONTAINER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class RecentSceneInfoContainer : public CaretObject {
        
    public:
        RecentSceneInfoContainer(const AString& sceneFileName,
                                 const AString& sceneName);
        
        RecentSceneInfoContainer(const QVariant& variant,
                                 bool& validFlagOut);
        
        virtual ~RecentSceneInfoContainer();
        
        RecentSceneInfoContainer(const RecentSceneInfoContainer& obj);

        RecentSceneInfoContainer& operator=(const RecentSceneInfoContainer& obj);
        
        bool operator==(const RecentSceneInfoContainer& obj) const;
        
        AString getSceneFileName() const;
        
        AString getSceneName() const;
        
        QVariant toQVariant() const;
        
    private:
        void copyHelperRecentSceneInfoContainer(const RecentSceneInfoContainer& obj);

        AString m_sceneFileName;
        
        AString m_sceneName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_SCENE_INFO_CONTAINER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_SCENE_INFO_CONTAINER_DECLARE__

} // namespace
#endif  //__RECENT_SCENE_INFO_CONTAINER_H__
