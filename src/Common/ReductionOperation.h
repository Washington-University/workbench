#ifndef __REDUCTION_OPERATION_H__
#define __REDUCTION_OPERATION_H__

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

#include "AString.h"
#include "ReductionEnum.h"

namespace caret {
    
    class ReductionOperation
    {
    public:
        static float reduce(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type);
        ///reduce, with exclusion based on number of standard deviations
        static float reduceExcludeDev(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type, const float& numDevBelow, const float& numDevAbove);
        static AString getHelpInfo();
    };
    
}

#endif //__REDUCTION_OPERATION_H__
