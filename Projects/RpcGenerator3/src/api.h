#ifndef API
#define API

#include <QString>

struct ApiTypeAndVarName
{
    ApiTypeAndVarName(QString apiCallArgType, QString apiCallArgName)
        : ApiTypeAndVarNameType(apiCallArgType)
        , ApiTypeAndVarNameName(apiCallArgName)
    { }
    QString ApiTypeAndVarNameType;
    QString ApiTypeAndVarNameName;
};
class Api;
struct ApiCall
{
    ApiCall(Api *api, const QString &apiCallSlotName, const QList<ApiTypeAndVarName> &requestArgs = QList<ApiTypeAndVarName>(), const QList<ApiTypeAndVarName> &responseArgs = QList<ApiTypeAndVarName>(), QStringList additionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent = QStringList()/*, QStringList additionalIncludeForWheneverThisApiCallsResponseArgsArePresent = QStringList()*/)
        : ParentApi(api)
        , ApiCallSlotName(apiCallSlotName)
        , RequestArgs(requestArgs)
        , ResponseArgs(responseArgs)
        , AdditionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent(additionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent)
        //, AdditionalIncludeForWheneverThisApiCallsResponseArgsArePresent(additionalIncludeForWheneverThisApiCallsResponseArgsArePresent)
    { }
    Api *ParentApi;
    QString ApiCallSlotName;
    QList<ApiTypeAndVarName> RequestArgs;
    QList<ApiTypeAndVarName> ResponseArgs;
    QStringList AdditionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent;
    //QStringList AdditionalIncludeForWheneverThisApiCallsResponseArgsArePresent;
};
struct ApiSessionProperty
{
    ApiSessionProperty(QString propertyType, QString propertyName, QString propertyDefaultValueOrEmptyStringIfToDefaultConstruct = QString())
        : PropertyTypeAndVarName(propertyType, propertyName)
        , PropertyDefaultValueOrEmptyStringIfToDefaultConstruct(propertyDefaultValueOrEmptyStringIfToDefaultConstruct)
    { }
    ApiTypeAndVarName PropertyTypeAndVarName;
    QString PropertyDefaultValueOrEmptyStringIfToDefaultConstruct;
};
struct Api
{
    Api(QString apiName)
        : ApiName(apiName)
    { }
    QString ApiName;
    QList<ApiSessionProperty> ApiAdditionalSessionProperties;
    QList<ApiCall> ApiCalls;

    void addPropertyToSession(QString propertyType, QString propertyName, QString propertyDefaultValueOrEmptyStringIfToDefaultConstruct = QString())
    {
        ApiAdditionalSessionProperties.append(ApiSessionProperty(propertyType, propertyName, propertyDefaultValueOrEmptyStringIfToDefaultConstruct));
    }
    void createApiCall(const QString &apiCallSlotName, const QList<ApiTypeAndVarName> &requestArgs = QList<ApiTypeAndVarName>(), const QList<ApiTypeAndVarName> &responseArgs = QList<ApiTypeAndVarName>(), QStringList additionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent = QStringList()/*, QStringList additionalIncludeForWheneverThisApiCallsResponseArgsArePresent = QStringList()*/)
    {
        ApiCall apiCall(this, apiCallSlotName, requestArgs, responseArgs, additionalIncludeForWheneverThisApiCallsRequestOrResponseArgsArePresent/*, additionalIncludeForWheneverThisApiCallsResponseArgsArePresent*/);
        ApiCalls.append(apiCall);
    }
};

#endif // API

