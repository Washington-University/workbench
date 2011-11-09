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
#include "ConnectivityLoaderFile.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQSpinBoxGroup.h"
#include "QCoreApplication"
#include "Brain.h"
#include "GuiManager.h"
#include "QSpinBox"
using namespace caret;
TimeSeriesManager::TimeSeriesManager(int32_t &index, ConnectivityLoaderControl *clc)
{
    m_index = index;
    m_clc = clc;
    m_helper = new AnimationHelper(index,clc);
    m_isPlaying = false;
}

void TimeSeriesManager::toggleAnimation()
{
    m_helper->toggle();
}

TimeSeriesManager::~TimeSeriesManager()
{
    stop();    
    delete m_helper;
}

void TimeSeriesManager::play()
{
    m_helper->play();
    //m_helper->doAnimation();
}
void TimeSeriesManager::stop()
{
    m_helper->stop();
}

void TimeSeriesManager::pause()
{
    m_helper->pause();
}



AnimationHelper::AnimationHelper(int32_t &index, ConnectivityLoaderControl *clc)
{
    m_index = index;
    m_clc = clc;
    m_timeIndex = 0;
    m_updateInterval = 200;
    m_stopThread = false;

    ConnectivityLoaderManager *clm = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    ConnectivityLoaderFile *clf = clm->getConnectivityLoaderFile(m_index);
    if(!clf) return;
    m_timePoints = clf->getNumberOfTimePoints();
    m_timeStep  = clf->getTimeStep();
    m_spinBox = clc->getTimeSpinBox(index);



    QObject::connect(this, SIGNAL(doubleSpinBoxValueChanged(const double)),
                     m_spinBox, SLOT(setValue(double)), Qt::QueuedConnection);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()), Qt::DirectConnection);
    moveToThread(this);
}

AnimationHelper::~AnimationHelper()
{
    stop();
    delete m_timer;
}

void AnimationHelper::getCurrentTime()
{
    ConnectivityLoaderManager *clm = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    ConnectivityLoaderFile *clf = clm->getConnectivityLoaderFile(m_index);
    if(!clf) return;
    m_timePoints = clf->getNumberOfTimePoints();
    m_timeStep  = clf->getTimeStep();
    QDoubleSpinBox* spinBox = m_clc->getTimeSpinBox(m_index);
    m_timeIndex = spinBox->value()/m_timeStep;
    if(m_timePoints<=m_timeIndex) m_timeIndex = 0;
}

void AnimationHelper::run()
{
    getCurrentTime();
    m_timer->start(m_updateInterval);
    exec();

}

void AnimationHelper::update()
{

    if(m_timeIndex<m_timePoints&&!(this->m_stopThread))
    {
        emit doubleSpinBoxValueChanged((double)m_timeIndex*m_timeStep);
        m_timeIndex++;
        //m_spinBox->thread()->wait(m_updateInterval-10);
    }
    else {
        m_timer->stop();
        exit();
    }
}

void AnimationHelper::play()
{
    this->m_stopThread = false;
    this->start();
}

void AnimationHelper::pause()
{
    if(m_timer->isActive()) {
        this->m_stopThread = true;
    }
}

void AnimationHelper::stop()
{
    pause();
}

void AnimationHelper::toggle()
{
    if(m_timer->isActive())
        stop();
    else play();
}

