#ifndef WTKEYSETANDGETWIDGET_H
#define WTKEYSETANDGETWIDGET_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WBreak>

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace Wt;
using namespace boost::interprocess;

#ifdef __cplusplus
extern "C" {
#endif
#include "event2/event.h"
#ifdef __cplusplus
} // extern "C"
#endif

class WtKeySetAndGetWidget : public WApplication
{
public:
    WtKeySetAndGetWidget(const WEnvironment& env);
    static message_queue *m_KeyMessageQueue;
    static message_queue *m_ValueMessageQueue;
    static void newAndOpenSetValueByKeyMessageQueues(const char *keyMessageQueueName, const char *valueMessageQueueName);
    static void deleteSetValueByKeyMessageQueues();
    static event *m_SetValueByKeyEvent; //hack? I can't think of any other way to do this yet but I think once I see it working I'll figure out the "proper" design for all this...
private:
    WLineEdit *m_KeyLineEdit;
    WLineEdit *m_ValueLineEdit;
    WContainerWidget *m_Canvas;
    void setValueByKey();
    //void setValueByKeyCallback();

    virtual void finalize();
};

#endif // WTKEYSETANDGETWIDGET_H
