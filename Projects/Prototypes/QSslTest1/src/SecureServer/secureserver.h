#ifndef SECURESERVER_H
#define SECURESERVER_H

#include <QObject>

#include "ssltcpserver.h"
#include "../shared/protocol.h"

class SecureServer : public QObject
{
    Q_OBJECT
public:
    explicit SecureServer(QObject *parent = 0);
private:
    SslTcpServer *m_SslTcpServer;

    void sendMessageToClient(ServerToClientMessage::TheMessage theMessage, QSslSocket *conn);
    //void sendWelcome(QSslSocket *conn);
signals:
    void d(const QString &);
public slots:
    void startServer();
private slots:
    void handleConnectedButNotEncryptedYet();
    void handleConnectedAndEncrypted(QSslSocket* secureSocket);
    void handleReadyRead();
};

#endif // SECURESERVER_H
