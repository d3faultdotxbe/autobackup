#ifndef RPCGENERATOR_H
#define RPCGENERATOR_H

#include <QObject>
#include <QSet>
#include <QHash>
#include <QSetIterator>
#include <QHashIterator>
#include <QDir>

#include "api.h"

#define TemplateBeforeAndAfter_Type QString /*before*/, QString /*after */
typedef QHash<TemplateBeforeAndAfter_Type> TemplateBeforeAndAfterStrings_Type;
typedef QHashIterator<TemplateBeforeAndAfter_Type> TemplateBeforeAndAfterStringsIterator;

struct GeneratedFile
{
    GeneratedFile(QString templateFileContents, QString generatedFileName, TemplateBeforeAndAfterStrings_Type templateBeforeAndAfterStrings = TemplateBeforeAndAfterStrings_Type())
        : TemplateFileContents(templateFileContents)
        , GeneratedFilePath(generatedFileName)
        , GeneratedFileContents(templateFileContents)
        , TemplateBeforeAndAfterStrings(templateBeforeAndAfterStrings)
    { }
    QString TemplateFileContents;
    QString GeneratedFilePath;
    QString GeneratedFileContents;
    TemplateBeforeAndAfterStrings_Type TemplateBeforeAndAfterStrings;

    void replaceTemplateBeforesWithAfters();
};
inline bool operator==(const GeneratedFile &g1, const GeneratedFile &g2)
{
    return (g1.GeneratedFilePath == g2.GeneratedFilePath);
}
inline uint qHash(const GeneratedFile &key, uint seed)
{
    return qHash(key.GeneratedFilePath, seed);
}

typedef GeneratedFile FileToWriteType;
typedef QSet<FileToWriteType> FilesToWriteType;
typedef QSetIterator<FileToWriteType> FilesToWriteIterator;

class RpcGenerator : public QObject
{
    Q_OBJECT
public:
    explicit RpcGenerator(QObject *parent = 0);
private:
    static QString frontLetterToLower(const QString &stringInput);
    static QString frontLetterToUpper(const QString &stringInput);
    static QString classHeaderFileName(QString apiName);
    static QString classSourceFileName(QString apiName);
    static QString apiCallToRequestBaseName(ApiCall apiCall);
    static QString apiCallToRequestInterfaceTypeName(ApiCall apiCall);
    static QString apiCallToRequestInterfaceHeaderInclude(ApiCall apiCall);
    static QString apiCallToForwardDefinitionRawCpp(ApiCall apiCall);
    static QString apiCallArgNamesToCommaSeparatedList(ApiCall apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter = true, bool emitTypes = false, bool emitNames = false);
    static QString apiCallToMethodCppSignature(ApiCall apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter = true);
    static QString apiCallToRawCppDeclaration(ApiCall apiCall, bool requestIfTrue);
    static QString apiCallArgToCpp(ApiCallArg apiCallArg, bool emitTypes = true, bool emitNames = false);
    static QString apiCallToApiDefinitionRawCpp(ApiCall apiCall, bool requestIfTrue);
    static QString apiCallToApiCallRequestMethodDeclarationInSessionHeader(ApiCall apiCall, const QString &boostOrQtCallbackArgs, bool includeApiNamePrefix = false);
    static QString apiCallToApiCallRequestMethodDefinitionInSessionSource(ApiCall apiCall, bool trueIfQt_falseIfWt);
    static QString apiCallToConstructorInitializationCpp(ApiCall apiCall);
    static QString apiCallRequestArgsToRequestInterfaceHeaderDefinitions(ApiCall apiCall);
    static QString apiCallRequestInterfaceArgMemberNamesWithLeadingCommaspaceIfAndArgs(ApiCall apiCall);

    //Qt Business
    GeneratedFile generateApiHeaderFile(Api api, QDir outputDir);
    GeneratedFile generateApiSourceFile(Api api, QDir outputDir);

    //API Request Interface
    GeneratedFile generateApiRequestInterface(Api api, QDir outputDir);

    //API Request Interfaces
    GeneratedFile generateApiCallRequestInterfaceHeader(ApiCall apiCall, QDir outputDir);
    GeneratedFile generateApiCallRequestInterfaceSource(ApiCall apiCall, QDir outputDir);

    //Qt API Requests
    GeneratedFile generateApiCallRequestFromQtHeader(ApiCall apiCall, QDir outputDir);
    GeneratedFile generateApiCallRequestFromQtSource(ApiCall apiCall, QDir outputDir);

    //Wt API Requests
    GeneratedFile generateApiCallRequestFromWtHeader(ApiCall apiCall, QDir outputDir);
    GeneratedFile generateApiCallRequestFromWtSource(ApiCall apiCall, QDir outputDir);

    //Session
    GeneratedFile generateApiSessionHeaderFile(Api api, QDir outputDir);
    GeneratedFile generateApiSessionSourceFile(Api api, QDir outputDir);
    //New Session Request Interface
    GeneratedFile generateNewSessionRequestInterface(Api api, QDir outputDir);
    //New Session Request
    GeneratedFile generateNewSessionRequestFromHeaderFile(Api api, QDir outputDir, const QString &qtOrWtFileNameSuffix);
    GeneratedFile generateNewSessionRequestFromSourceFile(Api api, QDir outputDir, const QString &qtOrWtFileNameSuffix);

    TemplateBeforeAndAfterStrings_Type initialBeforeAndAfterStrings(Api api);
    void appendApiCallBeforeAndAfterStrings(TemplateBeforeAndAfterStrings_Type *beforeAndAfterStrings, ApiCall apiCall);
    void writeApiCallFiles(FilesToWriteType *filesToWrite, ApiCall apiCall, QDir outputDir);

    bool generateRpcActual(Api api, QString outputPath);
    QString fileToString(QString filePath);
    bool writeFiles(FilesToWriteType filesToWrite);
    bool writeFile(QString filePath, QString fileContents);
signals:
    void o(QString);
    void e(QString);
    void rpcGenerated(bool success = false, const QString &directoryFilesWereGeneratedInto = QString());
public slots:
    void generateRpc();
};

#endif // RPCGENERATOR_H
