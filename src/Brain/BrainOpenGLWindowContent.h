#ifndef __BRAIN_OPEN_G_L_WINDOW_CONTENT_H__
#define __BRAIN_OPEN_G_L_WINDOW_CONTENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "BrainOpenGLViewportContent.h"
#include "CaretObject.h"



namespace caret {
    
    class BrainOpenGLWindowContent : public CaretObject {
        
    public:
        BrainOpenGLWindowContent();
        
        virtual ~BrainOpenGLWindowContent();
        
        BrainOpenGLWindowContent(const BrainOpenGLWindowContent& obj);

        BrainOpenGLWindowContent& operator=(const BrainOpenGLWindowContent& obj);
        
        void clear();
        
        void addTabViewport(BrainOpenGLViewportContent* tabsViewportContent);
        
        void setWindowViewport(BrainOpenGLViewportContent* windowViewportContent);
        
        int32_t getNumberOfTabViewports() const;
        
        const BrainOpenGLViewportContent* getTabViewportAtIndex(const int32_t index) const;
        
        const BrainOpenGLViewportContent* getTabViewportWithLockAspectXY(const int32_t x,
                                                                         const int32_t y) const;
        
        const BrainOpenGLViewportContent* getWindowViewport() const;
        
        std::vector<const BrainOpenGLViewportContent*> getAllTabViewports() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperBrainOpenGLWindowContent(const BrainOpenGLWindowContent& obj);

        std::vector<std::unique_ptr<BrainOpenGLViewportContent>> m_tabViewports;
        
        std::unique_ptr<BrainOpenGLViewportContent> m_windowViewport;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_WINDOW_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_WINDOW_CONTENT_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_WINDOW_CONTENT_H__
