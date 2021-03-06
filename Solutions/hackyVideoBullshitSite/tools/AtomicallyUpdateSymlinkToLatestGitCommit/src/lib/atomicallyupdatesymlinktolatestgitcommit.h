#ifndef ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMIT_H
#define ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMIT_H

#include <QObject>

class QProcess;

class AtomicallyUpdateSymlinkToLatestGitCommit : public QObject
{
    Q_OBJECT
public:
    explicit AtomicallyUpdateSymlinkToLatestGitCommit(QObject *parent = 0);
private:
    QString m_GitPath;
    QString m_TarPath;
    QString m_MvPath;
    QProcess *m_MvProcess;
    bool atomicallyUpdateSymlinkToLatestGitCommitPrivate(const QString &cloneSrcUrl, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation);
    bool gitArchiveAndUntarLatestCommit(const QString &srcRepoUrl, const QString &destArchivePath);
    bool atomicMoveOverwritingDestinationPlx(const QString &srcPath, const QString &destPath);
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
signals:
    void o(const QString &);
    void e(const QString &);
    void symlinkAtomicallyUpdatedToLatestGitCommit(bool successfully); //failed signal as well? or just fail silently? or quitRequested sufficient?
    //void quitRequested(); eh? bool success it is!
public slots:
    void setGitPath(const QString &gitPath);
    void setTarPath(const QString &tarPath);
    void setMvPath(const QString &mvPath);
    void atomicallyUpdateSymlinkToLatestGitCommit(const QString &cloneSrcUrl /*Note: be sure to file:// it*/, const QString &symlinkToAtomicallyUpdate, const QString &tempDirForBothCloneDestinationsAndSymlinkPreparation);
};

#endif // ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMIT_H
