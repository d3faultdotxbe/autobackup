#ifndef RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMATCLI_H
#define RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMATCLI_H

#include <QObject>
#include <QTextStream>

class RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;
    bool m_Quitting;

    void cliUsage();
signals:
    void exitRequested(int exitCode);
private slots:
    void handleE(const QString &msg);
    void handleDoneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool success);
};

#endif // RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMATCLI_H
