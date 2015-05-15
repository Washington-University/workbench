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

    class Brain;
    class BrowserTabContent;
    
    class BrainOpenGLViewportContent : public CaretObject {
        
    public:
        BrainOpenGLViewportContent(const int windowViewport[4],
                                   const int modelViewport[4],
                                   const bool highlightTabFlag,
                                   Brain* brain,
                                   BrowserTabContent* browserTabContent);
        
        ~BrainOpenGLViewportContent();
        
        void getModelViewport(int viewport[4]) const;
        
        const int* getModelViewport() const;
        
        const int* getWindowViewport() const;
        
        BrowserTabContent* getBrowserTabContent();
        
        Brain* getBrain();
        
        bool isTabHighlighted() const;
        
        static std::vector<BrainOpenGLViewportContent*> createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                         Brain* brain,
                                                                                         const int32_t windowWidth,
                                                                                         const int32_t windowHeight,
                                                                                         const std::vector<int32_t>& rowHeights,
                                                                                         const std::vector<int32_t>& columnWidths,
                                                                                         const int32_t highlightTabIndex);
        
    private:
        int m_modelViewport[4];
        
        int m_windowViewport[4];
        
        BrowserTabContent* m_browserTabContent;
        
        Brain* m_brain;
        
        bool m_highlightTab;
        
        BrainOpenGLViewportContent(const BrainOpenGLViewportContent&);

        BrainOpenGLViewportContent& operator=(const BrainOpenGLViewportContent&);
        
    public:
        virtual AString toString() const;
    };
    
#ifdef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
