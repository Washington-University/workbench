#ifndef __GAPS_AND_MARGINS_H__
#define __GAPS_AND_MARGINS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "SceneableInterface.h"

namespace caret {
    class SceneClassAssistant;

    class GapsAndMargins : public CaretObject, public SceneableInterface {
        
    public:
        GapsAndMargins();
        
        virtual ~GapsAndMargins();
        
        float getTabMarginLeft(const int32_t tabIndex) const;
        
        float getTabMarginRight(const int32_t tabIndex) const;
        
        float getTabMarginBottom(const int32_t tabIndex) const;
        
        float getTabMarginTop(const int32_t tabIndex) const;
        
        void setTabMarginLeft(const int32_t tabIndex,
                              const float margin);
        
        void setTabMarginRight(const int32_t tabIndex,
                              const float margin);
        
        void setTabMarginBottom(const int32_t tabIndex,
                              const float margin);
        
        void setTabMarginTop(const int32_t tabIndex,
                              const float margin);
        
        float getSurfaceMontageHorizontalGap() const;
        
        float getSurfaceMontageVerticalGap() const;
        
        float getVolumeMontageHorizontalGap() const;
        
        float getVolumeMontageVerticalGap() const;
        
        void setSurfaceMontageHorizontalGap(const float gap);
        
        void setSurfaceMontageVerticalGap(const float gap);
        
        void setVolumeMontageHorizontalGap(const float gap);
        
        void setVolumeMontageVerticalGap(const float gap);
        
        bool isSurfaceMontageScaleProportionatelySelected() const;
        
        bool isVolumeMontageScaleProportionatelySelected() const;
        
        void setSurfaceMontageScaleProportionatelySelected(const bool selected);
        
        void setVolumeMontageScaleProportionatelySelected(const bool selected);
        
        bool isTabMarginLeftAllSelected() const;
        
        bool isTabMarginRightAllSelected() const;
        
        bool isTabMarginBottomAllSelected() const;
        
        bool isTabMarginTopAllSelected() const;
        
        void setTabMarginLeftAllSelected(const bool status);
        
        void setTabMarginRightAllSelected(const bool status);
        
        void setTabMarginBottomAllSelected(const bool status);
        
        void setTabMarginTopAllSelected(const bool status);
        
        void reset();

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        GapsAndMargins(const GapsAndMargins&);

        GapsAndMargins& operator=(const GapsAndMargins&);
        
        SceneClassAssistant* m_sceneAssistant;

        float m_tabMarginsLeft[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        float m_tabMarginsRight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_tabMarginsBottom[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_tabMarginsTop[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_tabMarginLeftAllSelected;
        
        bool m_tabMarginRightAllSelected;
        
        bool m_tabMarginBottomAllSelected;
        
        bool m_tabMarginTopAllSelected;
        
        float m_surfaceMontageGaps[2];
        
        float m_volumeMontageGaps[2];
        
        bool m_surfaceMontageScaleProportionatelySelected;
        
        bool m_volumeMontageScaleProportionatelySelected;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GAPS_AND_MARGINS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GAPS_AND_MARGINS_DECLARE__

} // namespace
#endif  //__GAPS_AND_MARGINS_H__
