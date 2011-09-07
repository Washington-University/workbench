#ifndef __CARET_CONTENTS_H__
#define __CARET_CONTENTS_H__

/*
 * Copyright 1995-2009 Washington University School of Medicine.
 *
 * http://brainmap.wustl.edu
 *
 * CARET is free software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

namespace caret {

    /// Constants used in Caret
    class CaretConstants {

    private:
        CaretConstants();
        
        ~CaretConstants();
        
        CaretConstants(const CaretConstants&);
        
        CaretConstants& operator=(const CaretConstants*);

    public:
        /// maximum number of browser tabs
        static const int32_t MAXIMUM_NUMBER_OF_BROWSER_TABS;
        
    };

#ifdef __CARET_CONTENTS_DECLARE_H__
    const int32_t CaretConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS = 50;
#endif // __CARET_CONTENTS_DECLARE_H__
    
}  // namespace

#endif // __CARET_CONTENTS_H__
