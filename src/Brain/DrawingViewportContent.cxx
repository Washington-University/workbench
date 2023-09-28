
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

#define __DRAWING_VIEWPORT_CONTENT_DECLARE__
#include "DrawingViewportContent.h"
#undef __DRAWING_VIEWPORT_CONTENT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::DrawingViewportContent
 * \brief Info about a viewport in which a model is displayed
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param viewportContentType
 *    Type of viewport content
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content
 * @param graphicsViewport
 *    The viewport
 */
DrawingViewportContent::DrawingViewportContent(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                                               const int32_t windowIndex,
                                               const int32_t tabIndex,
                                               const GraphicsViewport& graphicsViewport)
: DrawingViewportContent(viewportContentType,
                         windowIndex,
                         tabIndex,
                         graphicsViewport,
                         ModelTypeEnum::MODEL_TYPE_INVALID,
                         DrawingViewportContentVolumeSlice(),
                         DrawingViewportContentSurface())
{
}

/**
 * Constructor.
 *
 * @param viewportContentType
 *    Type of viewport content
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content
 * @param graphicsViewport
 *    The viewport
 * @param modelType
 *    Type of model
 * @param volumeSlice
 *    Info about volume slice
 * @param surface
 *    Info about surface
 */
DrawingViewportContent::DrawingViewportContent(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                                               const int32_t windowIndex,
                                               const int32_t tabIndex,
                                               const GraphicsViewport& graphicsViewport,
                                               const ModelTypeEnum::Enum modelType,
                                               const DrawingViewportContentVolumeSlice& volumeSlice,
                                               const DrawingViewportContentSurface& surface)
: CaretObject(),
m_viewportContentType(viewportContentType),
m_windowIndex(windowIndex),
m_tabIndex(tabIndex),
m_graphicsViewport(graphicsViewport),
m_modelType(modelType),
m_volumeSlice(volumeSlice),
m_surface(surface)
{
}

/**
 * Constructor with invalid instance
 */
DrawingViewportContent::DrawingViewportContent()
: DrawingViewportContent(DrawingViewportContentTypeEnum::INVALID,
                         -1,
                         -1,
                         GraphicsViewport(),
                         ModelTypeEnum::MODEL_TYPE_INVALID,
                         DrawingViewportContentVolumeSlice(),
                         DrawingViewportContentSurface())
{
}

/**
 * Destructor.
 */
