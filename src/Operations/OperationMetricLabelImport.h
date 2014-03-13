#ifndef __OPERATION_METRIC_LABEL_IMPORT_H__
#define __OPERATION_METRIC_LABEL_IMPORT_H__

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

#include "AbstractOperation.h"

#include <map>
#include <set>

namespace caret {
    
    class GiftiLabelTable;
    
    class OperationMetricLabelImport : public AbstractOperation
    {
        static void translateLabels(const float* valuesIn, int32_t* labelsOut, const int& numNodes, GiftiLabelTable& myTable, std::map<int32_t, int32_t>& translate,
                             std::set<int32_t>& usedValues, const bool& dropUnused, const bool& discardOthers, const int32_t& unusedLabel);
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationMetricLabelImport> AutoOperationMetricLabelImport;

}

#endif //__OPERATION_METRIC_LABEL_IMPORT_H__
