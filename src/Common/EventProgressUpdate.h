#ifndef __EVENT_PROGRESS_UPDATE_H__
#define __EVENT_PROGRESS_UPDATE_H__

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


#include "Event.h"


namespace caret {

   class ProgressObject;
   
/// Event for updating the user-interface
   class EventProgressUpdate : public Event {

   public:
      EventProgressUpdate();

      EventProgressUpdate(ProgressObject* myObject);
       
      EventProgressUpdate(const int minimumProgressValue,
                          const int maximumProgressValue,
                          const int progressValue,
                          const QString& progressMessage);
       
      EventProgressUpdate(const QString& progressMessage);
       
      virtual ~EventProgressUpdate();

       void setProgress(const int progressValue,
                        const QString& progressMessage);
       
       void setProgressMessage(const QString& progressMessage);
       
      bool m_textUpdate, m_amountUpdate, m_finished, m_starting;

      ProgressObject* m_whichObject;//idea is for progress elements to check whether their object emitted this event or not, if not, ignore
       
       /** @return Did the user request cancellation of the task */
       bool isCancelled() const { return m_cancelled; }
   
       /** @return Minimum progress value. */
       int getMinimumProgressValue() const { return m_minimumProgressValue; }
       
       /** @return Maximum progress value */
       int getMaximumProgressValue() const { return m_maximumProgressValue; }
       
       /** @return Current value of progress */
       int getProgressValue() const { return m_progressValue; }
       
       /** @return Message displayed describing progress */
       QString getProgressMessage() const { return m_progressMessage; }
       
       /** Request cancellation of the task */
       void setCancelled() { m_cancelled = true; }
       
   private:
      EventProgressUpdate(const EventProgressUpdate&);

      EventProgressUpdate& operator=(const EventProgressUpdate&);

        int m_minimumProgressValue;
        int m_maximumProgressValue;
        int m_progressValue;
        QString m_progressMessage;
       
       bool m_cancelled;
   };

} // namespace

#endif // __EVENT_PROGRESS_UPDATE_H__
