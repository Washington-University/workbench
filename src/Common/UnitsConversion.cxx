
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __UNITS_CONVERSION_DECLARE__
#include "UnitsConversion.h"
#undef __UNITS_CONVERSION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::UnitsConversion 
 * \brief Convert a 'units value' from one to another
 * \ingroup Common
 */

/**
 * Constructor.
 */
UnitsConversion::UnitsConversion()
{
    
}

/**
 * Destructor.
 */
UnitsConversion::~UnitsConversion()
{
}

/**
 * Convert length value to different units
 * @param fromUnits
 *    Units of input value
 * @param toUnits
 *    Units for output value
 * @param value
 *    Value to convert to different units
 */
float
UnitsConversion::convertLength(const LengthUnits fromUnits,
                               const LengthUnits toUnits,
                               const float lengthValue)
{
    /*
     * Convert length to millimeters
     */
    float lengthMillimeters(0.0);
    switch (fromUnits) {
        case LengthUnits::CENTIMETERS:
            lengthMillimeters = lengthValue * CENTIMETERS_TO_MILLIMETERS;
            break;
        case LengthUnits::INCHES:
            lengthMillimeters = lengthValue * INCHES_TO_MILLIMETERS;
            break;
        case LengthUnits::METERS:
            lengthMillimeters = lengthValue * METERS_TO_MILLIMETERS;
            break;
        case LengthUnits::MILLIMETERS:
            lengthMillimeters = lengthValue;
            break;
    }
    
    /**
     * Convert millimeters to output units
     */
    float lengthOut(0.0);
    switch (toUnits) {
        case LengthUnits::CENTIMETERS:
            lengthOut = lengthMillimeters / CENTIMETERS_TO_MILLIMETERS;
            break;
        case LengthUnits::INCHES:
            lengthOut = lengthMillimeters / INCHES_TO_MILLIMETERS;
            break;
        case LengthUnits::METERS:
            lengthOut = lengthMillimeters / METERS_TO_MILLIMETERS;
            break;
        case LengthUnits::MILLIMETERS:
            lengthOut = lengthMillimeters;
            break;
    }
    
    return lengthOut;
}

