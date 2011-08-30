#ifndef __EVENT_MANAGER_TC_H__
#define __EVENT_MANAGER_TC_H__

#include <vector>
#include <stdint.h>
#include "CaretAssert.h"
//#include "assert.h"

namespace caret {

class EventManagerTC
{
public:
   enum EventTC
   {
      updateGUI,
      drawGLAreas
   };
   class ListenerTC
   {
      EventTC m_type;e
      void (*m_callback)();
   public:
      inline ListenerTC() { m_type = (EventTC)0; m_callback = NULL; };
      inline ListenerTC(EventTC type, void (*callback)()) {
         m_type = type;
         m_callback = callback;
      };
      inline virtual void doCallback() {
         CaretAssert(m_callback != NULL);
         //assert(m_callback != NULL);
         (*m_callback)();
      };
      inline virtual EventTC getEvent() { return m_type; };
      inline virtual bool operator==(const ListenerTC& rhs)
      {
         return (m_type == rhs.m_type) && (m_callback == rhs.m_callback);
      };
   };
private:
   std::vector<std::vector<ListenerTC> > m_listenerList;
public:
   inline void emitEvent(EventTC thisEvent) {
      if ((uint64_t)thisEvent >= m_listenerList.size() || thisEvent < 0) return;
      uint64_t mysize = m_listenerList[thisEvent].size();
      for (uint64_t i = 0; i < mysize; ++i)
      {
         m_listenerList[thisEvent][i].doCallback();
      }
   };
   inline void registerListener(ListenerTC thisListener)
   {
      EventTC thisEvent = thisListener.getEvent();
      if (thisEvent < 0) return;
      if (thisEvent >= (int64_t)m_listenerList.size())
      {
         m_listenerList.resize((uint64_t)thisEvent + 1);
      }
      m_listenerList[thisEvent].push_back(thisListener);
   };
   inline void unregisterListener(ListenerTC thisListener)
   {
      EventTC thisEvent = thisListener.getEvent();
      if (thisEvent >= (int64_t)m_listenerList.size() || thisEvent < 0) return;
      int64_t mysize = (int64_t)m_listenerList[thisEvent].size();
      std::vector<ListenerTC> tempList;
      tempList.reserve(mysize);
      for (int64_t i = mysize - 1; i >= 0; --i)
      {
         if (!(m_listenerList[thisEvent][i] == thisListener))
         {
            tempList.push_back(m_listenerList[thisEvent][i]);
         }
      }
      m_listenerList[thisEvent] = tempList;
   };
};

}

#endif // __EVENT_MANAGER_TC_H__
