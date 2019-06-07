
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __BACKGROUND_AND_FOREGROUND_COLORS_DECLARE__
#include "BackgroundAndForegroundColors.h"
#undef __BACKGROUND_AND_FOREGROUND_COLORS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BackgroundAndForegroundColors 
 * \brief Contains background and foreground colors used in graphics drawing.
 * \ingroup Common
 */

/**
 * Constructor.
 */
BackgroundAndForegroundColors::BackgroundAndForegroundColors()
: CaretObject()
{
    reset();
}

/**
 * Destructor.
 */
BackgroundAndForegroundColors::~BackgroundAndForegroundColors()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BackgroundAndForegroundColors::BackgroundAndForegroundColors(const BackgroundAndForegroundColors& obj)
: CaretObject(obj)
{
    copyHelperBackgroundAndForegroundColors(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BackgroundAndForegroundColors&
BackgroundAndForegroundColors::operator=(const BackgroundAndForegroundColors& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        copyHelperBackgroundAndForegroundColors(obj);
    }
    return *this;    
}

/**
 * Equality operator
 *
 * @param obj
 *    Compare 'this' to 'obj' for equality
 * @return
 *    True if 'this' is equal to 'obj', else false.
 */
bool
BackgroundAndForegroundColors::operator==(const BackgroundAndForegroundColors& obj) const
{
    bool equalFlag(true);
    
    for (int32_t i = 0; i < 3; i++) {
        if (m_colorForegroundWindow[i]        != obj.m_colorForegroundWindow[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorBackgroundWindow[i]        !=obj.m_colorBackgroundWindow[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorForegroundAll[i]           != obj.m_colorForegroundAll[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorBackgroundAll[i]           !=obj.m_colorBackgroundAll[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorForegroundChart[i]         != obj.m_colorForegroundChart[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorBackgroundChart[i]         != obj.m_colorBackgroundChart[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorForegroundSurface[i]       != obj.m_colorForegroundSurface[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorBackgroundSurface[i]       != obj.m_colorBackgroundSurface[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorForegroundVolume[i]        != obj.m_colorForegroundVolume[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorBackgroundVolume[i]        != obj.m_colorBackgroundVolume[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorChartMatrixGridLines[i]    != obj.m_colorChartMatrixGridLines[i]) {
            equalFlag = false;
            break;
        }
        if (m_colorChartHistogramThreshold[i] != obj.m_colorChartHistogramThreshold[i])  {
            equalFlag = false;
            break;
        }
    }
    
    return equalFlag;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BackgroundAndForegroundColors::copyHelperBackgroundAndForegroundColors(const BackgroundAndForegroundColors& obj)
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundWindow[i]        = obj.m_colorForegroundWindow[i];
        m_colorBackgroundWindow[i]        = obj.m_colorBackgroundWindow[i];
        m_colorForegroundAll[i]           = obj.m_colorForegroundAll[i];
        m_colorBackgroundAll[i]           = obj.m_colorBackgroundAll[i];
        m_colorForegroundChart[i]         = obj.m_colorForegroundChart[i];
        m_colorBackgroundChart[i]         = obj.m_colorBackgroundChart[i];
        m_colorForegroundSurface[i]       = obj.m_colorForegroundSurface[i];
        m_colorBackgroundSurface[i]       = obj.m_colorBackgroundSurface[i];
        m_colorForegroundVolume[i]        = obj.m_colorForegroundVolume[i];
        m_colorBackgroundVolume[i]        = obj.m_colorBackgroundVolume[i];
        m_colorChartMatrixGridLines[i]    = obj.m_colorChartMatrixGridLines[i];
        m_colorChartHistogramThreshold[i] = obj.m_colorChartHistogramThreshold[i];
    }
}

/**
 * Reset the colors to the default values.
 */
void
BackgroundAndForegroundColors::reset()
{
    const uint8_t backRed   = 0;
    const uint8_t backGreen = 0;
    const uint8_t backBlue  = 0;
    
    const uint8_t foreRed   = 255;
    const uint8_t foreGreen = 255;
    const uint8_t foreBlue  = 255;
    
    setColor(m_colorForegroundWindow, foreRed, foreGreen, foreBlue);
    
    setColor(m_colorBackgroundWindow, backRed, backGreen, backBlue);
    
    setColor(m_colorForegroundAll, foreRed, foreGreen, foreBlue);
    
    setColor(m_colorBackgroundAll, backRed, backGreen, backBlue);
    
    setColor(m_colorForegroundChart, foreRed, foreGreen, foreBlue);
    
    setColor(m_colorBackgroundChart, backRed, backGreen, backBlue);
    
    setColor(m_colorForegroundSurface, foreRed, foreGreen, foreBlue);
    
    setColor(m_colorBackgroundSurface, backRed, backGreen, backBlue);
    
    setColor(m_colorForegroundVolume, foreRed, foreGreen, foreBlue);
    
    setColor(m_colorBackgroundVolume, backRed, backGreen, backBlue);
    
    const uint8_t chartRed   = 255;
    const uint8_t chartGreen = 255;
    const uint8_t chartBlue  = 255;
    setColor(m_colorChartMatrixGridLines, chartRed, chartGreen, chartBlue);
    
    const uint8_t threshRed   = 100;
    const uint8_t threshGreen = 100;
    const uint8_t threshBlue  = 255;
    setColor(m_colorChartHistogramThreshold, threshRed, threshGreen, threshBlue);
}

/**
 * Get the foreground color for the window.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorForegroundWindow(uint8_t colorForeground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorForeground[i] = m_colorForegroundWindow[i];
    }
}

/**
 * Set the foreground color for the window.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorForegroundWindow(const uint8_t colorForeground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundWindow[i] = colorForeground[i];
    }
}

/**
 * Get the background color for the window.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorBackgroundWindow(uint8_t colorBackground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorBackground[i] = m_colorBackgroundWindow[i];
    }
}

/**
 * Set the background color for the window.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorBackgroundWindow(const uint8_t colorBackground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorBackgroundWindow[i] = colorBackground[i];
    }
}



/**
 * Get the foreground color for viewing the ALL model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorForegroundAllView(uint8_t colorForeground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorForeground[i] = m_colorForegroundAll[i];
    }
}

/**
 * Set the foreground color for viewing the ALL model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorForegroundAllView(const uint8_t colorForeground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundAll[i] = colorForeground[i];
    }
}

/**
 * Get the background color for viewing the ALL model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorBackgroundAllView(uint8_t colorBackground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorBackground[i] = m_colorBackgroundAll[i];
    }
}

/**
 * Set the background color for viewing the ALL model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorBackgroundAllView(const uint8_t colorBackground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorBackgroundAll[i] = colorBackground[i];
    }
}


/**
 * Get the foreground color for viewing the CHART model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorForegroundChartView(uint8_t colorForeground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorForeground[i] = m_colorForegroundChart[i];
    }
}

/**
 * Set the foreground color for viewing the CHART model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorForegroundChartView(const uint8_t colorForeground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundChart[i] = colorForeground[i];
    }
}

/**
 * Get the background color for viewing the CHART model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorBackgroundChartView(uint8_t colorBackground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorBackground[i] = m_colorBackgroundChart[i];
    }
}

/**
 * Set the background color for viewing the CHART model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorBackgroundChartView(const uint8_t colorBackground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorBackgroundChart[i] = colorBackground[i];
    }
}


/**
 * Get the foreground color for viewing the SURFACE model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorForegroundSurfaceView(uint8_t colorForeground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorForeground[i] = m_colorForegroundSurface[i];
    }
}

/**
 * Set the foreground color for viewing the SURFACE model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorForegroundSurfaceView(const uint8_t colorForeground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundSurface[i] = colorForeground[i];
    }
}

/**
 * Get the background color for viewing the SURFACE model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorBackgroundSurfaceView(uint8_t colorBackground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorBackground[i] = m_colorBackgroundSurface[i];
    }
}

/**
 * Get the background color for viewing the SURFACE model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorBackgroundSurfaceView(const uint8_t colorBackground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorBackgroundSurface[i] = colorBackground[i];
    }
}

/**
 * Get the foreground color for viewing the VOLUME model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorForegroundVolumeView(uint8_t colorForeground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorForeground[i] = m_colorForegroundVolume[i];
    }
}

/**
 * Set the foreground color for viewing the VOLUME model.
 *
 * @param colorForeground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorForegroundVolumeView(const uint8_t colorForeground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorForegroundVolume[i] = colorForeground[i];
    }
}

/**
 * Get the background color for viewing the VOLUME model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorBackgroundVolumeView(uint8_t colorBackground[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorBackground[i] = m_colorBackgroundVolume[i];
    }
}

/**
 * Set the background color for viewing the VOLUME model.
 *
 * @param colorBackground
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorBackgroundVolumeView(const uint8_t colorBackground[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorBackgroundVolume[i] = colorBackground[i];
    }
}

/**
 * Get the color for chart matrix grid lines
 *
 * @param colorChartMatrixGridLines
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorChartMatrixGridLines(uint8_t colorChartMatrixGridLines[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorChartMatrixGridLines[i] = m_colorChartMatrixGridLines[i];
    }
}

/**
 * Set the color for chart matrix grid lines
 *
 * @param colorChartMatrixGridLines
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorChartMatrixGridLines(const uint8_t colorChartMatrixGridLines[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorChartMatrixGridLines[i] = colorChartMatrixGridLines[i];
    }
}

/**
 * Get the color for chart histogram threshold
 *
 * @param colorChartHistogramThreshold
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::getColorChartHistogramThreshold(uint8_t colorChartHistogramThreshold[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        colorChartHistogramThreshold[i] = m_colorChartHistogramThreshold[i];
    }
}

/**
 * Set the color for chart histogram threshold
 *
 * @param colorChartHistogramThreshold
 *    RGB color components ranging [0, 255].
 */
void
BackgroundAndForegroundColors::setColorChartHistogramThreshold(const uint8_t colorChartHistogramThreshold[3])
{
    for (int32_t i = 0; i < 3; i++) {
        m_colorChartHistogramThreshold[i] = colorChartHistogramThreshold[i];
    }
}


/**
 * Set a color with the given color components that range 0 to 255.
 *
 * @param color
 *     Color that is set.
 * @param red
 *     Red component.
 * @param green
 *     Green component.
 * @param blue
 *     Blue component.
 */
void
BackgroundAndForegroundColors::setColor(uint8_t color[3],
                                        const uint8_t red,
                                        const uint8_t green,
                                        const uint8_t blue)
{
    color[0] = red;
    color[1] = green;
    color[2] = blue;
}

