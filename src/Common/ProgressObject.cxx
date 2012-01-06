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

#include "ProgressObject.h"
#include "CaretAssert.h"
#include "EventProgressUpdate.h"
#include "EventManager.h"

using namespace std;
using namespace caret;

///decrease these values to reduce progress bar overhead everywhere
///or, manually set them for any fast algorithm that calls reportProgress a lot (or change it so it calls reportProgress less often)
const float ProgressObject::MAX_CHILD_RESOLUTION = 0.01f;//up to 100 calls per child algorithm
const float ProgressObject::MAX_INTERNAL_RESOLUTION = 0.001f;//up to 1000 calls during internal processing

ProgressObject* ProgressObject::addAlgorithm(const float weight, const float childResolution)
{
    CaretAssertMessage(weight > 0.0f, "nonpositive weight in ProgressObject::addAlgorithm");
    if (m_disabled) return this;//disabled short circuits everything, can't track progress if an algorithm ignores and forwards the pointer
    ProgressInfo newInfo;
    newInfo.completed = false;
    newInfo.curProgress = 0.0f;
    newInfo.weight = weight;
    newInfo.progObjRef = new ProgressObject(weight, childResolution);
    newInfo.progObjRef->m_parent = this;
    newInfo.progObjRef->m_parentIndex = m_children.size();
    m_children.push_back(newInfo);
    float childWeight = 0.0f;
    vector<ProgressInfo>::iterator myend = m_children.end();
    for (vector<ProgressInfo>::iterator iter = m_children.begin(); iter != myend; ++iter)
    {
        childWeight += iter->weight;
    }
    m_totalWeight = childWeight + m_nonChildWeight;
    return newInfo.progObjRef;
}

void ProgressObject::algorithmStartSentinel()
{
    if (m_sentinelPassed)
    {
        m_disabled = true;//if it hits start twice (passed through an algorithm without interaction), disable it
    } else {
        m_sentinelPassed = true;
    }
}

void ProgressObject::finishLevel()
{
    if (m_finished) return;//don't finish twice
    m_currentProgress = m_totalWeight;
    m_finished = true;
    if (m_parent != NULL)
    {
        m_parent->m_children[m_parentIndex].completed = true;
        m_parent->updateProgress();
    }
    EventProgressUpdate myUpdate(this);
    myUpdate.m_finished = true;
    EventManager::get()->sendEvent(myUpdate.getPointer());
}

void ProgressObject::forceFinish()
{
    finishLevel();//this function is mostly to insert the word "force" so people think twice about using it
}

float ProgressObject::getCurrentProgressFraction()
{
    if (m_totalWeight <= 0.0f) return 0.0f;
    return m_currentProgress / m_totalWeight;
}

float ProgressObject::getCurrentProgressPercent()
{
    return getCurrentProgressFraction() * 100.0f;
}

const AString& ProgressObject::getTaskDescription()
{
    return m_description;
}

ProgressObject::ProgressObject(const float weight, const float childResolution)
{
    m_currentProgress = 0.0f;
    m_disabled = false;
    m_finished = false;
    m_lastReported = 0.0f;
    m_nonChildProgress = 0.0f;
    m_nonChildWeight = weight;
    m_parent = NULL;
    m_sentinelPassed = false;
    m_totalWeight = weight;
    m_childResolution = childResolution;
}

LevelProgress::LevelProgress(ProgressObject* myProgObj, const float finishedProgress, const float internalWeight, const float internalResolution)
{
    CaretAssertMessage(internalWeight > 0.0f, "nonpositive weight in ProgressObject::startLevel");
    m_lastReported = 0.0f;
    m_maximum = finishedProgress;
    m_progObjRef = myProgObj;
    m_internalResolution = internalResolution;
    if (m_progObjRef != NULL)
    {
        m_progObjRef->setInternalWeight(internalWeight);
        EventProgressUpdate myUpdate(myProgObj);
        myUpdate.m_starting = true;
        EventManager::get()->sendEvent(myUpdate.getPointer());
    }
}

