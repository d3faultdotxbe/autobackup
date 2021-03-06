#include "recursivelygpgsignintocustomdetachedsignaturesformatcli.h"

#include <QCoreApplication>

#include "recursivelygpgsignintocustomdetachedsignaturesformat.h"

#define ERRRRRR { cliUsage(); emit exitRequested(1); return; }

RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_Quitting(false)
{
    RecursivelyGpgSignIntoCustomDetachedSignaturesFormat *recursivelyGpgSignIntoCustomDetachedSignaturesFormat = new RecursivelyGpgSignIntoCustomDetachedSignaturesFormat(this);
    connect(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, SIGNAL(doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool)), this, SLOT(handleDoneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool)));
    connect(this, &RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOqt4: just eh a method that does the same

    QStringList argz = qApp->arguments();
    argz.removeFirst(); //app filename

    QStringList excludeEntries;
    int excludeIndex;
    while((excludeIndex = argz.indexOf(RecursiveGpgTools_EXCLUDE_ARG)) > -1)
    {
        argz.removeAt(excludeIndex);
        if(argz.size() < (excludeIndex+1))
            ERRRRRR
        excludeEntries << argz.takeAt(excludeIndex);
    }

    if(argz.isEmpty())
        ERRRRRR
    QString dir = argz.takeFirst();
    QString sigsFile;
    if(!argz.isEmpty())
        sigsFile = argz.takeFirst();
    QMetaObject::invokeMethod(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, "recursivelyGpgSignIntoCustomDetachedSignaturesFormat", Q_ARG(QString,dir), Q_ARG(QString,sigsFile), Q_ARG(QStringList, excludeEntries));
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::cliUsage()
{
    handleE("Usage: RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli dir [sigsFile = stdout]");
    handleE("");
    handleE("If no sigsFile is provided, the sigs are written to stdout");
    handleE(RecursiveGpgTools_EXCLUDE_ARG "\tExclude files. Specified as a path relative to the dir to recursively verify (without a leading './'). You can use this flag multiple times. Note, it only affects files on the filesystem not yet in the sigsfile. It does not suppress the warning 'file x was in sigsfile but not on filesystem'"); //TODOoptional: allow excludings sigs in sigsfile not on fs (to suppress that warning)
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::handleDoneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool success)
{
    if(!m_Quitting)//because this slot might be called more than once xD -- the backend might emit 'done' multiple times in error conditions
    {
        emit exitRequested(success ? 0 : 1);
        m_Quitting = true;
    }
}
