%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMWT_H
#define %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMWT_H

%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%

#include <boost/function.hpp>

class I%API_NAME%;

class %API_CALL_TO_REQUEST_BASE_NAME%FromWt : public I%API_CALL_TO_REQUEST_BASE_NAME%
{
public:
    %API_CALL_TO_REQUEST_BASE_NAME%FromWt(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (bool internalError, bool %API_CALL_SLOT_NAME%Success%COMMASPACE_IF_RESPONSE_HAS_ANY_ARGS%%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_VARNAMES%)> wApplicationCallback%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%);
    void respond(bool internalError, bool %API_CALL_SLOT_NAME%Success%COMMASPACE_IF_RESPONSE_HAS_ANY_ARGS%%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%);
private:
    std::string m_WtSessionId;
    boost::function<void (bool internalError, bool %API_CALL_SLOT_NAME%Success%COMMASPACE_IF_RESPONSE_HAS_ANY_ARGS%%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_VARNAMES%)> m_WApplicationCallback;
};

#endif // %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMWT_H
