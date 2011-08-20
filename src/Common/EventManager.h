#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include <vector>
#include <stdint.h>
#include "CaretAssert.h"
//#include "assert.h"

namespace caret {

class EventManager
{
public:
   enum Event
   {
      updateGUI,
      drawGLAreas
   };
   class Listener
   {
      Event m_type;
      void (*m_callback)();
   public:
      inline Listener() { m_type = (Event)0; m_callback = NULL; };
      inline Listener(Event type, void (*callback)()) {
         m_type = type;
         m_callback = callback;
      };
      inline virtual void doCallback() {
         CaretAssert(m_callback != NULL);
         //assert(m_callback != NULL);
         (*m_callback)();
      };
      inline virtual Event getEvent() { return m_type; };
      inline virtual bool operator==(const Listener& rhs)
      {
         return (m_type == rhs.m_type) && (m_callback == rhs.m_callback);
      };
   };
private:
   std::vector<std::vector<Listener> > m_listenerList;
public:
   inline void emitEvent(Event thisEvent) {
      if ((uint64_t)thisEvent >= m_listenerList.size() || thisEvent < 0) return;
      uint64_t mysize = m_listenerList[thisEvent].size();
      for (uint64_t i = 0; i < mysize; ++i)
      {
         m_listenerList[thisEvent][i].doCallback();
      }
   };
   inline void registerListener(Listener thisListener)
   {
      Event thisEvent = thisListener.getEvent();
      if (thisEvent < 0) return;
      if (thisEvent >= (int64_t)m_listenerList.size())
      {
         m_listenerList.resize((uint64_t)thisEvent + 1);
      }
      m_listenerList[thisEvent].push_back(thisListener);
   };
   inline void unregisterListener(Listener thisListener)
   {
      Event thisEvent = thisListener.getEvent();
      if (thisEvent >= (int64_t)m_listenerList.size() || thisEvent < 0) return;
      int64_t mysize = (int64_t)m_listenerList[thisEvent].size();
      std::vector<Listener> tempList;
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

#endif // __EVENT_MANAGER_H__
