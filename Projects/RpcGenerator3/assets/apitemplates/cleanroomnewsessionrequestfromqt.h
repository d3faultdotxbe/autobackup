%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H
#define %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H

#include "i%API_NAME_LOWERCASE%newsessionrequest.h"

class %API_NAME%NewSessionRequestFromQt : public I%API_NAME%NewSessionRequest
{
    Q_OBJECT
public:
    %API_NAME%NewSessionRequestFromQt(I%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToRespondTo, const char *slotCallback, QObject *parent = 0)
        : I%API_NAME%NewSessionRequest(%API_AS_VARIABLE_NAME%, parent)
        , m_ObjectToRespondTo(objectToRespondTo)
    {
        QObject::connect(this, SIGNAL(newSessionResponseRequested(%API_NAME%Session*)), objectToRespondTo, slotCallback);
    }
    void respond(%API_NAME%Session session);
private:
    QObject *m_ObjectToRespondTo;
signals:
    void newSessionResponseRequested(%API_NAME%Session session);
};

#endif // %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMQT_H
