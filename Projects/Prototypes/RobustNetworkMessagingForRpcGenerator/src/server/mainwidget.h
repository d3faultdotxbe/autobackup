#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "idebuggablestartablestoppablefrontend.h"

#include "robustnetworkmessagingforrpcgeneratortestserver.h"

class mainWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT

public:
    mainWidget(RobustNetworkMessagingForRpcGeneratorTestServer *debuggableStartableStoppableBackend, QWidget *parent = 0);
protected:
    void addButtonsToLayoutAndConnectToBackend();
private:
    QLineEdit *m_MsDelayLineEdit;
    QPushButton *m_StartSendingMessagesButton;
    QPushButton *m_StopSendingMessagesButton;
    QTimer *m_MessageSendTimer;
private slots:
    void toggleSendingMessages();
    void handleMessageSendTimerTimedOut();
};

#endif // MAINWIDGET_H
