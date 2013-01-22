#ifndef __TIME_LINE__
#define __TIME_LINE__
/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
/*LICENSE_END*/
#include "stdint.h"
#include "AString.h"
#include "vector"
#include "TimeLine.h"
#include "ColorManager.h"
#include "StructureEnum.h"
#include "QVector"
namespace caret {
enum TimeLineType {
    NODE,
    AVERAGE
};
class TimeLine {
public:
    TimeLine();
    TimeLine(uint64_t nodeidIn, double *pointIn,QVector<double> &xIn, QVector<double>&yIn,void * idIn = NULL);
    virtual ~TimeLine();
    uint64_t nodeid;
    double point[3];
    QVector<double> x;
    QVector<double> y;
    ColorId colorID;
    void *id;//pointer to originating clf that helps us track where timeline came from
    AString filename;
    int64_t clmID; //the timelines number in the connectivity loader manager
    double timeStep;
    TimeLineType type;
    int64_t surfaceNumberOfNodes;
    StructureEnum::Enum structure;
    AString structureName;
    AString parcelName;
    AString borderName;
    AString borderClassName;
    AString label;//label or ROI descriptor for timelines consisting of an average of nodes
};
}
#endif//__TIME_LINE__


