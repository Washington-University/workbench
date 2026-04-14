/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
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

#include "PaletteNew.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"

#include "ColorFunctions.h"
#include "MathFunctions.h"

#include <algorithm>

using namespace std;
using namespace caret;

namespace
{
    int searchLowSide(const float scalar, const vector<PaletteNew::ScalarColor>& controlPoints, int low, int high)
    {
        while (low < high - 1)
        {
            int guess = (high + low) / 2;//simple bisection search
            if (scalar < controlPoints[guess].scalar)
            {
                high = guess;
            } else {
                low = guess;
            }
        }
        return low;
    }
    
    void copyColor(float to[3], const float from[3])
    {
        for (int i = 0; i < 3; ++i)
        {
            CaretAssert(from[i] >= 0.0f && from[i] <= 1.0f);
            to[i] = from[i];
        }
    }
}

PaletteNew::PaletteNew(vector<ScalarColor> posRange, float zeroColor[3], vector<ScalarColor> negRange)
: PaletteBase(PaletteDesignTypeEnum::PALETTE_NEW), m_posRange(posRange), m_negRange(negRange)
{
    CaretAssert(posRange[0].scalar == 0.0f);
    CaretAssert(posRange.back().scalar == 1.0f);
    CaretAssert(negRange[0].scalar == -1.0f);
    CaretAssert(negRange.back().scalar == 0.0f);
    copyColor(m_zeroColor, zeroColor);
}

PaletteNew::PaletteNew(const PaletteNew& palette)
: PaletteBase(PaletteDesignTypeEnum::PALETTE_NEW), m_posRange(palette.m_posRange), m_negRange(palette.m_negRange)
{
    copyColor(m_zeroColor, palette.m_zeroColor);
    m_name = palette.m_name;
//    CaretAssert(m_posRange[0].scalar == 0.0f);
//    CaretAssert(m_posRange.back().scalar == 1.0f);
//    CaretAssert(m_negRange[0].scalar == -1.0f);
//    CaretAssert(m_negRange.back().scalar == 0.0f);
}

PaletteNew::~PaletteNew()
{
}

void PaletteNew::updateRanges(std::vector<ScalarColor> posRange, const float zeroColor[3], std::vector<ScalarColor> negRange)
{
    CaretAssert(posRange[0].scalar == 0.0f);
    CaretAssert(posRange.back().scalar == 1.0f);
    CaretAssert(negRange[0].scalar == -1.0f);
    CaretAssert(negRange.back().scalar == 0.0f);
    m_posRange = posRange;
    m_negRange = negRange;
    copyColor(m_zeroColor, zeroColor);
}

void PaletteNew::getPositiveColor(const float scalar, float rgbOut[3]) const
{
    CaretAssert(scalar >= 0.0f);
    m_posRange.getPaletteColor(scalar, rgbOut);
}

void PaletteNew::getNegativeColor(const float scalar, float rgbOut[3]) const
{
    CaretAssert(scalar <= 0.0f);
    m_negRange.getPaletteColor(scalar, rgbOut);
}

void PaletteNew::getPaletteColor(const float scalar,
                                 const bool /*interpolateColorFlag*/,
                                 float rgbaOut[4]) const
{
    if (scalar > 0.00001) {
        getPositiveColor(scalar, rgbaOut);
    }
    else if (scalar < -0.00001) {
        getNegativeColor(scalar, rgbaOut);
    }
    else {
        getZeroColor(rgbaOut);
    }
    rgbaOut[3] = 1.0;
}

