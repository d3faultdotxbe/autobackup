%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef I%API_NAME_UPPERCASE%_H
#define I%API_NAME_UPPERCASE%_H

#include <QObject>

%API_CALL_REQUEST_HEADER_INCLUDES%

class I%API_NAME%NewSessionRequest;

class I%API_NAME% : public QObject
{
    Q_OBJECT
public:
    explicit I%API_NAME%(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();
    void newSession(I%API_NAME%NewSessionRequest *request);

%API_CALLS_PURE_VIRTUAL_DECLARATIONS% //TODOreq: needs to be pure virtual declarations here
};

#endif // I%API_NAME_UPPERCASE%_H
