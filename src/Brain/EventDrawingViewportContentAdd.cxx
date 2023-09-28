
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

#define __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__
#include "EventDrawingViewportContentAdd.h"
#undef __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDrawingViewportContentAdd 
 * \brief Event to associate a drawing viewport with a window
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventDrawingViewportContentAdd::EventDrawingViewportContentAdd()
: Event(EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_ADD)
{
    
}

/**
 * Destructor.
 */
EventDrawingViewportContentAdd::~EventDrawingViewportContentAdd()
{
}

/**
 * @return Number of drawing viewport content
 */
int32_t
EventDrawingViewportContentAdd::getNumberOfDrawingViewportContent() const
{
    return m_drawingViewportContents.size();
}

/**
 * @return Take the item at the given index
 * @param index
 * Note: Do not call this method more than once since caller is "taking" the item.  Since
 * the item is taken, the unique_ptr becomes empty and thus this method cannot be 'const'
 */
std::unique_ptr<DrawingViewportContent>
EventDrawingViewportContentAdd::takeDrawingViewportContent(const int32_t index) 
{
    CaretAssertVectorIndex(m_drawingViewportContents, index);
    return std::move(m_drawingViewportContents[index]);
}

/**
 * Add a window before aspect lock
 * @param windowIndex
 *    Index of window
 * @param viewport
 *    The viewport
 */
void
EventDrawingViewportContentAdd::addWindowBeforeLock(const int32_t windowIndex,
                                                    const GraphicsViewport& viewport)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::WINDOW_BEFORE_ASPECT_LOCK,
                                                                           windowIndex,
                                                                           -1,
                                                                           viewport));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a window after aspect lock
 * @param windowIndex
 *    Index of window
 * @param viewport
 *    The viewport
 */
void
EventDrawingViewportContentAdd::addWindowAfterLock(const int32_t windowIndex,
                                                   const GraphicsViewport& viewport)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::WINDOW_AFTER_ASPECT_LOCK,
                                                                           windowIndex,
                                                                           -1,
                                                                           viewport));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a tab before aspect lock
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 */
void
EventDrawingViewportContentAdd::addTabBeforeLock(const int32_t windowIndex,
                                                 const int32_t tabIndex,
                                                 const GraphicsViewport& viewport)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::TAB_BEFORE_ASPECT_LOCK,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a tab after aspect lock
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 */
void
EventDrawingViewportContentAdd::addTabAfterLock(const int32_t windowIndex,
                                                const int32_t tabIndex,
                                                const GraphicsViewport& viewport)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::TAB_AFTER_ASPECT_LOCK,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a model
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 * @param modelType
 *    Type of model
 */
void
EventDrawingViewportContentAdd::addModel(const int32_t windowIndex,
                                         const int32_t tabIndex,
                                         const GraphicsViewport& viewport,
                                         const ModelTypeEnum::Enum modelType)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::MODEL,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport,
                                                                           modelType,
                                                                           DrawingViewportContentVolumeSlice(),
                                                                           DrawingViewportContentSurface()));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a surface grid
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 * @param numberOfGridRows
 *    Number of rows in grid
 * @param numberOfGridRows
 *    Number of columns in grid
 */
void
EventDrawingViewportContentAdd::addModelSurfaceGrid(const int32_t windowIndex,
                                                    const int32_t tabIndex,
                                                    const GraphicsViewport& viewport,
                                                    const int32_t numberOfGridRows,
                                                    const int32_t numberOfGridColumns)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::MODEL_SURFACE_GRID,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport,
                                                                           ModelTypeEnum::MODEL_TYPE_INVALID,
                                                                           DrawingViewportContentVolumeSlice(),
                                                                           DrawingViewportContentSurface(numberOfGridRows,
                                                                                                         numberOfGridColumns,
                                                                                                         -1,
                                                                                                         -1)));

    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a cell in the surface grid
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 * @param numberOfRows
 *    Cell of row in grid
 * @param numberOfColumns
 *    Cell of column in grid
 * @param gridRow
 *    Column in grid
 * @param gridColumn
 *    Column in grid
 */
void
EventDrawingViewportContentAdd::addModelSurfaceGridCell(const int32_t windowIndex,
                                                        const int32_t tabIndex,
                                                        const GraphicsViewport& viewport,
                                                        const int32_t numberOfGridRows,
                                                        const int32_t numberOfGridColumns,
                                                        const int32_t gridRow,
                                                        const int32_t gridColumn)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::MODEL_SURFACE_GRID_CELL,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport,
                                                                           ModelTypeEnum::MODEL_TYPE_INVALID,
                                                                           DrawingViewportContentVolumeSlice(),
                                                                           DrawingViewportContentSurface(numberOfGridRows,
                                                                                                         numberOfGridColumns,
                                                                                                         gridRow,
                                                                                                         gridColumn)));
    
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a volume grid
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 * @param numberOfGridRows
 *    Number of rows in grid
 * @param numberOfGridColumns
 *    Number of columns in grid
 */
void
EventDrawingViewportContentAdd::addModelVolumeGrid(const int32_t windowIndex,
                                                   const int32_t tabIndex,
                                                   const GraphicsViewport& viewport,
                                                   const int32_t numberOfGridRows,
                                                   const int32_t numberOfGridColumns)
{
    const int32_t invalidGridRow(-1);
    const int32_t invalidGridColumn(-1);
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::MODEL_VOLUME_GRID,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport,
                                                                           ModelTypeEnum::MODEL_TYPE_INVALID,
                                                                           DrawingViewportContentVolumeSlice(numberOfGridRows,
                                                                                                             numberOfGridColumns,
                                                                                                             -1,
                                                                                                             -1,
                                                                                                             VolumeSliceViewPlaneEnum::ALL,
                                                                                                             Plane(),
                                                                                                             Vector3D()),
                                                                           DrawingViewportContentSurface()));
    m_drawingViewportContents.push_back(std::move(ptr));
}

/**
 * Add a volume slice that may or may not be in a montage
 * @param windowIndex
 *    Index of window
 * @param tabIndex
 *    Index of tab
 * @param viewport
 *    The viewport
 * @param volumeSliceInfo
 *    Info about the volume slice
 */
void
EventDrawingViewportContentAdd::addVolumeSlice(const int32_t windowIndex,
                                               const int32_t tabIndex,
                                               const GraphicsViewport& viewport,
                                               const DrawingViewportContentVolumeSlice& volumeSliceInfo)
{
    std::unique_ptr<DrawingViewportContent> ptr(new DrawingViewportContent(DrawingViewportContentTypeEnum::MODEL_VOLUME_SLICE,
                                                                           windowIndex,
                                                                           tabIndex,
                                                                           viewport,
                                                                           ModelTypeEnum::MODEL_TYPE_INVALID,
                                                                           volumeSliceInfo,
                                                                           DrawingViewportContentSurface()));
    m_drawingViewportContents.push_back(std::move(ptr));
}