PaletteNew::PaletteRange::PaletteRange(const vector<ScalarColor> controlPoints)
{
    CaretAssert(controlPoints.size() > 1);
    for (size_t i = 1; i < controlPoints.size(); ++i)
    {
        if (controlPoints[i].scalar <= controlPoints[i - 1].scalar)
        {
            CaretAssert(false);
            throw CaretException("palette ranges must use unique, ascending-sorted scalars");
        }
    }
    m_controlPoints = controlPoints;
    m_lowPoint = m_controlPoints[0].scalar;
    m_highPoint = m_controlPoints.back().scalar;
    float diff = m_highPoint - m_lowPoint;//in practice this should always be 1
    m_lookup[0] = 0;
    m_lookup[BUCKETS] = m_controlPoints.size() - 2;//do not allow search to ever say that it is past the last control point
    for (int i = 1; i < BUCKETS; ++i)
    {
        m_lookup[i] = searchLowSide(m_lowPoint + (diff * i) / (BUCKETS), m_controlPoints, 0, m_controlPoints.size() - 1);//save the low control point at each bucket wall (can derive high from next bucket)
    }
}

void PaletteNew::PaletteRange::getPaletteColor(const float scalar, float rgbOut[3]) const
{
    CaretAssert(MathFunctions::isNumeric(scalar));
    int bucket = int((scalar - m_lowPoint) / (m_highPoint - m_lowPoint) * BUCKETS);//this implicitly tests for below or above the palette range
    if (bucket < 0)
    {
        copyColor(rgbOut, m_controlPoints[0].color);
        return;
    }
    if (bucket >= BUCKETS)
    {
        copyColor(rgbOut, m_controlPoints.back().color);
        return;
    }
    int lowSide = searchLowSide(scalar, m_controlPoints, m_lookup[bucket], m_lookup[bucket + 1] + 1);//this is why m_lookup is [BUCKETS + 1]
    float interpFactor = (scalar - m_controlPoints[lowSide].scalar) / (m_controlPoints[lowSide + 1].scalar - m_controlPoints[lowSide].scalar);
    for (int i = 0; i < 3; ++i)
    {//never output outside of [0, 1]
        rgbOut[i] = max(0.0f, min(1.0f, m_controlPoints[lowSide].color[i] * (1 - interpFactor) + m_controlPoints[lowSide + 1].color[i] * interpFactor));
    }
}

