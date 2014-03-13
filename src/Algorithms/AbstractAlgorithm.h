#ifndef __ABSTRACT_ALGORITHM_H__
#define __ABSTRACT_ALGORITHM_H__

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

//make it easy to use these in an algorithm class, don't just forward declare them
#include "ProgressObject.h"
#include "CaretAssert.h"
#include "OperationParameters.h"
#include "AbstractOperation.h"

namespace caret {

    ///the constructor for algorithms does the processing, because constructor/execute cycles don't make sense for something this simple
    class AbstractAlgorithm : public AbstractOperation
    {
        ProgressObject* m_progObj;//so that the destructor can make sure the bar finishes
        bool m_finish;
        AbstractAlgorithm();//prevent default construction
    protected:
        ///override this with the weights of the algorithms this algorithm will call
        static float getSubAlgorithmWeight();//protected so that people don't try to use them to set algorithm weights in progress objects
        ///override this with the amount of work the algorithm does internally, outside of calls to other algorithms
        static float getAlgorithmInternalWeight();
        AbstractAlgorithm(ProgressObject* myProgressObject);
        virtual ~AbstractAlgorithm();
    public:
        ///use this to set the weight parameter of a ProgressObject
        static float getAlgorithmWeight();
    };

}
#endif //__ABSTRACT_ALGORITHM_H__
