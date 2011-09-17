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

#include <vector>
#include "AString.h"

namespace caret {
   
   class LevelProgress;
   //NOTE: this tries to intelligently avoid doing recursive progress updates when the value doesn't change much
   class ProgressObject
   {
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
      ProgressObject(float weight);
      ~ProgressObject();
      void forceFinish();//call this after the algorithm returns to ensure it finishes (in case it ignores the object)
      ProgressObject* addAlgorithm(float weight);
      LevelProgress startLevel(const float finishedProgress = 1.0);
      void algorithmStartSentinel();//used in the constructor of 
      float getCurrentProgressFraction();//in the range [0, 1]
      float getCurrentProgressPercent();//in range [0, 100]
      const AString& getTaskDescription();
      //TODO: make something to return the statuses of all in-progress (nonzero curProgress) tasks for the entire tree, for detailed progress info
      //TODO: set up callbacks so progress changes don't have to be polled for
      friend class LevelProgress;//so that LevelProgress can report progress, but nothing else can
   };
   
   class LevelProgress
   {//reports progress on processing done in this level
      float m_maximum;
      float m_lastReported;
      ProgressObject* m_progObjRef;
      LevelProgress();//deny default construction, assignment, because we use the destructor for a specific purpose
      LevelProgress& operator=(const LevelProgress& right);
   public:
      void reportProgress(const float currentTotal);
      void setTask(const AString& taskDescription);//yes, this reaches through the class, but it is better to have both reporting functions on the same object
      ~LevelProgress();//automatically finishes level
      friend class ProgressObject;//so that ProgressObject can create a LevelProgress object, but nothing else can
   };
   
}
#endif //__PROGRESS_OBJECT_H__
