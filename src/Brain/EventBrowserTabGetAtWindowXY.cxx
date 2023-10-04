
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

#define __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_DECLARE__
#include "EventBrowserTabGetAtWindowXY.h"
#undef __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DrawingViewportContent.h"
#include "EventDrawingViewportContentGet.h"
#include "EventTypeEnum.h"
#include "SamplesDrawingSettings.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabGetAtWindowXY 
 * \brief Get the BrowserTabContent at an XY location in a window
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param windowIndex
 *    Index of window
 * @param mouseXY
 *    Location of mouse
 */
EventBrowserTabGetAtWindowXY::EventBrowserTabGetAtWindowXY(const int32_t windowIndex,
                                                           const Vector3D& mouseXY)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_GET_AT_WINDOW_XY),
m_windowIndex(windowIndex),
m_mouseXY(mouseXY)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabGetAtWindowXY::~EventBrowserTabGetAtWindowXY()
{
}

/**
 * @return Index of window
 */
int32_t
EventBrowserTabGetAtWindowXY::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Location of mouse
 */
Vector3D
EventBrowserTabGetAtWindowXY::getMouseXY() const
{
    return m_mouseXY;
}

/**
 * @return BrainOpenGLViewportContent at the mouse
 */
const BrainOpenGLViewportContent*
EventBrowserTabGetAtWindowXY::getBrainOpenGLViewportContent() const
{
    return m_brainOpenGLViewportContent;
}

/**
 * @return BrowserTabContent at mouse location or NULL if not found
 */
BrowserTabContent*
EventBrowserTabGetAtWindowXY::getBrowserTabContent() const
{
    return m_browserTabContent;
}

/**
 * @return Drawing viewports for all volume montage slices in the tab
 */
std::vector<std::shared_ptr<DrawingViewportContent>>
EventBrowserTabGetAtWindowXY::getVolumeMontageViewportContent() const
{
    return m_volumeMontageViewportContent;
}

std::vector<std::shared_ptr<DrawingViewportContent>>
EventBrowserTabGetAtWindowXY::getSamplesDrawingVolumeMontageViewportContents() const
{
    const SamplesDrawingSettings* samplesDrawingSettings(m_browserTabContent->getSamplesDrawingSettings());
    std::vector<std::shared_ptr<DrawingViewportContent>> contentsOut;
    
    for (const auto& vp : m_volumeMontageViewportContent) {
        const DrawingViewportContentVolumeSlice& volumeSlice(vp->getVolumeSlice());
        if (samplesDrawingSettings->isSliceInLowerUpperOffsetRange(volumeSlice.getRowIndex(),
                                                                   volumeSlice.getColumnIndex())) {
            contentsOut.push_back(vp);
        }
    }
    return contentsOut;
}

/**
 * @return True if the window XY in a valid samples drawing slice.
 * @param windowXY
 *    The window XY
 */
bool
EventBrowserTabGetAtWindowXY::isWindowXyInSamplesDrawingVolumeSlice(const Vector3D& windowXY) const
{
    const SamplesDrawingSettings* samplesDrawingSettings(m_browserTabContent->getSamplesDrawingSettings());
    for (const auto& vp : m_volumeMontageViewportContent) {
        const DrawingViewportContentVolumeSlice& volumeSlice(vp->getVolumeSlice());
        if (samplesDrawingSettings->isSliceInLowerUpperOffsetRange(volumeSlice.getRowIndex(),
                                                                   volumeSlice.getColumnIndex())) {
            if (vp->containsWindowXY(windowXY)) {
                return true;
            }
        }
    }

    return false;
}

/**
 * @return If the slice containing the window coordinate is valid for samples drawing and the
 * range of slices valid for samples drawing is valid, the following is returned:
 * Element 0: The viewport containing the windowXY
 * Element 1: The viewport containing the first valid slice in the range
 * Element 2: The viewport containing the last valid slice in the range
 * If not valid, an empty vector is returned
 */
std::vector<std::shared_ptr<DrawingViewportContent>>
EventBrowserTabGetAtWindowXY::getSamplesDrawingViewportContents(const Vector3D& windowXY) const
{
    std::shared_ptr<DrawingViewportContent> drawingViewportContent;
    std::shared_ptr<DrawingViewportContent> firstViewportContent;
    std::shared_ptr<DrawingViewportContent> lastViewportContent;

    const SamplesDrawingSettings* samplesDrawingSettings(m_browserTabContent->getSamplesDrawingSettings());
    for (auto& vp : m_volumeMontageViewportContent) {
        const DrawingViewportContentVolumeSlice& volumeSlice(vp->getVolumeSlice());
        if (samplesDrawingSettings->isSliceInLowerUpperOffsetRange(volumeSlice.getRowIndex(),
                                                                   volumeSlice.getColumnIndex())) {
            if ( ! firstViewportContent) {
                firstViewportContent = vp;
            }
            lastViewportContent = vp;
            if (vp->containsWindowXY(windowXY)) {
                drawingViewportContent = vp;
            }
        }
    }

    std::vector<std::shared_ptr<DrawingViewportContent>> dvc;
    if ((firstViewportContent)
        && (lastViewportContent)
        && (drawingViewportContent)) {
        dvc.push_back(drawingViewportContent);
        dvc.push_back(firstViewportContent);
        dvc.push_back(lastViewportContent);
    }
    return dvc;
}

/**
 * Set the browser tab content
 * @param brainOpenGLViewportContent
 *    The brain opengl viewport content containing mouse
 * @param browserTabContent
 *    Browser tab content contining mousse
 */
void
EventBrowserTabGetAtWindowXY::setBrowserTabContent(const BrainOpenGLViewportContent* brainOpenGLViewportContent,
                                                   BrowserTabContent* browserTabContent)
{
    m_brainOpenGLViewportContent = brainOpenGLViewportContent;
    m_browserTabContent          = browserTabContent;
    
    if ((m_brainOpenGLViewportContent != NULL)
        && (m_browserTabContent != NULL)) {
        m_volumeMontageViewportContent = EventDrawingViewportContentGet::getVolumeMontageSlicesInTab(m_browserTabContent->getTabNumber());
    }
}
