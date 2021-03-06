#ifndef REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H
#define REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H

#include <QObject>

#include "userinterfaceskeletongeneratordata.h"

class RequstsResponseContractsGlueGenerator : public QObject
{
    Q_OBJECT
public:
    explicit RequstsResponseContractsGlueGenerator(QObject *parent = 0);
    bool generateRequstsResponseContractsGlue_AndAddContractSignalsAndSlotsToData(UserInterfaceSkeletonGeneratorData::Data *data, QString targetDir_WithTrailingSlash);
private:
    void addSignalsAndSlotsToData(UserInterfaceSkeletonGeneratorData::Data *data);

    bool generateBusinessObjectRequestResponseContractsHeaderFile(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectRequestResponseContractsSourceFile(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectRequestResponseContractsPriFile(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectSomeSlotRequestResponseHeaderFiles(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectSomeSlotRequestResponseSourceFiles(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectSomeSlotScopedResponderHeaderFiles(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectSomeSlotScopedResponderSourceFiles(const UserInterfaceSkeletonGeneratorData::Data &data, QString targetDir_WithTrailingSlash);

    static QString firstLetterToUpper(const QString &inputString)
    {
        if(inputString.isEmpty())
            return inputString;
        QString ret(inputString);
        ret.replace(0, 1, ret.at(0).toUpper());
        return ret;
    }
    static QString firstLetterToLower(const QString &inputString)
    {
        if(inputString.isEmpty())
            return inputString;
        QString ret(inputString);
        ret.replace(0, 1, ret.at(0).toLower());
        return ret;
    }
    static QString TAB;
signals:
    void e(QString);
    void finishedGeneratingRequestResponseContractGlue(bool success = false); //emit on error only (it used to be my finished signal), and only false
};

#endif // REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H
