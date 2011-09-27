#ifndef __ABSTRACT_ALGORITHM_H__
#define __ABSTRACT_ALGORITHM_H__

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

//make it easy to use the progress object, don't just forward declare
#include "ProgressObject.h"

namespace caret {

   ///the constructor for algorithms does the processing, because constructor/execute cycles don't make sense for something this simple
   class AbstractAlgorithm
   {
      ProgressObject* m_progObj;
      AbstractAlgorithm();//prevent default construction
      static float getAlgorithmInternalWeight();//override these to make progress bars smooth
      static float getSubAlgorithmWeight();//private so that people don't try to use them to set progress weights
   protected:
      AbstractAlgorithm(ProgressObject* myProgressObject);
      virtual ~AbstractAlgorithm();
   public:
      static float getAlgorithmWeight();
   };

}
#endif //__ABSTRACT_ALGORITHM_H__
