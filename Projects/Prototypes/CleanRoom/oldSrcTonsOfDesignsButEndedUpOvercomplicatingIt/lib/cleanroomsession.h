#ifndef CLEANROOMSESSION_H
#define CLEANROOMSESSION_H

#include <QObject>

class CleanRoomSession : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomSession(QObject *parent = 0);
    void requestDefaultView();
};

#endif // CLEANROOMSESSION_H
