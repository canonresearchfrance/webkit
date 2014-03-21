
#include "config.h"
#include "NotifyEvent.h"

#include "EventNames.h"

namespace WebCore {

NotifyEvent::NotifyEvent()
{
}

NotifyEvent::NotifyEvent(const String& data)
    : Event(eventNames().notifyEvent, false, false)
    , m_data(data)
{
}

NotifyEvent::~NotifyEvent()
{
}

void NotifyEvent::initNotifyEvent(const AtomicString& type, bool canBubble, bool cancelable, const String& data)
{
    initEvent(type, canBubble, cancelable);

    m_data = data;
}

} // namespace WebCore
