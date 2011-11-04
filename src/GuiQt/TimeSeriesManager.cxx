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

#include "TimeSeriesManager.h"
using namespace caret;
TimeSeriesManager::TimeSeriesManager(int32_t &index, ConnectivityLoaderManager *clm)
{
    m_index = index;
    m_clm = clm;
    m_helper = new AnimationHelper(index,clm);
    m_isPlaying = false;
}

void TimeSeriesManager::toggleAnimation()
{
    if(m_isPlaying) pause();
    else play();
}

TimeSeriesManager::~TimeSeriesManager()
{
    stop();
    m_helper->terminate();
    delete m_helper;
}

void TimeSeriesManager::play()
{
    if(m_isPlaying) return;
    m_isPlaying = true;
    m_helper->play();
}
void TimeSeriesManager::stop()
{
    if(!m_isPlaying) return;
    m_isPlaying = false;
    m_helper->stop();
}

void TimeSeriesManager::pause()
{
    if(!m_isPlaying) return;
    m_isPlaying = false;
    m_helper->pause();
}



AnimationHelper::AnimationHelper(int32_t &index, ConnectivityLoaderManager *clm)
{
    m_index = index;
    m_clm = clm;
    m_timeIndex = 0;
    m_updateInterval = 100;
    m_stopThread = false;
    moveToThread(this);
}

void AnimationHelper::run()
{
    //moveToThread(this);
    ConnectivityLoaderFile *clf = m_clm->getConnectivityLoaderFile(m_index);
    if(!clf) return;
    int64_t timePoints = clf->getNumberOfTimePoints();
    for(;m_timeIndex<timePoints;m_timeIndex++)
    {
        if(m_stopThread)
        {
            m_stopThread = false;
            return;
        }
        clf->loadTimePointAtTime(m_timeIndex);
        this->wait(m_updateInterval);
    }
    m_timeIndex = 0;//reset so that user can play again
}

void AnimationHelper::play()
{
    this->start();
}

void AnimationHelper::pause()
{
    m_stopThread = true;
}

void AnimationHelper::stop()
{
    m_stopThread = true;
    m_timeIndex = 0;
}
