#ifndef I%API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUEST_H
#define I%API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUEST_H

#include <QObject>
#include "i%API_NAME_LOWERCASE%request.h"

#include "%API_NAME_LOWERCASE%.h"

class I%API_CALL_TO_REQUEST_BASE_NAME% : public QObject, public I%API_NAME%Request
{
    Q_OBJECT
public:
    I%API_CALL_TO_REQUEST_BASE_NAME%(%API_NAME% *%API_AS_VARIABLE_NAME%%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%, QObject *parent = 0)
        : QObject(parent)%API_CALL_ARG_CONSTRUCTOR_INITIALIZATION_LIST%
    {
        connect(this, &I%API_CALL_TO_REQUEST_BASE_NAME%::%API_CALL_TO_REQUEST_BASE_NAME_FRONT_CHAR_TO_LOWER%ed, %API_AS_VARIABLE_NAME%, &%API_NAME%::%API_CALL_SLOT_NAME%);
    }
    void processRequest();
    virtual void respond(%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%)=0;%API_CALL_ARG_MEMBER_DEFINITIONS_IN_REQUEST_INTERFACE_HEADER%
signals:
    void %API_CALL_TO_REQUEST_BASE_NAME_FRONT_CHAR_TO_LOWER%ed(I%API_CALL_TO_REQUEST_BASE_NAME% *request%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%);
};

#endif // I%API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUEST_H