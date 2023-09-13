
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __DRAWING_VIEWPORT_CONTENT_WINDOW_DECLARE__
#include "DrawingViewportContentWindow.h"
#undef __DRAWING_VIEWPORT_CONTENT_WINDOW_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::DrawingViewportContentWindow 
 * \brief Drawing content of a particular window
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param beforeAspectLockedGraphicsViewport
 *    Viewport before aspect locking
 * @param afterAspectLockedGraphicsViewport
 *    Viewport after aspect locking
 */
DrawingViewportContentWindow::DrawingViewportContentWindow(const int32_t windowIndex,
                                                           const GraphicsViewport& beforeAspectLockedGraphicsViewport,
                                                           const GraphicsViewport& afterAspectLockedGraphicsViewport)
: DrawingViewportContentBase(DrawingViewportContentTypeEnum::WINDOW,
                             windowIndex,
                             -1, /* Invalid tab index */
                             beforeAspectLockedGraphicsViewport,
                             afterAspectLockedGraphicsViewport)
{
}

/**
 * Destructor.
 */
DrawingViewportContentWindow::~DrawingViewportContentWindow()
{
}

/**
 * @return Tab Viewport Content for the given tab in this window (NULL if not found)
 */
DrawingViewportContentTab*
DrawingViewportContentWindow::getTabViewportForTabIndex(const int32_t tabIndex)
{
    for (auto& tv : m_tabViewports) {
        CaretAssert(tv);
        if (tv->getTabIndex() == tabIndex) {
            return tv.get();
        }
    }
    return NULL;
}

/**
 * Add a child viewport to this window
 * @param drawingViewportContentBase
 *    child viewport to add
 */
void
DrawingViewportContentWindow::addChildViewport(DrawingViewportContentBase* drawingViewportContentBase)
{
    CaretAssert(drawingViewportContentBase);
    if (getWindowIndex() == drawingViewportContentBase->getWindowIndex()) {
        switch (drawingViewportContentBase->getViewportContentType()) {
            case DrawingViewportContentTypeEnum::INVALID:
                CaretAssert(0);
                break;
            case DrawingViewportContentTypeEnum::MODEL:
            {
                DrawingViewportContentModel* modelViewport(drawingViewportContentBase->castToModel());
                CaretAssert(modelViewport);
                const int32_t tabIndex(modelViewport->getTabIndex());
                
                DrawingViewportContentTab* tabViewport(getTabViewportForTabIndex(tabIndex));
                if (tabViewport != NULL) {
                    tabViewport->addChildViewport(modelViewport);
                }
                else {
                    const AString msg("Trying to add model="
                                      + modelViewport->toString()
                                      + " to non-existent tab="
                                      + AString::number(tabIndex)
                                      + " in window="
                                      + AString::number(getWindowIndex()));
                    CaretLogSevere(msg);
                    CaretAssertMessage(0, msg);
                }
            }
                break;
            case DrawingViewportContentTypeEnum::TAB:
            {
                DrawingViewportContentTab* tabViewport(drawingViewportContentBase->castToTab());
                CaretAssert(tabViewport);
                std::unique_ptr<DrawingViewportContentTab> ptr(tabViewport);
                m_tabViewports.push_back(std::move(ptr));
            }
                break;
            case DrawingViewportContentTypeEnum::WINDOW:
            {
                const AString msg("Two windows with same index, adding window="
                                  + drawingViewportContentBase->toString()
                                  + " to window="
                                  + this->toString());
                CaretLogSevere(msg);
                CaretAssertMessage(0, msg);
            }
                break;
        }
    }
    else {
        const AString msg("Trying to add viewport with incorrect window index to window="
                          + AString::number(getWindowIndex())
                          + " child window="
                          + drawingViewportContentBase->toString());
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
            
//    std::unique_ptr<DrawingViewportContentTab> ptr(drawingViewportContentTab);
//    m_tabViewports.push_back(std::move(ptr));
}

/**
 * Get drawing viewport content at the mouse location
 * @param contentType
 *    Desired content type
 * @param mouseXY
 *    The mouse location
 * @return Desired content type at mouseXY or NULL if not found.
 *
 */
DrawingViewportContentBase*
DrawingViewportContentWindow::getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                     const Vector3D& mouseXY)
{
    DrawingViewportContentBase* drawingViewportContentOut(NULL);
    
    if (getBeforeAspectLockedGraphicsViewportFromBase().containsWindowXY(mouseXY)) {
        if (contentType == getViewportContentType()) {
            drawingViewportContentOut = this;
        }
        else {
            /*
             * NOTE: Tabs may overlap in a MANUAL tile tabs layout
             * BUT: The tabs are drawn from back to front and ordered that
             *      way in this window
             * SO: Use the result from the last tab that contains the mouse
             */
            for (auto& tv : m_tabViewports) {
                CaretAssert(tv);
                DrawingViewportContentBase* dvc(tv->getViewportTypeAtMouse(contentType,
                                                                           mouseXY));
                if (dvc != NULL) {
                    drawingViewportContentOut = dvc;
                }
            }
        }
    }
    
    return drawingViewportContentOut;
}

/**
 * @return The before lockingh aspect locking viewport.
 * Note: If aspect is not locking value returned is same as getAfterAspectLockedGraphicsViewport()
 */
GraphicsViewport
DrawingViewportContentWindow::getBeforeAspectLockedGraphicsViewport() const
{
    return getBeforeAspectLockedGraphicsViewportFromBase();
}

/**
 * @return The after locking aspect locking viewport
 * Note: If aspect is not locking value returned is same as getBeforeAspectLockedGraphicsViewport()
 */
GraphicsViewport
DrawingViewportContentWindow::getAfterAspectLockedGraphicsViewport() const
{
    return getAfterAspectLockedGraphicsViewportFromBase();
}
