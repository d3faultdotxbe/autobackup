#include "directoriesofaudioandvideofilesmuxersyncercli.h"

#include <QCoreApplication>
#include <QStringList>

#include "directoriesofaudioandvideofilesmuxersyncer.h"

//#define ERRRRRR cliUsage(); emit exitRequested(1); return;
#define ERRRRRR cliUsage(); qApp->exit(1); return;

DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::DirectoriesOfAudioAndVideoFilesMuxerSyncerCli(QStringList arguments, QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    arguments.removeFirst(); //filename
#if 1 //fff keeping qcoreapp out of this class means i haven't to exitRequested yet (still in constructor). could solve it a few ways, none of which are 'elegant' :(. Maybe I can/should introduce a "delayedConstructor" that is queued invoked from every "real" constructor, so i can get around the "can't connect to signals emitted in constructor" problem (for EVERY qobject i make, not just these *cli and *gui ones). that tiny bit of non-elegance does buy me a decent amount... (but i am digressing like fuck considering this is all dep of musicfingers xD (i never turn down an opportunity to experiment with improving the elegance of my code (maybe that's a problem (but if i don't do it, i never improve my code))))
    if(arguments.size() < 2)
    {
        ERRRRRR
    }
#endif
    QString directoryOfAudioFiles = arguments.takeFirst();
    QString directoryOfVideoFiles = directoryOfAudioFiles;
    if(arguments.size() > 1)
    {
        directoryOfVideoFiles = arguments.takeFirst();
    }
    QString outputDirectory = arguments.takeFirst();

    DirectoriesOfAudioAndVideoFilesMuxerSyncer *directoriesOfAudioAndVideoFilesMuxerSyncer = new DirectoriesOfAudioAndVideoFilesMuxerSyncer(this);
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool)), this, SLOT(handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool)));

    QMetaObject::invokeMethod(directoriesOfAudioAndVideoFilesMuxerSyncer, "muxAndSyncDirectoryOfAudioWithDirectoryOfVideo", Q_ARG(QString, directoryOfAudioFiles), Q_ARG(QString, directoryOfVideoFiles), Q_ARG(QString, outputDirectory));
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::cliUsage()
{
    handleO("Usage: DirectoriesOfAudioAndVideoFilesMuxerSyncerCli srcDir destDir\n\nYou can specify that the audio and video sources are in different directories, in which case you specify the src audio dir first, src video dir second, and of course dest dir last");
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success)
{
    if(!success)
    {
        handleE("muxing and syncing directory of audio with directory of video failed");
    }
    //emit exitRequested(success ? 0 : 1);
    qApp->exit(success ? 0 : 1);
}
