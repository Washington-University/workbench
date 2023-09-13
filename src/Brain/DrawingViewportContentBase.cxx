
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

#define __DRAWING_VIEWPORT_CONTENT_BASE_DECLARE__
#include "DrawingViewportContentBase.h"
#undef __DRAWING_VIEWPORT_CONTENT_BASE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::DrawingViewportContentBase 
 * \brief Base class for drawing content of a particular viewport
 * \ingroup Brain
 */

/**
 * Constructor.
 * Note: Not all base classes suport both before and aspect locking graphics viewports
 *
 *@ param viewportContentType
 *    Type of viewport content
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content (Note: not valid for all subclasses)
 * @param beforeAspectLockedGraphicsViewport
 *    Viewport before aspect locking (not valid for all subclasses)
 * @param afterAspectLockedGraphicsViewport
 *    Viewport after aspect locking
 */
DrawingViewportContentBase::DrawingViewportContentBase(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                                                       const int32_t windowIndex,
                                                       const int32_t tabIndex,
                                                       const GraphicsViewport& beforeAspectLockedGraphicsViewport,
                                                       const GraphicsViewport& afterAspectLockedGraphicsViewport)
: CaretObject(),
m_viewportContentType(viewportContentType),
m_windowIndex(windowIndex),
m_tabIndex(tabIndex),
m_beforeAspectLockedGraphicsViewport(beforeAspectLockedGraphicsViewport),
m_afterAspectLockedGraphicsViewport(afterAspectLockedGraphicsViewport)
{
}

/**
 * Destructor.
 */
DrawingViewportContentBase::~DrawingViewportContentBase()
{
}

/**
 * Type of viewport content in this instance
 */
DrawingViewportContentTypeEnum::Enum
DrawingViewportContentBase::getViewportContentType() const
{
    return m_viewportContentType;
}

/**
 * @return Index of window containing this instance
 */
int32_t
DrawingViewportContentBase::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Index of tab containing this instance (may not be valid for all subclasses)
 */
int32_t
DrawingViewportContentBase::getTabIndexFromBase() const
{
    return m_tabIndex;
}

/**
 * @return The before aspect locking viewport (may not be valid for all subclasses)
 */
GraphicsViewport
DrawingViewportContentBase::getBeforeAspectLockedGraphicsViewportFromBase() const
{
    return m_beforeAspectLockedGraphicsViewport;
}

/**
 * @return The after aspect locking viewport
 */
GraphicsViewport
DrawingViewportContentBase::getAfterAspectLockedGraphicsViewportFromBase() const
{
    return m_afterAspectLockedGraphicsViewport;
}

/**
 * Add a child viewport to this viewport
 * @param drawingViewportContentBase
 *    child viewport to add
 */
void
DrawingViewportContentBase::addChildViewport(DrawingViewportContentBase* drawingViewportContentBase)
{
    const AString msg("Adding children to drawing viewport content type="
                      + DrawingViewportContentTypeEnum::toName(drawingViewportContentBase->getViewportContentType())
                      + " is not supported.");
    CaretLogSevere(msg);
    CaretAssertMessage(0, msg);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DrawingViewportContentBase::toString() const
{
    const AString contentTypeName(DrawingViewportContentTypeEnum::toName(m_viewportContentType));
    
    AString txt;
    txt.appendWithNewLine("m_viewportContentType="
                          + contentTypeName);
    txt.appendWithNewLine("    m_windowIndex="
                          + AString::number(m_windowIndex));
    txt.appendWithNewLine("    m_tabIndex="
                          + AString::number(m_tabIndex));
    txt.appendWithNewLine("    Before Lock="
                          + m_beforeAspectLockedGraphicsViewport.toString());
    txt.appendWithNewLine("    After Lock="
                          + m_afterAspectLockedGraphicsViewport.toString());
    return txt;
}

