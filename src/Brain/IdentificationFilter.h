#ifndef __IDENTIFICATION_FILTER_H__
#define __IDENTIFICATION_FILTER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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
#include "IdentificationFilterTabSelectionEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class IdentificationFilter : public CaretObject, public SceneableInterface {
        
    public:
        IdentificationFilter();
        
        virtual ~IdentificationFilter();
        
        IdentificationFilter(const IdentificationFilter&) = delete;

        IdentificationFilter& operator=(const IdentificationFilter&) = delete;

        IdentificationFilterTabSelectionEnum::Enum getTabFiltering() const;
        
        void setTabFiltering(const IdentificationFilterTabSelectionEnum::Enum tabFiltering);
        
        bool isShowCiftiLoadingEnabled() const;
        
        void setShowCiftiLoadingEnabled(const bool status);
        
        bool isShowBorderEnabled() const;
        
        void setShowBorderEnabled(const bool borderEnabled);
        
        bool isShowFociEnabled() const;
        
        void setShowFociEnabled(const bool fociEnabled);
        
        bool isShowVertexVoxelEnabled() const;
        
        void setShowVertexVoxelEnabled(const bool vertexVoxelEnabled);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        IdentificationFilterTabSelectionEnum::Enum m_tabFiltering = IdentificationFilterTabSelectionEnum::ALL_DISPLAYED_TABS;
        
        bool m_showCiftiLoadingEnabled = true;

        bool m_showBorderEnabled = true;

        bool m_showFociEnabled = true;

        bool m_showVertexVoxelEnabled = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IDENTIFICATION_FILTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_FILTER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_FILTER_H__
