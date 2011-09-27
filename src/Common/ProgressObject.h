#ifndef __PROGRESS_OBJECT_H__
#define __PROGRESS_OBJECT_H__

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

#include "stdint.h"
#include <vector>
#include "AString.h"

namespace caret {
   
   class LevelProgress;
   //NOTE: this tries to intelligently avoid doing recursive progress updates when the value doesn't change much
   class ProgressObject
   {
//don't always report progress, in case someone uses this in an inner loop
      const static float MAX_CHILD_RESOLUTION;
      const static float MAX_INTERNAL_RESOLUTION;

      struct ProgressInfo
      {
         ProgressObject* progObjRef;//used to clean up the memory on finish()
         float weight;
         float curProgress;
         bool completed;
      };
      std::vector<ProgressInfo> m_children;
      float m_totalWeight;
      float m_nonChildWeight;
      float m_nonChildProgress;
      float m_currentProgress;
      float m_lastReported;
      float m_childResolution;
      AString m_description;
      ProgressObject* m_parent;
      int32_t m_parentIndex;//which index in parent's vector this object is
      bool m_sentinelPassed;
      bool m_disabled;//disables itself if sentinel called twice
      bool m_finished;
      void updateProgress();//used by LevelProgress to report changes
      void finishLevel();//moves this progress object to 100%, then updates parent if not NULL
      ProgressObject();
   public:
      ///fill in weight with the ...Algorithm::getAlgorithmWeight() function at the root level (before starting the algorithm)
      ///if using multiple algorithms at the root level (shame!), sum their weights first, then use addAlgorithm to get objects for each
      ProgressObject(const float weight, const float childResolution = MAX_CHILD_RESOLUTION);
      ~ProgressObject();
      
      ///call this on progress objects you make after an algorithm returns to ensure it finishes (in case it ignores the object)
      void forceFinish();
      
      ///add an algorithm to this algorithm's progress status, and get a pointer to give to that algorithm
      ///fill in weight with the ...Algorithm::getAlgorithmWeight() function
      ProgressObject* addAlgorithm(const float weight, const float childResolution = MAX_CHILD_RESOLUTION);
      
      ///call this inside an algorithm AFTER ALL addAlgorithm CALLS, to correctly activate the progress bar
      LevelProgress startLevel(const float finishedProgress = 1.0, const float internalWeight = 1.0, const float internalResolution = MAX_INTERNAL_RESOLUTION);
      
      ///DO NOT USE: used by AbstractAlgorithm constructor to check for algorithms that ignore the object
      void algorithmStartSentinel();
      
      ///get progress as a fraction of 1 (in range [0, 1])
      float getCurrentProgressFraction();
      
      ///get progress as percent (in range [0, 100])
      float getCurrentProgressPercent();
      
      ///get the description of the current task
      const AString& getTaskDescription();
      
      ///true if algorithmStartSentinel disabled the object
      bool isDisabled();
      //TODO: make something to return the statuses of all in-progress (nonzero curProgress) tasks for the entire tree, for detailed progress info
      //TODO: set up callbacks so progress changes don't have to be polled for
      friend class LevelProgress;//so that LevelProgress can report progress, but nothing else can
   };
   
   class LevelProgress
   {//reports progress on processing done in this level
      float m_maximum;
      float m_lastReported;
      float m_internalResolution;
      ProgressObject* m_progObjRef;
      LevelProgress();//deny default construction, assignment, because we use the destructor for a specific purpose
      LevelProgress& operator=(const LevelProgress& right);
   public:
      
      ///call with the fraction of finishedProgress passed to ProgressObject::startLevel (default 1.0) that this algorithm has done internally
      ///work done by subalgorithms is automatically added and updated as progress is made, DO NOT call this unless the current algorithm does direct processing
      void reportProgress(const float currentTotal);
      
      ///set a description for current task, like the name of the subalgorithm you are about to call
      void setTask(const AString& taskDescription);//yes, this reaches through the class, but it is better to have both reporting functions on the same object
      ~LevelProgress();//automatically finishes level
      friend class ProgressObject;//so that ProgressObject can create a LevelProgress object, but nothing else can
   };
   
}
#endif //__PROGRESS_OBJECT_H__
