#ifndef LOCALAPPBANKCACHE_H
#define LOCALAPPBANKCACHE_H

#include <QObject>

#include "localbankdb.h"
#include "remotebankserverhelper.h"

class LocalAppBankCache : public QObject
{
    Q_OBJECT
public:
    explicit LocalAppBankCache(QObject *parent = 0);
private:
    RemoteBankServerHelper *m_BankServer; //helper, encapsulates ssl + protocol etc
    LocalBankDb m_Db; //TODO: for now, in-memory works. but a future version of this prototype should use an sql db to be persistent across exe sessions.

    void userNeedsNewBitcoinAddFundsKey(QString user);
signals:
    void userAdded(QString newUser);
    //void addFundsKeyReceived(QString user, QString newKey);
    void d(const QString &);
public slots:
    void addUser(QString userNameToAdd);
    void addFundsRequest(QString user);

    void handleUserAdded(QString newUser);
    void handleAddFundsKeyReceived(QString user, QString newKey);
    void init();
};

#endif // LOCALAPPBANKCACHE_H