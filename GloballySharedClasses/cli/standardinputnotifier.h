#ifndef STANDARDINPUTNOTIFIER_H
#define STANDARDINPUTNOTIFIER_H

#include <QObject>
#include <QTextStream>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <QTimer>
#else
#include <QSocketNotifier>
#endif

class StandardInputNotifier : public QObject
{
    Q_OBJECT
public:
    explicit StandardInputNotifier(QObject *parent = 0, int msecTimeoutForPollingStandardInput_WINDOWS_ONLY = 500);
private:
QTextStream m_StandardInputTextStream;

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    QTimer *m_StandardInputPollingTimerLoLWindows;
#else
    QSocketNotifier *m_StandardInputSocketNotifierWootEventDriven;
#endif

signals:
    void standardInputReceivedLine(const QString &);
public slots:
    //the user can just disconnect the signal (but actually this slot does still make rapid toggling easier, not that i need it :-P): void stopNotifyingOnStandardInput();
    void readStandardInAndEmitSignalIfNotEmpty();
};

#endif // STANDARDINPUTNOTIFIER_H