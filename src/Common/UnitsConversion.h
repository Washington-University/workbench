#ifndef __UNITS_CONVERSION_H__
#define __UNITS_CONVERSION_H__

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



#include <memory>





namespace caret {

    class UnitsConversion {
        
    public:
        enum class LengthUnits {
            CENTIMETERS,
            INCHES,
            METERS,
            MILLIMETERS
        };
        
        virtual ~UnitsConversion();
        
        UnitsConversion(const UnitsConversion&) = delete;

        UnitsConversion& operator=(const UnitsConversion&) = delete;

        static float convertLength(const LengthUnits fromUnits,
                                   const LengthUnits toUnits,
                                   const float value);

        static const float CENTIMETERS_TO_MILLIMETERS;
        static const float INCHES_TO_MILLIMETERS;
        static const float METERS_TO_MILLIMETERS;
        
        // ADD_NEW_METHODS_HERE

    private:
        UnitsConversion();
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __UNITS_CONVERSION_DECLARE__
    const float UnitsConversion::CENTIMETERS_TO_MILLIMETERS = 10.0f;
    const float UnitsConversion::INCHES_TO_MILLIMETERS = 25.4000508f;
    const float UnitsConversion::METERS_TO_MILLIMETERS = 1000;
    

#endif // __UNITS_CONVERSION_DECLARE__

} // namespace
#endif  //__UNITS_CONVERSION_H__
