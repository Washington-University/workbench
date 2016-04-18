#ifndef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
#define __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_

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

namespace caret {

    class BrowserTabContent;
    class GapsAndMargins;
    class TileTabsConfiguration;
    
    class BrainOpenGLViewportContent : public CaretObject {
        
    public:
        ~BrainOpenGLViewportContent();
        
        BrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);
        
        BrainOpenGLViewportContent& operator=(const BrainOpenGLViewportContent& obj);
        
        void getModelViewport(int modelViewportOut[4]) const;
        
        void getSurfaceMontageModelViewport(const int32_t montageX,
                                            const int32_t montageY,
                                            int subViewportOut[4]) const;
        
        void getTabViewportBeforeApplyingMargins(int tabViewportOut[4]) const;
        
        void getWindowViewport(int windowViewportOut[4]) const;
        
        int getWindowIndex() const;
        
        BrowserTabContent* getBrowserTabContent() const;
        
        int32_t getTabIndex() const;
        
        bool isTabHighlighted() const;
        
        static void adjustViewportForAspectRatio(int viewport[4],
                                                 const float aspectRatio);
        
        static void adjustWidthHeightForAspectRatio(const float aspectRatio,
                                                    int32_t& width,
                                                    int32_t& height);
        
        static std::vector<BrainOpenGLViewportContent*> createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                         TileTabsConfiguration* tileTabsConfiguration,
                                                                                         const GapsAndMargins* gapsAndMargins,
                                                                                         const int32_t windowIndex,
                                                                                         const int32_t windowViewport[4],
                                                                                         const int32_t highlightTabIndex);
        
        static BrainOpenGLViewportContent* createViewportForSingleTab(BrowserTabContent* browserTabContent,
                                                                      const GapsAndMargins* gapsAndMargins,
                                                                      const int32_t windowIndex,
                                                                      const int32_t windowViewport[4]);
        
    private:
        /**
         * Assists with creation of the tile tab viewports
         */
        class TileTabsViewportSizingInfo {
        public:
            TileTabsViewportSizingInfo(BrowserTabContent* browserTabContent,
                                       const int32_t rowIndexFromTop,
                                       const int32_t columnIndex,
                                       const float initialWidth,
                                       const float initialHeight);
            
            TileTabsViewportSizingInfo& operator=(const TileTabsViewportSizingInfo& obj);

            void print(const int32_t x,
                       const int32_t y);
            
            BrowserTabContent* m_browserTabContent;
            int32_t m_rowIndexFromTop;
            int32_t m_columnIndex;
            
            /** size with application of tile tabs configuration */
            float m_initialWidth;
            float m_initialHeight;
            
            /** size after application of lock aspect ratio */
            int32_t m_width;
            int32_t m_height;
        };
        
        BrainOpenGLViewportContent(const int windowViewport[4],
                                   const int tabViewport[4],
                                   const int modelViewport[4],
                                   const int windowIndex,
                                   const bool highlightTabFlag,
                                   BrowserTabContent* browserTabContent);
        
        void initializeMembersBrainOpenGLViewportContent();
        
        void copyHelperBrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);

        static void createModelViewport(const int tabViewport[4],
                                        const int32_t tabIndex,
                                        const GapsAndMargins* gapsAndMargins,
                                        int modelViewportOut[4]);
        
        const int m_windowIndex;
        
        const bool m_highlightTab;
        
        /** Tab viewport's X-coordinate */
        int m_tabX;
        /** Tab viewport's Y-coordinate */
        int m_tabY;
        /** Tab viewport's Width */
        int m_tabWidth;
        /** Tab viewport's Height */
        int m_tabHeight;
        
        /** Model viewport's X-coordinate */
        int m_modelX;
        /** Model viewport's Y-coordinate */
        int m_modelY;
        /** Model viewport's Width */
        int m_modelWidth;
        /** Model viewport's Height */
        int m_modelHeight;

        /** Window viewport's X-coordinate */
        int m_windowX;
        /** Window viewport's Y-coordinate */
        int m_windowY;
        /** Window viewport's Width */
        int m_windowWidth;
        /** Window viewport's Height */
        int m_windowHeight;
        
        BrowserTabContent* m_browserTabContent;
        
    public:
        virtual AString toString() const;
    };
    
#ifdef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
