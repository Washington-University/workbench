#ifndef TIMESERIESMANAGER_H
#define TIMESERIESMANAGER_H
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
#include "QThread"
#include "ConnectivityLoaderManager.h"
#include "ConnectivityLoaderFile.h"
namespace caret {

class AnimationHelper/*: public QThread*/ {
public:
    AnimationHelper(int32_t &index, ConnectivityLoaderManager *clm);
    void run();
    void play();
    void pause();
    void stop();
    void doAnimation();
private:
    int32_t m_index; //index for connectivity loader file
    ConnectivityLoaderManager *m_clm;
    int64_t m_timeIndex;
    int64_t m_updateInterval;
    bool m_stopThread;
};

class TimeSeriesManager
{
public:
    TimeSeriesManager(int32_t &index, ConnectivityLoaderManager *clm);
    virtual ~TimeSeriesManager();
    void play();
    void pause();
    void stop();
    void toggleAnimation();
private:
    AnimationHelper *m_helper;
    bool m_isPlaying;
    int32_t m_index;
    ConnectivityLoaderManager *m_clm;
};


}
#endif // TIMESERIESMANAGER_H
