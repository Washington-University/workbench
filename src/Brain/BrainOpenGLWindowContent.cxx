
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

#define __BRAIN_OPEN_G_L_WINDOW_CONTENT_DECLARE__
#include "BrainOpenGLWindowContent.h"
#undef __BRAIN_OPEN_G_L_WINDOW_CONTENT_DECLARE__

#include "AnnotationBrowserTab.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLWindowContent 
 * \brief Content of window and its tabs.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLWindowContent::BrainOpenGLWindowContent()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLWindowContent::~BrainOpenGLWindowContent()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainOpenGLWindowContent::BrainOpenGLWindowContent(const BrainOpenGLWindowContent& obj)
: CaretObject(obj)
{
    this->copyHelperBrainOpenGLWindowContent(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BrainOpenGLWindowContent&
BrainOpenGLWindowContent::operator=(const BrainOpenGLWindowContent& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBrainOpenGLWindowContent(obj);
    }
    return *this;    
}

/**
 * Clear the content of the window and tabs.
 */
void
BrainOpenGLWindowContent::clear()
{
    m_tabViewports.clear();
    m_windowViewport.reset();
}

/**
 * Add a tab viewport.
 *
 * @param tabViewport
 *     Tab viewport to add.
 */
void
BrainOpenGLWindowContent::addTabViewport(BrainOpenGLViewportContent* tabViewport)
{
    std::unique_ptr<BrainOpenGLViewportContent> ptr(tabViewport);
    m_tabViewports.push_back(std::move(ptr));
}

/**
 * Set the window viewport.
 *
 * @param windowViewport
 *     The new window viewport.
 */
void
BrainOpenGLWindowContent::setWindowViewport(BrainOpenGLViewportContent* windowViewport)
{
    m_windowViewport.reset(windowViewport);
}

/**
 * @return Number of tab viewports.
 */
int32_t
BrainOpenGLWindowContent::getNumberOfTabViewports() const
{
    return m_tabViewports.size();
}

/**
 * Get the viewport content for the tab at the given index.
 *
 * @param index
 *     Index of element in viewports BUT IS NOT the TAB INDEX
 * @return
 *     Tab for the given index or NULL if not available.
 */
const BrainOpenGLViewportContent*
BrainOpenGLWindowContent::getTabViewportAtIndex(const int32_t index) const
{
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_tabViewports.size()))) {
        CaretAssertVectorIndex(m_tabViewports, index);
        return m_tabViewports[index].get();
    }
    
    return NULL;
}

/**
 * Get the tab viewport containing the given X/Y coordinates
 * after lock aspect ratio has been performed on the tab viewport.
 *
 * @param x
 *     The X-coordinate
 * @param y 
 *     The Y-coordinate
 * @return 
 *     Tab viewport containing the coordinate or NULL if not found.
 */
const BrainOpenGLViewportContent*
BrainOpenGLWindowContent::getTabViewportWithLockAspectXY(const int32_t x,
                                                         const int32_t y) const
{
    BrainOpenGLViewportContent* viewportContentOut(NULL);
    
    for (const auto& vp : m_tabViewports) {
        int32_t viewport[4];
        vp->getTabViewportBeforeApplyingMargins(viewport);
        if ((x >= viewport[0])
            && (x < (viewport[0] + viewport[2]))
            && (y >= viewport[1])
            && (y < (viewport[1] + viewport[3]))) {
            if (viewportContentOut == NULL) {
                viewportContentOut = vp.get();
            }
            else {
                /*
                 * Note: The tabs ONLY overlay in a manual tile tabs layout
                 */
                const BrowserTabContent* btc = vp.get()->getBrowserTabContent();
                const BrowserTabContent* btcOut = viewportContentOut->getBrowserTabContent();
                if ((btc != NULL)
                    && (btcOut != NULL)) {
                    if (btc->getManualLayoutBrowserTabAnnotation()->getStackingOrder()
                        > btcOut->getManualLayoutBrowserTabAnnotation()->getStackingOrder()) {
                        viewportContentOut = vp.get();
                    }
                }
            }
        }
    }
    
    return viewportContentOut;
}


/**
 * All viewports for all tabs.
 */
std::vector<const BrainOpenGLViewportContent*>
BrainOpenGLWindowContent::getAllTabViewports() const
{
    std::vector<const BrainOpenGLViewportContent*> allTabs;
    
    for (auto& vp : m_tabViewports) {
        allTabs.push_back(vp.get());
    }
    
    return allTabs;
}


/**
 * @return The viewport for the window (could be NULL).
 */
const BrainOpenGLViewportContent*
BrainOpenGLWindowContent::getWindowViewport() const
{
    return m_windowViewport.get();
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BrainOpenGLWindowContent::copyHelperBrainOpenGLWindowContent(const BrainOpenGLWindowContent& obj)
{
    clear();
    
    if (obj.m_windowViewport) {
        setWindowViewport(new BrainOpenGLViewportContent(*obj.m_windowViewport.get()));
    }
    
    for (const auto& tabvp : obj.m_tabViewports) {
        BrainOpenGLViewportContent* vp(NULL);
        const BrainOpenGLViewportContent* ptr = tabvp.get();
        if (ptr != NULL) {
            vp = new BrainOpenGLViewportContent(*ptr);
        }
        addTabViewport(vp);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLWindowContent::toString() const
{
    return "BrainOpenGLWindowContent";
}

