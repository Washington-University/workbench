
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

#define __DRAWING_VIEWPORT_CONTENT_TAB_DECLARE__
#include "DrawingViewportContentTab.h"
#undef __DRAWING_VIEWPORT_CONTENT_TAB_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;


    
/**
 * \class caret::DrawingViewportContentTab
 * \brief Drawing content of a particular window
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content
 * @param beforeAspectLockedGraphicsViewport
 *    Viewport before aspect locking
 * @param afterAspectLockedGraphicsViewport
 *    Viewport after aspect locking
 */
DrawingViewportContentTab::DrawingViewportContentTab(const int32_t windowIndex,
                                                     const int32_t tabIndex,
                                                     const GraphicsViewport& beforeAspectLockedGraphicsViewport,
                                                     const GraphicsViewport& afterAspectLockedGraphicsViewport)
: DrawingViewportContentBase(DrawingViewportContentTypeEnum::TAB,
                             windowIndex,
                             tabIndex,
                             beforeAspectLockedGraphicsViewport,
                             afterAspectLockedGraphicsViewport)
{
}

/**
 * Destructor.
 */
DrawingViewportContentTab::~DrawingViewportContentTab()
{
}

/**
 * @return The before lockingh aspect locking viewport.
 * Note: If aspect is not locking value returned is same as getAfterAspectLockedGraphicsViewport()
 */
GraphicsViewport
DrawingViewportContentTab::getBeforeAspectLockedGraphicsViewport() const
{
    return getBeforeAspectLockedGraphicsViewportFromBase();
}

/**
 * @return The after locking aspect locking viewport
 * Note: If aspect is not locking value returned is same as getBeforeAspectLockedGraphicsViewport()
 */
GraphicsViewport
DrawingViewportContentTab::getAfterAspectLockedGraphicsViewport() const
{
    return getAfterAspectLockedGraphicsViewportFromBase();
}

/**
 * Add a model viewport to this tab
 * @param drawingViewportContentModel
 *    Model viewport to add
 */
void
DrawingViewportContentTab::addChildViewport(DrawingViewportContentBase* drawingViewportContentBase)
{
    CaretAssert(drawingViewportContentBase);
    switch (drawingViewportContentBase->getViewportContentType()) {
        case DrawingViewportContentTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case DrawingViewportContentTypeEnum::MODEL:
        {
            DrawingViewportContentModel* modelViewport(drawingViewportContentBase->castToModel());
            CaretAssert(modelViewport);
            std::unique_ptr<DrawingViewportContentModel> ptr(modelViewport);
            m_modelViewports.push_back(std::move(ptr));
        }
            break;
        case DrawingViewportContentTypeEnum::TAB:
        {
            const AString msg("Cannot add tab viewport="
                              + drawingViewportContentBase->toString()
                              + " to model viewport="
                              + this->toString());
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
        }
            break;
        case DrawingViewportContentTypeEnum::WINDOW:
        {
            const AString msg("Cannot add window viewport="
                              + drawingViewportContentBase->toString()
                              + " to model viewport="
                              + this->toString());
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
        }
            break;
    }
}

/**
 * @return Tab index for this content
 */
int32_t
DrawingViewportContentTab::getTabIndex() const
{
    return getTabIndexFromBase();
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
DrawingViewportContentTab::getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                     const Vector3D& mouseXY)
{
    DrawingViewportContentBase* drawingViewportContentOut(NULL);
    
    if (getBeforeAspectLockedGraphicsViewport().containsWindowXY(mouseXY)) {
        if (contentType == DrawingViewportContentTypeEnum::TAB) {
            drawingViewportContentOut = this;
        }
        else if (contentType == DrawingViewportContentTypeEnum::MODEL) {
            for (auto& mv : m_modelViewports) {
                drawingViewportContentOut = mv->getViewportTypeAtMouse(contentType,
                                                                       mouseXY);
                if (drawingViewportContentOut != NULL) {
                    /*
                     * Model viewports within a tab should NEVER overlap
                     */
                    break;
                }
            }
        }
    }
    
    return drawingViewportContentOut;
}

