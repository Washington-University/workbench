/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <cassert>

#define __PALETTE_DEFINE__
#include "Palette.h"
#undef __PALETTE_DEFINE__

//#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"

using namespace caret;

/**
 * Constructor.
 *
 */
Palette::Palette()
    : CaretObject()
{
    this->initializeMembersPalette();
}

/**
 * Destructor
 */
Palette::~Palette()
{
    uint64_t num = this->paletteScalars.size();
    for (uint64_t i = 0; i < num; i++) {
        delete this->paletteScalars[i];
        this->paletteScalars[i] = NULL;
    }
    this->paletteScalars.clear();
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
Palette::Palette(const Palette& o)
    : CaretObject(o)
{
    this->initializeMembersPalette();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
Palette&
Palette::operator=(const Palette& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
Palette::copyHelper(const Palette& o)
{
    this->name = o.name;
    this->paletteScalars.clear();
    uint64_t num = o.paletteScalars.size();
    for (uint64_t i = 0; i < num; i++) {
        this->paletteScalars.push_back(new PaletteScalarAndColor(*o.paletteScalars[i]));
    }
}

void
Palette::initializeMembersPalette()
{
    this->modifiedFlag = false;
    this->name = "";
}
/**
 * Get string representation for debugging.
 * 
 * @return String containing info.
 *
 */
std::string
Palette::toString() const
{
    std::string s;
    
    s += "[name=" + this->name + ", ";
    uint64_t num = this->paletteScalars.size();
    for (uint64_t i = 0; i < num; i++) {
        if (i > 0) s += ",";
        s += this->paletteScalars[i]->toString();
    }    
    s += "]";
    
    return s;
}

/**
 * Get the name of the palette.
 * 
 * @return - name of palette.
 *
 */
std::string
Palette::getName() const
{
    return this->name;
}

/**
 * Set the name of this palette.
 * 
 * @param name - new value for name.
 *
 */
void
Palette::setName(const std::string& name)
{
    if (this->name != name) {
        this->name = name;
        this->setModified();
    }
}

/**
 * Get the number of scalars and colors.
 * 
 * @return - number of scalars and colors.
 *
 */
int32_t
Palette::getNumberOfScalarsAndColors() const
{
    return this->paletteScalars.size();
}

/**
 * Get a scalar and color for the specified index.
 * 
 * @param index - index of scalar and color.
 * @return  Reference to item at index or null if invalid index.
 *
 */
PaletteScalarAndColor*
Palette::getScalarAndColor(const int32_t index) const
{
    assert((index >= 0) && (index < this->getNumberOfScalarsAndColors()));
    return this->paletteScalars[index];
}

/**
 * Add a scalar and color to the palette.
 * 
 * @param scalar - scalar value.
 * @param colorName - color name.
 *
 */
void
Palette::addScalarAndColor(const float scalar,
                           const std::string& colorName)
{
    this->paletteScalars.push_back(new PaletteScalarAndColor(scalar, colorName));
    this->setModified();
}

/**
 * Insert a scalar/color pair.
 * 
 * @param psac - item to add.
 * @param insertAfterIndex - Insert after this index.
 *
 */
void
Palette::insertScalarAndColor(
                   const PaletteScalarAndColor& psac,
                   const int32_t insertAfterIndex)
{
    assert((insertAfterIndex >= 0) && (insertAfterIndex < this->getNumberOfScalarsAndColors()));
    this->paletteScalars.insert(this->paletteScalars.begin() + insertAfterIndex,
                                new PaletteScalarAndColor(psac));
    this->setModified();
}

/**
 * Remove the scalar and color at index.
 * 
 * @param index - index of scalar and color to remove.
 *
 */
void
Palette::removeScalarAndColor(const int32_t index)
{
    assert((index >= 0) && (index < this->getNumberOfScalarsAndColors()));
    this->paletteScalars.erase(this->paletteScalars.begin() + index);
    
    this->setModified();
}

/**
 *
 * Get the minimum and maximum scalar values in this palette.
 * 
 * @return two-dimensional float array with min and max values.
 *
 */
void
Palette::getMinMax(float& minOut, float& maxOut) const
{
    minOut = std::numeric_limits<float>::max();
    maxOut = -std::numeric_limits<float>::max();
    uint64_t num = this->paletteScalars.size();
    for (uint64_t i = 0; i < num; i++) {
        const float f = this->paletteScalars[i]->getScalar();
        if (f < minOut) minOut = f;
        if (f > maxOut) maxOut = f;
    }    
}

/**
 * Get the RGBA (4) colors in the range of zero to one.
 * 
 * @param scalar - scalar for which color is sought.
 * @param interpolateColorFlag - interpolate the color between scalars.
 * @return Array of 4 containing color components ranging zero to one.
 *
 */
void
Palette::getPaletteColor(
                   const float scalarIn,
                   const bool interpolateColorFlagIn,
                   float rgbaOut[4]) const
{
    rgbaOut[0] = 1.0f;
    rgbaOut[1] = 1.0f;
    rgbaOut[2] = 1.0f;
    rgbaOut[3] = 1.0f;
    
    bool interpolateColorFlag = interpolateColorFlagIn;
    
    float scalar = scalarIn;
    if (scalar < -1.0) scalar = -1.0;
    if (scalar >  1.0) scalar = 1.0;
    
    int numScalarColors = this->getNumberOfScalarsAndColors();
    if (numScalarColors > 0) {
        
        int paletteIndex = -1;
        if (numScalarColors == 1) {
            paletteIndex = 0;
            interpolateColorFlag = false;
        }
        else {
            if (scalar >= this->getScalarAndColor(0)->getScalar()) {
                paletteIndex = 0;
                interpolateColorFlag = false;
            }
            else if (scalar <=
                     this->getScalarAndColor(numScalarColors - 1)->getScalar()) {
                paletteIndex = numScalarColors - 1;
                interpolateColorFlag = false;
            }
            else {
                for (int i = 1; i < numScalarColors; i++) {
                    const PaletteScalarAndColor* psac = this->getScalarAndColor(i);
                    if (scalar > psac->getScalar()) {
                        paletteIndex = i - 1;
                        break;
                    }
                }
                
                //
                // Always interpolate if there are only two colors
                //
                if (numScalarColors == 2) {
                    interpolateColorFlag = true;
                }
            }
        }
        if (paletteIndex >= 0) {
            const PaletteScalarAndColor* psac = this->getScalarAndColor(paletteIndex);
            psac->getColor(rgbaOut);
            if (interpolateColorFlag &&
                (paletteIndex < (numScalarColors - 1))) {
                const PaletteScalarAndColor* psacBelow =
                    this->getScalarAndColor(paletteIndex + 1);
                float totalDiff = psac->getScalar() - psacBelow->getScalar();
                if (totalDiff != 0.0) {
                    float offset = scalar - psacBelow->getScalar();
                    float percentAbove = offset / totalDiff;
                    float percentBelow = 1.0f - percentAbove;
                    if (psacBelow->getColorName() != Palette::NONE_COLOR_NAME) {
                        float rgbaAbove[4];
                        psac->getColor(rgbaAbove);
                        float rgbaBelow[4];
                        psacBelow->getColor(rgbaBelow);
                        rgbaOut[0] = (percentAbove * rgbaAbove[0]
                                      + percentBelow * rgbaBelow[0]);
                        rgbaOut[1] = (percentAbove * rgbaAbove[1]
                                      + percentBelow * rgbaBelow[1]);
                        rgbaOut[2] = (percentAbove * rgbaAbove[2]
                                      + percentBelow * rgbaBelow[2]);
                    }
                }
            }
            else if (psac->getColorName() == Palette::NONE_COLOR_NAME) {
                rgbaOut[3] = 0.0f;
            }
        }
    }
}

/**
 * Set this object has been modified.
 *
 */
void
Palette::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
Palette::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
Palette::isModified() const
{
    return this->modifiedFlag;
}

