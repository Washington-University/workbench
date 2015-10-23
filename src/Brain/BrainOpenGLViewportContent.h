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
    
    class BrainOpenGLViewportContent : public CaretObject {
        
    public:
        ~BrainOpenGLViewportContent();
        
        BrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);
        
        BrainOpenGLViewportContent& operator=(const BrainOpenGLViewportContent& obj);
        
        void getModelViewport(int modelViewportOut[4]) const;
        
        void getTabViewport(int tabViewportOut[4]) const;
        
        void getWindowViewport(int windowViewportOut[4]) const;
        
        int getWindowIndex() const;
        
        BrowserTabContent* getBrowserTabContent() const;
        
        int32_t getTabIndex() const;
        
        bool isTabHighlighted() const;
        
        static void adjustViewportForAspectRatio(int viewport[4],
                                                 const float aspectRatio);
        
        static std::vector<BrainOpenGLViewportContent*> createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                         const int32_t windowIndex,
                                                                                         const int32_t windowViewport[4],
                                                                                         const std::vector<int32_t>& rowHeights,
                                                                                         const std::vector<int32_t>& columnWidths,
                                                                                         const int32_t highlightTabIndex,
                                                                                         const GapsAndMargins* gapsAndMargins);
        
        static BrainOpenGLViewportContent* createViewportForSingleTab(const int windowViewport[4],
                                                                      const int modelViewport[4],
                                                                      const int windowIndex,
                                                                      const bool highlightTabFlag,
                                                                      const GapsAndMargins* gapsAndMargins,
                                                                      BrowserTabContent* browserTabContent);
        
    private:
        BrainOpenGLViewportContent(const int32_t tileTabsRowIndex,
                                   const int32_t tileTabsColumnIndex,
                                   const int windowViewport[4],
                                   const int modelViewport[4],
                                   const int windowIndex,
                                   const bool highlightTabFlag,
                                   const GapsAndMargins* gapsAndMargins,
                                   BrowserTabContent* browserTabContent);
        
        void initializeMembersBrainOpenGLViewportContent();
        
        void copyHelperBrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);

        static void centerViewportsInWindow(std::vector<BrainOpenGLViewportContent*>& viewports,
                                            const std::vector<int32_t>& rowHeights,
                                            const std::vector<int32_t>& columnWidths);
        
        static void wrapWindowViewportAroundTabViewports(std::vector<BrainOpenGLViewportContent*>& viewports);
        
        static BrainOpenGLViewportContent* findViewportAtRowColumn(std::vector<BrainOpenGLViewportContent*>& viewports,
                                                                   const int32_t row,
                                                                   const int32_t column);
        
        const int32_t m_tileTabsRowIndex;
        
        const int32_t m_tileTabsColumnIndex;
        
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
