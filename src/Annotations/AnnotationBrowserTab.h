#ifndef __ANNOTATION_BROWSER_TAB_H__
#define __ANNOTATION_BROWSER_TAB_H__

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

#include <array>

#include "AnnotationTwoDimensionalShape.h"
#include "CaretPointer.h"
#include "TileTabsLayoutBackgroundTypeEnum.h"

namespace caret {

    class BrowserTabContent;
    class TileTabsBrowserTabGeometry;
    
    class AnnotationBrowserTab : public AnnotationTwoDimensionalShape {
        
    public:
        AnnotationBrowserTab(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationBrowserTab();
        
        AnnotationBrowserTab(const AnnotationBrowserTab& obj);
        
        AnnotationBrowserTab& operator=(const AnnotationBrowserTab& obj);

        void setFromTileTabsGeometry(const TileTabsBrowserTabGeometry* geometry);
        
        void getTileTabsGeometry(TileTabsBrowserTabGeometry* geometryOut) const;

        void setBrowserTabContent(BrowserTabContent* browserTabContent,
                                  const int32_t tabIndex);
        
        BrowserTabContent* getBrowserTabContent();

        const BrowserTabContent* getBrowserTabContent() const;
        
        bool isBrowserTabDisplayed() const;
        
        void setBrowserTabDisplayed(const bool status);
        
        int32_t getTabIndex() const;
        
        TileTabsLayoutBackgroundTypeEnum::Enum getBackgroundType() const;
        
        void setBackgroundType(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType);
        
        virtual bool intersectionTest(const Annotation* other,
                                      const int32_t windowIndex) const override;

        void getBounds2D(float& minX,
                         float& maxX,
                         float& minY,
                         float& maxY) const;
        
        void setBounds2D(const float minX,
                         const float maxX,
                         const float minY,
                         const float maxY);
        
        static bool shrinkAndExpandToFillEmptySpace(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                                    const AnnotationBrowserTab* tabToExpand,
                                                    std::array<float, 4>& boundsOut);

        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationBrowserTab(const AnnotationBrowserTab& obj);
        
        void initializeMembersAnnotationBrowserTab();
        
        static bool expandTab(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                              const AnnotationBrowserTab* tabToExpand,
                              const std::array<float, 4>& boundsIn,
                              const bool boundsInValidFlag,
                              std::array<float, 4>& boundsOut);
        
        static bool shrinkTab(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                              const AnnotationBrowserTab* tabToShrink,
                              std::array<float, 4>& boundsOut);
        
        static bool shrinkTabAux(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                 const AnnotationBrowserTab* tabToShrink,
                                 const int32_t startingBounds[4],
                                 const bool testAllOnFlag,
                                 std::array<float, 4>& boundsOut);
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        /** Browser tab that owns this annotation DO NOT DELETE */
        BrowserTabContent* m_browserTabContent = NULL;
        
        /** Display status of browser tab */
        bool m_displayStatus = true;
        
        /** Index of browser tab that owns this annotation */
        int32_t m_tabIndex = -1;
        
        /** Type of background (opaque / transparent) for tab*/
        TileTabsLayoutBackgroundTypeEnum::Enum m_backgroundType = TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_BROWSER_TAB_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_BROWSER_TAB_DECLARE__

} // namespace
#endif  //__ANNOTATION_BROWSER_TAB_H__
