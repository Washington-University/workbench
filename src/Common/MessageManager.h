#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <vector>
#include <stdint.h>
#include "CaretAssert.h"

namespace caret {

class Listener;
class Message;
inline bool operator==(const Listener& lhs, const Listener& rhs);
inline bool operator==(const Message& lhs, const Listener& rhs);
inline bool operator==(const Listener& lhs, const Message& rhs);
inline bool operator==(const Message& lhs, const Message& rhs);

enum MessageType
{
    updateGUI,
    drawGLAreas
};

class Message
{
public:
    Message(MessageType etype) : m_etype(etype) {}
    MessageType getMessageType() const { return m_etype; }
    bool HasData() { return false;} //default Messages have no data payload
    //GetData should be implemented and return data for Messages that carry them.
    //SetData should be implemented and be used to SetData on Messages that carry them.

private:
    MessageType m_etype;
};

class Listener
{
    MessageType m_type;
    void (*m_callback)(void);

public:
    Listener() { m_type = (MessageType)0; m_callback = NULL; }
    Listener(MessageType type) {  m_type = type; }
    virtual void doCallback(Message &Message) { if(m_callback) m_callback(); }
    MessageType getMessageType() const { return m_type; }
    void setCallback(void (*callback)(void)) { m_callback = callback; }
};

class MessageManager
{
    std::vector<std::vector<Listener> > m_listenerList;
public:
    void sendMessage(Message &thisMessage) {
        MessageType MessageType = thisMessage.getMessageType();
        if ((uint64_t)MessageType >= m_listenerList.size() || MessageType < 0) return;
        uint64_t mysize = m_listenerList[MessageType].size();
        for (uint64_t i = 0; i < mysize; ++i)
        {
            m_listenerList[MessageType][i].doCallback(thisMessage);
        }
    }
    void registerListener(Listener &thisListener)
    {
        MessageType messageType = thisListener.getMessageType();
        if (messageType < 0) return;
        if (messageType >= (int64_t)m_listenerList.size())
        {
            m_listenerList.resize((uint64_t)messageType + 1);
        }
        m_listenerList[messageType].push_back(thisListener);
    }
    void unregisterListener(Listener &thisListener)
    {
        MessageType messageType = thisListener.getMessageType();
        if (messageType >= (int64_t)m_listenerList.size() || messageType < 0) return;
        int64_t mysize = (int64_t)m_listenerList[messageType].size();
        std::vector<Listener> tempList;
        tempList.reserve(mysize);
        for (int64_t i = mysize - 1; i >= 0; --i)
        {
            if (!(m_listenerList[messageType][i] == thisListener))
            {
                tempList.push_back(m_listenerList[messageType][i]);
            }
        }
        m_listenerList[messageType] = tempList;
    }
};


inline bool operator==(const Listener& lhs, const Listener& rhs){ return (lhs.getMessageType() == rhs.getMessageType()); }
inline bool operator==(const Message& lhs, const Listener& rhs){ return (lhs.getMessageType() == rhs.getMessageType()); }
inline bool operator==(const Listener& lhs, const Message& rhs){ return (lhs.getMessageType() == rhs.getMessageType()); }
inline bool operator==(const Message& lhs, const Message& rhs) { return (lhs.getMessageType() == rhs.getMessageType()); }


}


#endif // MESSAGE_MANAGER_H
