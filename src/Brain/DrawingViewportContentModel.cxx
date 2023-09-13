
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

#define __DRAWING_VIEWPORT_CONTENT_MODEL_DECLARE__
#include "DrawingViewportContentModel.h"
#undef __DRAWING_VIEWPORT_CONTENT_MODEL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DrawingViewportContentModel
 * \brief Drawing content of a particular window
 * \ingroup Brain
 */

/**
 * Constructor for a model in NOT a montage row or column
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content
 * @param graphicsViewport
 *    Viewport
 * @param modelType
 */
DrawingViewportContentModel::DrawingViewportContentModel(const int32_t windowIndex,
                                                         const int32_t tabIndex,
                                                         const GraphicsViewport& graphicsViewport,
                                                         const ModelTypeEnum::Enum modelType)
: DrawingViewportContentModel(windowIndex,
                              tabIndex,
                              graphicsViewport,
                              modelType,
                              -1,  /* Invalid montage row index */
                              -1)  /* Invalid montage column index */
{
}

/**
 * Constructor for a model in a montage row or column
 * @param windowIndex
 *    Index of window containing this viewport content
 * @param tabIndex
 *    Index of tab containing this viewport content
 * @param graphicsViewport
 *    Viewport
 * @param modelType
 *    Type of model in viewport
 * @param montageRowIndex
 *    Index of montage row
 * @param montageColumnIndex
 *    Index of montage column
 */
DrawingViewportContentModel::DrawingViewportContentModel(const int32_t windowIndex,
                                                         const int32_t tabIndex,
                                                         const GraphicsViewport& graphicsViewport,
                                                         const ModelTypeEnum::Enum modelType,
                                                         const int32_t montageRowIndex,
                                                         const int32_t montageColumnIndex)
: DrawingViewportContentBase(DrawingViewportContentTypeEnum::MODEL,
                             windowIndex,
                             tabIndex,
                             GraphicsViewport(), /* before locking viewport is invalid*/
                             graphicsViewport),   /* Use after locking viewport in base */
m_modelType(modelType),
m_montageRowIndex(montageRowIndex),
m_montageColumnIndex(montageColumnIndex)
{
}

/**
 * Destructor.
 */
DrawingViewportContentModel::~DrawingViewportContentModel()
{
}

/**
 * @return The  viewport.
 */
GraphicsViewport
DrawingViewportContentModel::getGraphicsViewport() const
{
    return getAfterAspectLockedGraphicsViewportFromBase();
}

/**
 * @return Tab index for this content
 */
int32_t
DrawingViewportContentModel::getTabIndex() const
{
    return getTabIndexFromBase();
}

/**
 * @return Type of model in the viewport
 */
ModelTypeEnum::Enum
DrawingViewportContentModel::getModelType() const
{
    return m_modelType;
}

/**
 * @return Row index in montage (negative if not in montage)
 */
int32_t
DrawingViewportContentModel::getMontageRowIndex() const
{
    return m_montageRowIndex;
}

/**
 * @return Column index in montage (negative if not in montage)
 */
int32_t
DrawingViewportContentModel::getMontageColumnIndex() const
{
    return m_montageColumnIndex;
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
DrawingViewportContentModel::getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                    const Vector3D& mouseXY)
{
    DrawingViewportContentBase* drawingViewportContentOut(NULL);
    
    if (getGraphicsViewport().containsWindowXY(mouseXY)) {
        if (contentType == getViewportContentType()) {
            drawingViewportContentOut = this;
        }
    }
    
    return drawingViewportContentOut;
}