void ProgressObject::setInternalWeight(const float& myInternalWeight)
{
    m_nonChildWeight = myInternalWeight;
    float childWeight = 0.0f;
    vector<ProgressInfo>::iterator myend = m_children.end();
    for (vector<ProgressInfo>::iterator iter = m_children.begin(); iter != myend; ++iter)
    {
        childWeight += iter->weight;
    }
    m_totalWeight = childWeight + m_nonChildWeight;
}

void ProgressObject::updateProgress()
{
    if (m_disabled) return;
    if (m_finished)
    {
        return;//nothing to do, finishLevel() should have taken care of everything
    }
    float totalWeightComplete = 0.0f;//recalculating the sum is thread-safe, as long as float assignment is atomic
    vector<ProgressInfo>::iterator myend = m_children.end();
    for (vector<ProgressInfo>::iterator iter = m_children.end(); iter < myend; ++iter)
    {
        if (iter->completed)
        {
            totalWeightComplete += iter->weight;
        } else {
            totalWeightComplete += iter->curProgress * iter->weight;
        }
    }
    if (m_nonChildProgress > 0.0f && m_nonChildWeight > 0.0f)
    {
        totalWeightComplete += m_nonChildProgress * m_nonChildWeight;
    }
    if (totalWeightComplete < m_totalWeight)
    {
        if (totalWeightComplete < 0.0f)
        {
            m_currentProgress = 0.0f;
        } else {
            m_currentProgress = totalWeightComplete / m_totalWeight;
        }
    } else {
        m_currentProgress = 1.0f;
    }
    if (m_parent != NULL)
    {
        m_parent->m_children[m_parentIndex].curProgress = m_currentProgress;
        if (m_currentProgress - m_lastReported > m_childResolution)
        {//don't recurse unless progress has changed more than the resolution specified
            m_lastReported = m_currentProgress;
            m_parent->updateProgress();
        }
    }
    EventProgressUpdate myUpdate(this);//just send the event, LevelProgress should already have checked if the amount of change was significant
    myUpdate.m_amountUpdate = true;
    EventManager::get()->sendEvent(myUpdate.getPointer());
}

bool ProgressObject::isDisabled()
{
    return m_disabled;
}

ProgressObject::~ProgressObject()
{
    finishLevel();//so that things listening for progress events are kept consistent
    vector<ProgressInfo>::iterator myend = m_children.end();
    for (vector<ProgressInfo>::iterator iter = m_children.begin(); iter != myend; ++iter)
    {
        if (iter->progObjRef != NULL)
        {
            delete iter->progObjRef;
            iter->progObjRef = NULL;
        }
    }
}

void LevelProgress::reportProgress(const float currentTotal)
{
    if (m_progObjRef == NULL || m_progObjRef->m_disabled) return;
    float curProgress = currentTotal / m_maximum;
    if (curProgress > 1.0f)
    {
        curProgress = 1.0f;
    }
    if (curProgress < m_lastReported)
    {
        curProgress = m_lastReported;
    }
    m_progObjRef->m_nonChildProgress = curProgress;
    if (curProgress - m_lastReported > m_internalResolution)
    {
        m_lastReported = curProgress;
        m_progObjRef->updateProgress();
    }
}

void LevelProgress::setTask(const AString& taskDescription)
{//maybe this should be in a setter in m_progObjRef, here for coherence with progress reporting
    if (m_progObjRef == NULL) return;
    m_progObjRef->m_description = taskDescription;
    EventProgressUpdate myUpdate(m_progObjRef);
    myUpdate.m_textUpdate = true;
    EventManager::get()->sendEvent(myUpdate.getPointer());
}

LevelProgress::~LevelProgress()
{
    if (m_progObjRef == NULL) return;
    m_progObjRef->finishLevel();//finish level on destruction of the object, for automatic detection of algorithm finishing
}