DrawingViewportContent::~DrawingViewportContent()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DrawingViewportContent::DrawingViewportContent(const DrawingViewportContent& obj)
: CaretObject(obj)
{
    this->copyHelperDrawingViewportContent(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
DrawingViewportContent&
DrawingViewportContent::operator=(const DrawingViewportContent& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDrawingViewportContent(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
DrawingViewportContent::copyHelperDrawingViewportContent(const DrawingViewportContent& obj)
{
    m_viewportContentType = obj.m_viewportContentType;
    m_windowIndex         = obj.m_windowIndex;
    m_tabIndex            = obj.m_tabIndex;
    m_graphicsViewport    = obj.m_graphicsViewport;
    m_modelType           = obj.m_modelType;
    m_volumeSlice         = obj.m_volumeSlice;
    m_surface             = obj.m_surface;
}

/**
 * Type of viewport content in this instance
 */
DrawingViewportContentTypeEnum::Enum
DrawingViewportContent::getViewportContentType() const
{
    return m_viewportContentType;
}

/**
 * @return Index of window
 */
int32_t
DrawingViewportContent::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Index of tab
 */
int32_t
DrawingViewportContent::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return The graphics viewport
 */
GraphicsViewport
DrawingViewportContent::getGraphicsViewport() const
{
    return m_graphicsViewport;
}

/**
 * @return True if the content contains the given window X/Y
 * @param windowXY
 *   The window XY
 */
bool
DrawingViewportContent::containsWindowXY(const Vector3D& windowXY) const
{
    if (m_viewportContentType != DrawingViewportContentTypeEnum::INVALID) {
        if (m_graphicsViewport.containsWindowXY(windowXY)) {
            return true;
        }
    }
    return false;
}

/**
 * @return String describing this object's content.
 */
AString 
DrawingViewportContent::toString() const
{
    const AString indentation("");
    return toStringWithIndentation(indentation);
}

/**
 * @return String describing this object's content with indentation
 */
AString
DrawingViewportContent::toStringWithIndentation(const AString& indentation) const
{
    const AString contentTypeName(DrawingViewportContentTypeEnum::toName(m_viewportContentType));
    
    bool showTabFlag(false);
    bool showModelTypeFlag(false);
    bool showSurfaceFlag(false);
    bool showVolumeSliceInfo(false);
    switch (m_viewportContentType) {
        case DrawingViewportContentTypeEnum::INVALID:
            break;
        case DrawingViewportContentTypeEnum::MODEL:
            showModelTypeFlag = true;
            showTabFlag = true;
            break;
        case DrawingViewportContentTypeEnum::MODEL_SURFACE_GRID:
            showTabFlag = true;
            break;
        case DrawingViewportContentTypeEnum::MODEL_SURFACE_GRID_CELL:
            showTabFlag = true;
            showSurfaceFlag = true;
            break;
        case DrawingViewportContentTypeEnum::MODEL_VOLUME_GRID:
            showTabFlag = true;
            break;
        case DrawingViewportContentTypeEnum::MODEL_VOLUME_SLICE:
            showTabFlag = true;
            showVolumeSliceInfo = true;
            break;
        case DrawingViewportContentTypeEnum::SPACER:
            break;
        case DrawingViewportContentTypeEnum::TAB_AFTER_ASPECT_LOCK:
            showTabFlag = true;
            break;
        case DrawingViewportContentTypeEnum::TAB_BEFORE_ASPECT_LOCK:
            showTabFlag = true;
            break;
        case DrawingViewportContentTypeEnum::WINDOW_AFTER_ASPECT_LOCK:
            break;
        case DrawingViewportContentTypeEnum::WINDOW_BEFORE_ASPECT_LOCK:
            break;
    }
    
    AString txt;
    txt.appendWithNewLine(indentation
                          + "m_viewportContentType="
                          + contentTypeName);
    txt.appendWithNewLine(indentation
                          + "  m_windowIndex="
                          + AString::number(m_windowIndex));
    if (showTabFlag) {
        txt.appendWithNewLine(indentation
                              + "  m_tabIndex="
                              + AString::number(m_tabIndex));
    }
    txt.appendWithNewLine(indentation
                          + "  GraphicsViewport="
                          + m_graphicsViewport.toString());
    if (showModelTypeFlag) {
        txt.appendWithNewLine(indentation
                              + "  Model="
                              + ModelTypeEnum::toName(m_modelType));
    }
    if (showSurfaceFlag) {
        txt.appendWithNewLine(indentation
                              + "  Grid Number of Rows="
                              + AString::number(m_surface.getNumberOfRows())
                              + ", Columns="
                              + AString::number(m_surface.getNumberOfColumns()));
        txt.appendWithNewLine(indentation
                              + "  Grid Row="
                              + AString::number(m_surface.getRowIndex())
                              + ", Columns="
                              + AString::number(m_surface.getColumnIndex()));
    }
    if (showVolumeSliceInfo) {
        txt.appendWithNewLine(indentation
                              + "  Grid Row="
                              + AString::number(m_volumeSlice.getRowIndex())
                              + ", Columns="
                              + AString::number(m_volumeSlice.getColumnIndex()));
        txt.appendWithNewLine(indentation
                              + "  Grid Number of Rows="
                              + AString::number(m_volumeSlice.getNumberOfRows())
                              + ", Columns="
                              + AString::number(m_volumeSlice.getNumberOfColumns()));
        txt.appendWithNewLine(indentation
                              + "  Slice View Plane="
                              + VolumeSliceViewPlaneEnum::toName(m_volumeSlice.getVolumeSliceViewPlane()));
        txt.appendWithNewLine(indentation
                              + "  Plane="
                              + m_volumeSlice.getPlane().toString());
        txt.appendWithNewLine(indentation
                              + "  Slice Coordinates="
                              + m_volumeSlice.getSliceXYZ().toString());
    }
    
    return txt;
}

