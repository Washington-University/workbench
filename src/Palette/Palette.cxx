/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <deque>
#include <limits>

#include "CaretAssert.h"
#define __PALETTE_DEFINE__
#include "Palette.h"
#undef __PALETTE_DEFINE__

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
    : CaretObject(o), TracksModificationInterface()
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
AString
Palette::toString() const
{
    AString s;
    
    s += "name=" + this->name + "\n";
    uint64_t num = this->paletteScalars.size();
    for (uint64_t i = 0; i < num; i++) {
        s += ("    "
              + this->paletteScalars[i]->toString()
              + "\n");
    }    
    s += "\n";
    
    return s;
}

/**
 * Get the name of the palette.
 * 
 * @return - name of palette.
 *
 */
AString
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
Palette::setName(const AString& name)
{
    if (this->name != name) {
        this->name = name;
        this->setModified();
    }
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
                           const AString& colorName)
{
    CaretAssert(paletteScalars.size() == 0 || scalar <= paletteScalars.back()->getScalar());//die in debug if a palette is constructed incorrectly
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
    CaretAssertVectorIndex(this->paletteScalars, insertAfterIndex);
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
Palette::removeScalarAndColor(const int32_t indx)
{
    CaretAssertVectorIndex(this->paletteScalars, indx);
    this->paletteScalars.erase(this->paletteScalars.begin() + indx);
    
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

///**
// * Get the RGBA (4) colors in the range of zero to one.
// * 
// * @param scalar - scalar for which color is sought.
// * @param interpolateColorFlag - interpolate the color between scalars.
// * @return Array of 4 containing color components ranging zero to one.
// *
// */
//void
//Palette::getPaletteColor(
//                   const float scalarIn,
//                   const bool interpolateColorFlagIn,
//                   float rgbaOut[4]) const
//{
//    rgbaOut[0] = 0.0f;
//    rgbaOut[1] = 0.0f;
//    rgbaOut[2] = 0.0f;
//    rgbaOut[3] = 1.0f;
//    
//    bool interpolateColorFlag = interpolateColorFlagIn;
//    
//    float scalar = scalarIn;
//    if (scalar < -1.0) scalar = -1.0;
//    if (scalar >  1.0) scalar = 1.0;
//    
//    int numScalarColors = this->getNumberOfScalarsAndColors();
//    if (numScalarColors > 0) {
//        
//        int paletteIndex = -1;
//        if (numScalarColors == 1) {
//            paletteIndex = 0;
//            interpolateColorFlag = false;
//        }
//        else {
//            if (scalar >= this->getScalarAndColor(0)->getScalar()) {
//                paletteIndex = 0;
//                interpolateColorFlag = false;
//            }
//            else if (scalar <=
//                     this->getScalarAndColor(numScalarColors - 1)->getScalar()) {
//                paletteIndex = numScalarColors - 1;
//                interpolateColorFlag = false;
//            }
//            else {
//                for (int i = 1; i < numScalarColors; i++) {
//                    const PaletteScalarAndColor* psac = this->getScalarAndColor(i);
//                    if (scalar > psac->getScalar()) {
//                        paletteIndex = i - 1;
//                        break;
//                    }
//                }
//                
//                /*
//                 * Always interpolate if there are only two colors
//                 */
//                if (numScalarColors == 2) {
//                    interpolateColorFlag = true;
//                }
//            }
//        }
//        if (paletteIndex >= 0) {
//            const PaletteScalarAndColor* psac = this->getScalarAndColor(paletteIndex);
//            psac->getColor(rgbaOut);
//            if (interpolateColorFlag &&
//                (paletteIndex < (numScalarColors - 1))) {
//                const PaletteScalarAndColor* psacBelow =
//                    this->getScalarAndColor(paletteIndex + 1);
//                float totalDiff = psac->getScalar() - psacBelow->getScalar();
//                if (totalDiff != 0.0) {
//                    float offset = scalar - psacBelow->getScalar();
//                    float percentAbove = offset / totalDiff;
//                    float percentBelow = 1.0f - percentAbove;
//                    if ( ! psacBelow->isNoneColor()) {
//                        const float* rgbaAbove = psac->getColor();
//                        const float* rgbaBelow = psacBelow->getColor();
//                        
//                        rgbaOut[0] = (percentAbove * rgbaAbove[0]
//                                      + percentBelow * rgbaBelow[0]);
//                        rgbaOut[1] = (percentAbove * rgbaAbove[1]
//                                      + percentBelow * rgbaBelow[1]);
//                        rgbaOut[2] = (percentAbove * rgbaAbove[2]
//                                      + percentBelow * rgbaBelow[2]);
//                    }
//                }
//            }
//            else if (psac->isNoneColor()) {
//                rgbaOut[3] = 0.0f;
//            }
//        }
//    }
//}


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
    /*
     * When the number of colors in a palette is small, the
     * binary search algorithm may be slower than the linear
     * algorithm.  It is moderately faster for large palettes
     * such as those from FSL with 256 colors.
     * 
     * TSC: The FSL palettes now just interpolate between two colors,
     * and don't have 256 entries.  Could reorganize the palette
     * storage to be a vector of structs, rather than of pointers,
     * to remove cost of indirection.
     * Also notable is that typical volume files color faster with
     * methods that color near-zero faster than other values.
     * I compared performance on the simplified palettes with a simplified
     * binary search (that doesn't test against the next value after
     * each guess) and with an interpolation search, with no benefit.
     * Another possibility would be to prebuild a lookup from rounded
     * normalized value to min and max possible reference color to
     * search between (so, values between 0 and 0.01 are always between
     * the middle and previous point, same for 0.01 to 0.02, etc).  This
     * could take some substantial reorganization of palette code.
     * For now, only activate binary search when number of points is large.
     *
     * The linear search could be improved by starting at the bottom
     * when the data value is negative.
     *
     * N   Log2(N)
     * 1     0
     * 2     1
     * 3     1.6
     * 4     2
     * 5     2.3
     * 6     2.6
     * 7     2.8
     * 8     3
     * 9     3.2
     */
    int numScalarColors = this->getNumberOfScalarsAndColors();
    const bool doBinarySearchFlag = numScalarColors > 50;
    
    rgbaOut[0] = 0.0f;
    rgbaOut[1] = 0.0f;
    rgbaOut[2] = 0.0f;
    rgbaOut[3] = 1.0f;
    
    bool interpolateColorFlag = interpolateColorFlagIn;
    
    float scalar = scalarIn;
    if (scalar < -1.0) scalar = -1.0;
    if (scalar >  1.0) scalar = 1.0;
    
    if (numScalarColors > 0) {
        
        int32_t highDataIndex = 0;
        int32_t lowDataIndex  = numScalarColors - 1;
        int32_t paletteIndex = -1;
        
        if (numScalarColors == 1) {
            paletteIndex = 0;
            interpolateColorFlag = false;
        }
        else {
            if (scalar >= this->getScalarAndColor(highDataIndex)->getScalar()) {
                paletteIndex = 0;
                interpolateColorFlag = false;
            }
            else if (scalar <= this->getScalarAndColor(lowDataIndex)->getScalar()) {
                paletteIndex = numScalarColors - 1;
                interpolateColorFlag = false;
            }
            else if (numScalarColors == 2) {
                paletteIndex = 0;
                interpolateColorFlag = true;
            }
            else {
                if (doBinarySearchFlag) {
                    /*
                     * Binary Search
                     * NOTE: The palette orders the scalars in DESCENDING ORDER
                     */
                    int32_t binaryPaletteIndex = -1;
                    const int32_t maximumIndex = numScalarColors - 1;
                    
                    bool loopFlag = true;
                    while (loopFlag) {
                        int32_t midIndex = (lowDataIndex + highDataIndex) / 2;
                        if (midIndex <= 0) {
                            binaryPaletteIndex = 0;
                            loopFlag = false;
                        }
                        else if (midIndex >= maximumIndex) {
                            binaryPaletteIndex = maximumIndex;
                            loopFlag = false;
                        }
                        else {
                            const float midScalar = this->getScalarAndColor(midIndex)->getScalar();
                            if (scalar <= midScalar) {
                                const float nextScalar = this->getScalarAndColor(midIndex + 1)->getScalar();
                                if (scalar > nextScalar) {
                                    binaryPaletteIndex = midIndex;
                                    loopFlag = false;
                                }
                                else {
                                    highDataIndex = midIndex;
                                }
                            }
                            else {
                                lowDataIndex = midIndex;
                            }
                        }
                    }
                    
                    paletteIndex = binaryPaletteIndex;
                }
                else {
                    /*
                     * Linear Search
                     */
                    for (int32_t i = 1; i < numScalarColors; i++) {
                        const PaletteScalarAndColor* psac = this->getScalarAndColor(i);
                        if (scalar > psac->getScalar()) {
                            paletteIndex = i - 1;
                            break;
                        }
                    }
                }
            }
        }
        if (paletteIndex >= 0) {
            const PaletteScalarAndColor* psac = this->getScalarAndColor(paletteIndex);
            if (psac->isNoneColor()) {
                rgbaOut[3] = 0.0;
            }
            else {
                psac->getColor(rgbaOut); // color assigned here
                if (interpolateColorFlag &&
                    (paletteIndex < (numScalarColors - 1))) {
                    const PaletteScalarAndColor* psacBelow = this->getScalarAndColor(paletteIndex + 1);
                    float totalDiff = psac->getScalar() - psacBelow->getScalar();
                    if (totalDiff != 0.0) {
                        float offset = scalar - psacBelow->getScalar();
                        float percentAbove = offset / totalDiff;
                        float percentBelow = 1.0f - percentAbove;
                        if ( ! psacBelow->isNoneColor()) {
                            const float* rgbaAbove = psac->getColor();
                            const float* rgbaBelow = psacBelow->getColor();
                            
                            rgbaOut[0] = (percentAbove * rgbaAbove[0]
                                          + percentBelow * rgbaBelow[0]);
                            rgbaOut[1] = (percentAbove * rgbaAbove[1]
                                          + percentBelow * rgbaBelow[1]);
                            rgbaOut[2] = (percentAbove * rgbaAbove[2]
                                          + percentBelow * rgbaBelow[2]);
                        }
                    }
                }
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

/**
 * @return An sign separate inverted version of this palette.  
 * The positive and negative sections are separately inverted.
 *
 * For the POSITIVE region, the conversion formula is:
 *    IS = -S + (Min + Max)
 * Where:
 *    IS => Inverted Scalar
 *    S  => Scalar
 *    Min => Minimum positive value
 *    Max => Maximum positive value
 *
 * The formula was derived by charting:
 *    Plot (x=Min, y=Max) and (x=Max, y=Min)
 *    The slope is negative one [dx = (max - min); dy = (min-max)]
 *    So Y = -(dy/dx) * X + C
 *       Y = -X + C
 *       C = X + Y
 *       C = (Min + Max)
 * And finally:
 *       Y = -X + (Min + Max)
 *
 * Example: (1.0, Red), (0.4, Yellow), (0, Black), (-0.3, Green), (-1.0, Blue)
 * becomes  (1.0, Black), (0.6, Yellow), (1.0, Red), (0, Blue), (-0.7, Green), (-1.0, Black)
 */
const Palette*
Palette::getSignSeparateInvertedPalette() const
{
    if ( ! m_signSeparateInvertedPalette) {
        float leastPositiveScalar =  1000.0f;
        float mostPositiveScalar  = -1000.0f;
        float leastNegativeScalar = -1000.0f;
        float mostNegativeScalar  =  1000.0f;
        bool havePositivesFlag = false;
        bool haveNegativesFlag = false;
        for (const auto ps : paletteScalars) {
            const float scalar = ps->getScalar();
            
            if (scalar >= 0.0) {
                if (scalar > mostPositiveScalar)  mostPositiveScalar  = scalar;
                if (scalar < leastPositiveScalar) leastPositiveScalar = scalar;
                if (scalar > 0.0) {
                    havePositivesFlag = true;
                }
            }
            
            if (scalar <= 0.0) {
                if (scalar > leastNegativeScalar) leastNegativeScalar = scalar;
                if (scalar < mostNegativeScalar)  mostNegativeScalar  = scalar;
                if (scalar < 0.0) {
                    haveNegativesFlag = true;
                }
            }
        }
        
        std::deque<PaletteScalarAndColor*> positives;
        std::deque<PaletteScalarAndColor*> negatives;
        
        for (const auto ps : paletteScalars) {
            const float scalar = ps->getScalar();
            if (havePositivesFlag) {
                if (scalar >= 0.0) {
                    PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
                    psc->setScalar((leastPositiveScalar + mostPositiveScalar) - scalar);
                    positives.push_front(psc);
                }
            }
            if (haveNegativesFlag) {
                if (scalar <= 0.0) {
                    PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
                    psc->setScalar((leastNegativeScalar + mostNegativeScalar) - scalar);
                    negatives.push_front(psc);
                }
            }
        }
        
        Palette* palette = new Palette();
        palette->setName(getName());
        palette->paletteScalars.insert(palette->paletteScalars.end(),
                                       positives.begin(), positives.end());

        palette->paletteScalars.insert(palette->paletteScalars.end(),
                                       negatives.begin(), negatives.end());
        
        palette->clearModified();
        
        m_signSeparateInvertedPalette.reset(palette);
    }
    
    return m_signSeparateInvertedPalette.get();
}



/**
 * @return An inverted version of this palette.  An inverted
 * palette may be useful when data is all negative and the
 * palette is for positive data.
 *
 * Example: (1.0, Red), (0.4, Yellow), (-0.3, Green), (-1.0, Blue)
 * becomes  (1.0, Blue), (0.3, Green), (-0.4, Yellow), (-1.0, Red)
 */
const Palette*
Palette::getInvertedPalette() const
{
    if ( ! m_invertedPalette) {
        Palette* palette = new Palette(*this);
        std::reverse(palette->paletteScalars.begin(), palette->paletteScalars.end());
        
        /*
         * Reverse colors and then flip sign of scalars so
         * that it is still negative to positive
         */
        for (auto scalar : palette->paletteScalars) {
            scalar->setScalar( - scalar->getScalar());
        }
        
        palette->clearModified();
        
        m_invertedPalette.reset(palette);
    }
    
    return m_invertedPalette.get();
}

/**
 * @return Name of the default palette.
 */
AString
Palette::getDefaultPaletteName()
{
    return ROY_BIG_BL_PALETTE_NAME;
}



