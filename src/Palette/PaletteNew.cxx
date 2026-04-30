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
#include "DataFileTypeEnum.h"
#include "MathFunctions.h"
#include "Palette.h"
#include "PaletteNewXmlStreamReader.h"
#include "PaletteNewXmlStreamWriter.h"
#include "PaletteScalarAndColor.h"

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

PaletteNew::PaletteNew(const vector<ScalarColor>& posRange, const float zeroColor[3], const vector<ScalarColor>& negRange)
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
    
    m_convertedToPalette.reset();
}

void PaletteNew::setName(const AString& name)
{
    m_name = name;
    
    m_convertedToPalette.reset();
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
 * @return This instance converted to a Palette
 */
const Palette*
PaletteNew::getAsPalette() const
{
    if (m_convertedToPalette) {
        return m_convertedToPalette.get();
    }
    
    const std::vector<ScalarColor> posScalars(m_posRange.getRange());
    const std::vector<ScalarColor> negScalars(m_negRange.getRange());

    Palette* palette(new Palette());
    palette->setName(getName());
    
    /*
     * Positive range has 0 first and 1 last so interate in reverse
     * Also, use a small positive value instead of zero
     */
    const int32_t numPosScalars(posScalars.size());
    for (int32_t i = (numPosScalars - 1); i >= 0; i--) {
        CaretAssertVectorIndex(posScalars, i);
        const ScalarColor& sc(posScalars[i]);
        const float scalar((i == 0)
                           ? 0.00001
                           : sc.scalar);
        const AString colorName("PosColor_" + AString::number(scalar, 'f', 6));
        const float rgba[4] { sc.color[0], sc.color[1], sc.color[2], 1.0 };
        palette->addScalarAndColor(scalar, colorName, rgba);
    }
    
    const AString colorName("ZeroColor");
    const float rgba[4] { m_zeroColor[0], m_zeroColor[1], m_zeroColor[2], 1.0 };
    palette->addScalarAndColor(0.0, colorName, rgba);
    
    /*
     * Negative range has -1 first and 0 last
     * Use a small negative value instead of zero
     */
    const int32_t numNegScalars(negScalars.size());
    for (int32_t i = (numNegScalars - 1); i >= 0; i--) {
        CaretAssertVectorIndex(negScalars, i);
        const ScalarColor& sc(negScalars[i]);
        const float scalar((i == (numNegScalars - 1))
                           ? -0.00001
                           : sc.scalar);
        const AString colorName("NegColor_" + AString::number(scalar, 'f', 6));
        const float rgba[4] { sc.color[0], sc.color[1], sc.color[2], 1.0 };
        palette->addScalarAndColor(scalar, colorName, rgba);
    }
    
    const bool testFlag(false);
    if (testFlag) {
        std::cout << "PaletteNew: " << std::endl;
        std::cout << toString() << std::endl;
        std::cout << "Palette" << std::endl;
        std::cout << palette->toString() << std::endl;
        std::cout << std::flush;
    }
    
    m_convertedToPalette.reset(palette);
    
    return m_convertedToPalette.get();
}

PaletteNew*
PaletteNew::createFromPalette(const Palette* palette,
                              AString& importNotesOut)
{
    importNotesOut.clear();
    PaletteNew* paletteOut(NULL);
    
    std::vector<ScalarColor> posRange;
    std::vector<ScalarColor> negRange;
    float zeroColor[3] = { 0.0, 0.0, 0.0 };
    bool haveZeroFlag(false);
    
    /*
     * Separate into positive, negative, and zero
     */
    const int32_t numScalarsAndColors(palette->getNumberOfScalarsAndColors());
    for (int32_t i = 0; i < numScalarsAndColors; i++) {
        const PaletteScalarAndColor* psac(palette->getScalarAndColor(i));
        const float scalar(psac->getScalar());
        float rgba[4];
        psac->getColor(rgba);
        
        if (scalar > 0.0) {
            posRange.push_back(ScalarColor(scalar, rgba));
        }
        else if (scalar < 0.0) {
            negRange.push_back(ScalarColor(scalar, rgba));
        }
        else {
            zeroColor[0] = rgba[0];
            zeroColor[1] = rgba[1];
            zeroColor[2] = rgba[2];
            haveZeroFlag = true;
        }
    }
    
    const float red[3] { 1.0, 0.0, 0.0 };
    const float black[3] { 0.0, 0.0, 0.0 };
    const float blue[3] { 0.0, 0.0, 1.0 };
    if (posRange.empty()) {
        posRange.push_back(ScalarColor(1.0, red));
        posRange.push_back(ScalarColor(0.0, black));
        importNotesOut.appendWithNewLine("Added positive range.");
    }
    else {
        /*
         * Must have 1.0 as first positive scalar
         */
        if (posRange.front().scalar != 1.0) {
            float rgba[4];
            palette->getPaletteColor(1.0, true, rgba);
            posRange.insert(posRange.begin(), ScalarColor(1.0, rgba));
            importNotesOut.appendWithNewLine("Added positive 1.0.");
        }
        
        /*
         * Must have 0.0 as last positive scalar
         */
        if (posRange.back().scalar != 0.0) {
            float rgba[4];
            palette->getPaletteColor(0.0, true, rgba);
            posRange.insert(posRange.end(), ScalarColor(0.0, rgba));
            importNotesOut.appendWithNewLine("Added positive 0.0.");
        }
    }
    
    if (negRange.empty()) {
        negRange.push_back(ScalarColor(0.0, black));
        negRange.push_back(ScalarColor(-1.0, blue));
        importNotesOut.appendWithNewLine("Added negative range.");
    }
    else {
        /*
         * Must have 0.0 as first negative scalar
         */
        if (negRange.front().scalar != 0.0) {
            float rgba[4];
            palette->getPaletteColor(0.0, true, rgba);
            negRange.insert(negRange.begin(), ScalarColor(0.0, rgba));
            importNotesOut.appendWithNewLine("Added negative 0.0.");
        }
        
        /*
         * Must have -1.0 as first positive scalar
         */
        if (negRange.back().scalar != -1.0) {
            float rgba[4];
            palette->getPaletteColor(-1.0, true, rgba);
            negRange.insert(negRange.end(), ScalarColor(-1.0, rgba));
            importNotesOut.appendWithNewLine("Added negative -1.0.");
        }
    }
    
    if ( ! haveZeroFlag) {
        float rgba[4];
        palette->getPaletteColor(0.0, true, rgba);
        zeroColor[0] = rgba[0];
        zeroColor[1] = rgba[1];
        zeroColor[2] = rgba[2];
        importNotesOut.appendWithNewLine("Added 0.0.");
    }
    
    std::reverse(posRange.begin(), posRange.end());
    std::reverse(negRange.begin(), negRange.end());
    
    paletteOut = new PaletteNew(posRange, zeroColor, negRange);
    
    CaretLogFine("Palette In:\n" + palette->toString());
    CaretLogFine("New Copy:\n" + paletteOut->toString());
    
    return paletteOut;
}

AString
PaletteNew::toString() const
{
    AString s;
    s += "name=" + getName() + "\n";
    
    const std::vector<ScalarColor> posScalars(m_posRange.getRange());
    const std::vector<ScalarColor> negScalars(m_negRange.getRange());

    const int32_t numPosScalars(posScalars.size());
    for (int32_t i = (numPosScalars - 1); i >= 0; i--) {
        CaretAssertVectorIndex(posScalars, i);
        const ScalarColor& sc(posScalars[i]);
        s += ("    " + AString::number(sc.scalar, 'f', 5) + " " + AString::fromNumbers(sc.color, 3, ",") + "\n");
    }
    
    s += ("       zero " + AString::fromNumbers(m_zeroColor, 3, ",")+ "\n");

    const int32_t numNegScalars(negScalars.size());
    for (int32_t i = (numNegScalars - 1); i >= 0; i--) {
        CaretAssertVectorIndex(negScalars, i);
        const ScalarColor& sc(negScalars[i]);
        s += ("   " + AString::number(sc.scalar, 'f', 5) + " " + AString::fromNumbers(sc.color, 3, ",")+ "\n");
    }

    return s;
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
    /*
     * THIS IS NOT USED
     */
    return NULL;
}

/**
 * @return New instance of sign separate palette
 */
PaletteNew*
PaletteNew::createSignSeparateInvertedPalette() const
{
    /*
     * THIS IS NOT USED
     */
    return NULL;
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
    if ( ! m_invertedPalette) {
        /*
         * Note: Positives are in order 0..1
         *       Negatives are in order -1..0
         */
        std::vector<ScalarColor> posScalars(m_posRange.getRange());
        std::vector<ScalarColor> negScalars(m_negRange.getRange());
        
        std::reverse(posScalars.begin(), posScalars.end());
        std::reverse(negScalars.begin(), negScalars.end());
        
        for (auto& sc : posScalars) {
            sc.scalar *= -1.0;
        }
        for (auto& sc : negScalars) {
            sc.scalar *= -1.0;
        }

        PaletteNew* p(new PaletteNew(negScalars, m_zeroColor, posScalars));
        p->setName(getName());

        const bool testFlag(false);
        if (testFlag) {
            std::cout << "Palette" << std::endl;
            std::cout << toString() << std::endl;
            std::cout << "Inverted" << std::endl;
            std::cout << p->toString() << std::endl;
        }
        
        m_invertedPalette.reset(p);
    }
    
    return m_invertedPalette.get();
}

/**
 * @return An NONE separate inverted version of this palette.
 *
 * PaletteNew does not support a "none" color so just use the inverted palette.
 */
const PaletteBase*
PaletteNew::getNoneSeparateInvertedPalette() const
{
    return getInvertedPalette();
}

/**
 * @return Function result with palette read from file or error
 * @param filename
 *    Name of file
 */
FunctionResultValue<PaletteNew*>
PaletteNew::readFromFile(const AString& filename)
{
    PaletteNewXmlStreamReader paletteXmlReader;
    return paletteXmlReader.readFromFile(filename);
}

/**
 * Write the palette to the given file
 * @param filename
 *    Name of file
 * @return
 *    Function result with success or failure
 */
FunctionResult
PaletteNew::writeToFile(const AString& filename) const
{
    PaletteNewXmlStreamWriter paletteXmlWriter;
    FunctionResult result(paletteXmlWriter.writeToFile(*this,
                                                       filename));
    return result;
}

/**
 * @return Function result with palette read from file or error
 * @param filename
 *    Name of file
 */
FunctionResultValue<PaletteNew*>
PaletteNew::readFromString(const AString& string)
{
    PaletteNewXmlStreamReader paletteXmlReader;
    return paletteXmlReader.readFromString(string);
}

/**
 * Write the palette to the given file
 * @param filename
 *    Name of file
 * @return
 *    Function result with success or failure
 */
FunctionResultValue<AString>
PaletteNew::writeToString() const
{
    PaletteNewXmlStreamWriter paletteXmlWriter;
    return paletteXmlWriter.writeToString(*this);
}

