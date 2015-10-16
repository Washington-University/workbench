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
        
        float getMarginLeftForTab(const int32_t tabIndex) const;
        
        float getMarginRightForTab(const int32_t tabIndex) const;
        
        float getMarginBottomForTab(const int32_t tabIndex) const;
        
        float getMarginTopForTab(const int32_t tabIndex) const;
        
        void setMarginLeftForTab(const int32_t tabIndex,
                              const float margin);
        
        void setMarginRightForTab(const int32_t tabIndex,
                              const float margin);
        
        void setMarginBottomForTab(const int32_t tabIndex,
                              const float margin);
        
        void setMarginTopForTab(const int32_t tabIndex,
                              const float margin);
        
        float getSurfaceMontageHorizontalGapForWindow(const int32_t windowIndex) const;
        
        float getSurfaceMontageVerticalGapForWindow(const int32_t windowIndex) const;
        
        float getVolumeMontageHorizontalGapForWindow(const int32_t windowIndex) const;
        
        float getVolumeMontageVerticalGapForWindow(const int32_t windowIndex) const;
        
        void setSurfaceMontageHorizontalGapForWindow(const int32_t windowIndex,
                                                     const float gap);
        
        void setSurfaceMontageVerticalGapForWindow(const int32_t windowIndex,
                                                   const float gap);
        
        void setVolumeMontageHorizontalGapForWindow(const int32_t windowIndex,
                                                    const float gap);
        
        void setVolumeMontageVerticalGapForWindow(const int32_t windowIndex,
                                                  const float gap);
        
        void reset();

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        void getMarginsInPixelsForDrawing(const int32_t tabIndex,
                                          const int32_t viewportWidth,
                                          const int32_t viewportHeight,
                                          int32_t& leftMarginOut,
                                          int32_t& rightMarginOut,
                                          int32_t& bottomMarginOut,
                                          int32_t& topMarginOut) const;
        
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
        
        float m_surfaceMontageHorizontalGaps[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        float m_surfaceMontageVerticalGaps[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        float m_volumeMontageHorizontalGaps[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        float m_volumeMontageVerticalGaps[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GAPS_AND_MARGINS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GAPS_AND_MARGINS_DECLARE__

} // namespace
#endif  //__GAPS_AND_MARGINS_H__