vector<float> PaletteNew::PaletteRange::getPerceptualGradient(const int numBuckets) const
{
    CaretAssert(numBuckets >= 3);//one at min, one at max, all equally spaced
    vector<float> ret(numBuckets);
    ScalarColor lowColor, midColor, highColor;
    midColor.scalar = m_lowPoint;
    getPaletteColor(midColor.scalar, midColor.color);
    lowColor = midColor;//use central difference, with unsymmetric endpoints
    for (int i = 1; i < numBuckets - 1; ++i)
    {
        float interpFactor = float(i) / (numBuckets - 1);
        highColor.scalar = (1 - interpFactor) * m_lowPoint + interpFactor * m_highPoint;
        getPaletteColor(highColor.scalar, highColor.color);
        ret[i - 1] = ColorFunctions::perceptualDistanceSRGB(highColor.color, lowColor.color) / (highColor.scalar - lowColor.scalar);
        lowColor = midColor;
        midColor = highColor;
    }
    ret[numBuckets - 1] = ColorFunctions::perceptualDistanceSRGB(highColor.color, midColor.color) / (highColor.scalar - midColor.scalar);
    return ret;
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
const PaletteBase*
PaletteNew::getSignSeparateInvertedPalette() const
{
    return NULL;
//    if ( ! m_signSeparateInvertedPalette) {
//        
//        m_signSeparateInvertedPalette.reset(createSignSeparateInvertedPalette());
//    }
//    
//    return m_signSeparateInvertedPalette.get();
}

/**
 * @return New instance of sign separate palette
 */
PaletteNew*
PaletteNew::createSignSeparateInvertedPalette() const
{
    return NULL;
//    float leastPositiveScalar =  1000.0f;
//    float mostPositiveScalar  = -1000.0f;
//    float leastNegativeScalar = -1000.0f;
//    float mostNegativeScalar  =  1000.0f;
//    bool havePositivesFlag = false;
//    bool haveNegativesFlag = false;
//    for (const auto ps : paletteScalars) {
//        const float scalar = ps->getScalar();
//        
//        if (scalar >= 0.0) {
//            if (scalar > mostPositiveScalar)  mostPositiveScalar  = scalar;
//            if (scalar < leastPositiveScalar) leastPositiveScalar = scalar;
//            if (scalar > 0.0) {
//                havePositivesFlag = true;
//            }
//        }
//        
//        if (scalar <= 0.0) {
//            if (scalar > leastNegativeScalar) leastNegativeScalar = scalar;
//            if (scalar < mostNegativeScalar)  mostNegativeScalar  = scalar;
//            if (scalar < 0.0) {
//                haveNegativesFlag = true;
//            }
//        }
//    }
//    
//    std::deque<PaletteScalarAndColor*> positives;
//    std::deque<PaletteScalarAndColor*> negatives;
//    
//    for (const auto ps : paletteScalars) {
//        const float scalar = ps->getScalar();
//        if (havePositivesFlag) {
//            if (scalar >= 0.0) {
//                PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
//                psc->setScalar((leastPositiveScalar + mostPositiveScalar) - scalar);
//                positives.push_front(psc);
//            }
//        }
//        if (haveNegativesFlag) {
//            if (scalar <= 0.0) {
//                PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
//                psc->setScalar((leastNegativeScalar + mostNegativeScalar) - scalar);
//                negatives.push_front(psc);
//            }
//        }
//    }
//    
//    Palette* palette = new Palette();
//    palette->setName(getName());
//    palette->paletteScalars.insert(palette->paletteScalars.end(),
//                                   positives.begin(), positives.end());
//    
//    palette->paletteScalars.insert(palette->paletteScalars.end(),
//                                   negatives.begin(), negatives.end());
//    
//    palette->clearModified();
//    
//    return palette;
}




/**
 * @return An inverted version of this palette.  An inverted
 * palette may be useful when data is all negative and the
 * palette is for positive data.
 *
 * Example: (1.0, Red), (0.4, Yellow), (-0.3, Green), (-1.0, Blue)
 * becomes  (1.0, Blue), (0.3, Green), (-0.4, Yellow), (-1.0, Red)
 */
const PaletteBase*
PaletteNew::getInvertedPalette() const
{
    return NULL;
//    if ( ! m_invertedPalette) {
//        Palette* palette = new Palette(*this);
//        std::reverse(palette->paletteScalars.begin(), palette->paletteScalars.end());
//        
//        /*
//         * Reverse colors and then flip sign of scalars so
//         * that it is still negative to positive
//         */
//        for (auto scalar : palette->paletteScalars) {
//            scalar->setScalar( - scalar->getScalar());
//        }
//        
//        palette->clearModified();
//        
//        m_invertedPalette.reset(palette);
//    }
//    
//    return m_invertedPalette.get();
}

/**
 * @return An NONE separate inverted version of this palette.
 * The positive and negative sections are separately inverted
 * around the NONE color.  If the palette contains zero it MUST
 * be the NONE color.
 *
 * If the palette DOES NOT have NONE or if it has both
 * NONE and zero, the sign separate palette is created and returned.
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
const PaletteBase*
PaletteNew::getNoneSeparateInvertedPalette() const
{
    return NULL;
//    if ( ! m_noneSeparateInvertedPalette) {
//        float leastPositiveScalar =  1000.0f;
//        float mostPositiveScalar  = -1000.0f;
//        float leastNegativeScalar = -1000.0f;
//        float mostNegativeScalar  =  1000.0f;
//        int32_t noneColorIndex = -1;
//        int32_t zeroIndex = -1;
//        
//        const int32_t numScalars = static_cast<int32_t>(this->paletteScalars.size());
//        for (int32_t i = 0; i < numScalars; i++) {
//            CaretAssertVectorIndex(this->paletteScalars, i);
//            const float scalar = this->paletteScalars[i]->getScalar();
//            
//            if (this->paletteScalars[i]->isNoneColor()) {
//                noneColorIndex = i;
//            }
//            else {
//                if (scalar == 0.0) {
//                    zeroIndex = i;
//                }
//                if (scalar >= 0.0) {
//                    if (scalar > mostPositiveScalar)  mostPositiveScalar  = scalar;
//                    if (scalar < leastPositiveScalar) leastPositiveScalar = scalar;
//                }
//                if (scalar <= 0.0) {
//                    if (scalar > leastNegativeScalar) leastNegativeScalar = scalar;
//                    if (scalar < mostNegativeScalar)  mostNegativeScalar  = scalar;
//                }
//                
//            }
//        }
//        
//        bool badNoneLocationFlag = false;
//        if (noneColorIndex >= 0) {
//            CaretAssertVectorIndex(this->paletteScalars, noneColorIndex);
//            const float noneValue = this->paletteScalars[noneColorIndex]->getScalar();
//            if ((noneValue >= leastPositiveScalar)
//                && (noneValue <= mostPositiveScalar)) {
//                badNoneLocationFlag = true;
//            }
//            else if ((noneValue >= mostNegativeScalar)
//                     && (noneValue <= leastNegativeScalar)) {
//                badNoneLocationFlag = true;
//            }
//        }
//        
//        bool validNoneInvertFlag = true;
//        if (badNoneLocationFlag) {
//            const AString msg("Palette named " + getName()
//                              + " has NONE within the positive or negative range.");
//            CaretAssertMessage(0, msg);
//            validNoneInvertFlag = false;
//        }
//        else if (noneColorIndex < 0) {
//            /* no NONE so same as sign separate palette */
//            validNoneInvertFlag = false;
//        }
//        else if (zeroIndex >= 0) {
//            /* both none and zero not allowed */
//            const AString msg("Palette named " + getName()
//                              + " has both NONE and zero so NONE separated palette cannot be created.");
//            CaretAssertMessage(0, msg);
//            validNoneInvertFlag = false;
//        }
//        if ( ! validNoneInvertFlag) {
//            m_noneSeparateInvertedPalette.reset(createSignSeparateInvertedPalette());
//            return m_noneSeparateInvertedPalette.get();
//        }
//        
//        std::deque<PaletteScalarAndColor*> positives;
//        std::deque<PaletteScalarAndColor*> negatives;
//        
//        for (int32_t i = 0; i < numScalars; i++) {
//            CaretAssertVectorIndex(this->paletteScalars, i);
//            const PaletteScalarAndColor* ps = this->paletteScalars[i];
//            const float scalar = ps->getScalar();
//            
//            if (i < noneColorIndex) {
//                PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
//                psc->setScalar((leastPositiveScalar + mostPositiveScalar) - scalar);
//                positives.push_front(psc);
//            }
//            else if (i > noneColorIndex) {
//                PaletteScalarAndColor* psc = new PaletteScalarAndColor(*ps);
//                psc->setScalar((leastNegativeScalar + mostNegativeScalar) - scalar);
//                negatives.push_front(psc);
//            }
//        }
//        
//        
//        Palette* palette = new Palette();
//        palette->setName(getName());
//        palette->paletteScalars.insert(palette->paletteScalars.end(),
//                                       positives.begin(), positives.end());
//        
//        CaretAssertVectorIndex(this->paletteScalars, noneColorIndex);
//        palette->paletteScalars.push_back(new PaletteScalarAndColor(*this->paletteScalars[noneColorIndex]));
//        
//        palette->paletteScalars.insert(palette->paletteScalars.end(),
//                                       negatives.begin(), negatives.end());
//        
//        palette->clearModified();
//        
//        m_noneSeparateInvertedPalette.reset(palette);
//    }
//    
//    return m_noneSeparateInvertedPalette.get();
}
