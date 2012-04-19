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
#include "ConnectivityLoaderManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQSpinBoxGroup.h"
#include "QCoreApplication"
#include "Brain.h"
#include "GuiManager.h"
#include "QSpinBox"
#include "SessionManager.h"
#include "CaretPreferences.h"
using namespace caret;
TimeSeriesManager::TimeSeriesManager(int32_t &index, ConnectivityLoaderControl *clc)
{
    m_index = index;
    m_clc = clc;    
    m_isPlaying = false;
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(true).getPointer());
    
    m_timeIndex = 0;
    m_updateInterval = 500; //200;
    m_stopThread = false;

    ConnectivityLoaderManager *clm = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    ConnectivityLoaderFile *clf = clm->getConnectivityLoaderFile(m_index);
    if(!clf) return;
    m_timePoints = clf->getNumberOfTimePoints();
    m_timeStep  = clf->getTimeStep();
    m_spinBox = clc->getTimeSpinBox(index);
    CaretPreferences *prefs = SessionManager::get()->getCaretPreferences();
    
    double time;
    prefs->getAnimationStartTime( time );
    this->setAnimationStartTime(time);



    QObject::connect(this, SIGNAL(doubleSpinBoxValueChanged(const double)),
                     m_spinBox, SLOT(setValue(double)), Qt::QueuedConnection);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()), Qt::DirectConnection);
    moveToThread(this);
}

TimeSeriesManager::~TimeSeriesManager()
{
    stop();
    delete m_timer;
}

void TimeSeriesManager::getCurrentTime()
{
    ConnectivityLoaderManager *clm = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    ConnectivityLoaderFile *clf = clm->getConnectivityLoaderFile(m_index);
    if(!clf) return;
    m_timePoints = clf->getNumberOfTimePoints();
    m_timeStep  = clf->getTimeStep();
    QDoubleSpinBox* spinBox = m_clc->getTimeSpinBox(m_index);
    m_timeIndex = (spinBox->value()-m_startTime)/m_timeStep;
    if(m_timePoints<=m_timeIndex) m_timeIndex = 0;
}

void TimeSeriesManager::run()
{
    getCurrentTime();
    m_timer->start(m_updateInterval);
    exec();

}

void TimeSeriesManager::update()
{
    if(m_timeIndex<m_timePoints&&!(this->m_stopThread))
    {
        emit doubleSpinBoxValueChanged((double)(m_timeIndex*m_timeStep)+m_startTime);
        m_timeIndex++;
        //m_spinBox->thread()->wait(m_updateInterval-10);
    }
    else {
        m_timer->stop();
        exit();
    }
}

void TimeSeriesManager::play()
{
    this->m_stopThread = false;
    this->start();
}

void TimeSeriesManager::stop()
{
    if(m_timer->isActive()) {
        this->m_stopThread = true;
    }
}



void TimeSeriesManager::toggleAnimation()
{
    if(m_timer->isActive())
        stop();
    else play();
}

void TimeSeriesManager::setAnimationStartTime ( const double& time )
{
   this->m_startTime = time;
   QDoubleSpinBox* spinBox = m_clc->getTimeSpinBox(m_index);
   double currentTime = spinBox->value();
   if(currentTime < m_startTime)
   {
      currentTime = time;
      spinBox->setValue(currentTime);
   }
   //next, set qspinbox start and end extents
   spinBox->setMinimum(time);
   spinBox->setMaximum(time+m_timePoints*this->m_timeStep);
   m_timeIndex = (currentTime-m_startTime)/m_timeStep;
   
   
}


