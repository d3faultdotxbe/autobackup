#include "ffmpegsegmentuploader.h"

#include <QFile>
#include <QFileSystemWatcher> //was considering QSocketNotifier, but QFile::handle() doesn't work on windows -_-
#include <QDateTime>
#include <QStringList>

//at first i was amped to learn about sftp, but now after trying to use it in automation it's a freaking pain in the ass. provides very little feedback (whereas scp i'd just check return code == 0). hmm, *tries cranking up verbosity*. cool, increasing verbosity does NOTHING (except a bunch of shit i don't care about on stderr). there's no "upload complete" message... pos...
FfmpegSegmentUploader::FfmpegSegmentUploader(QObject *parent) :
    QObject(parent), m_FfmpegSegmentsEntryListFile(0), m_FfmpegSegmentsEntryListFileWatcher(0), m_FiveSecondRetryTimer(new QTimer(this)), m_SftpProcess(0), m_SftpProcessTextStream(0), m_SftpIsReadyForCommands(false), m_SftpPutInProgressSoWatchForRenameCommandEcho(false), m_SftpWasToldToQuit(false)
{
    m_FiveSecondRetryTimer->setInterval(5000);
    m_FiveSecondRetryTimer->setSingleShot(true);
    connect(m_FiveSecondRetryTimer, SIGNAL(timeout()), this, SLOT(tryDequeueAndUploadSingleSegment()));
}
FfmpegSegmentUploader::~FfmpegSegmentUploader()
{
    if(m_SftpProcessTextStream)
        delete m_SftpProcessTextStream;
}
void FfmpegSegmentUploader::startSftpProcessInBatchMode()
{
    QStringList sftpArgs;
    sftpArgs << "-b" << "-" << m_UserHostPathComboSftpArg;
    m_SftpProcess->start(m_SftpProcessPath, sftpArgs, QIODevice::ReadWrite); //TODOidgaf: does windows use \r\n for stdin delimiting also? if so, QIODevice::Text flag should be passed in too
    //m_SftpProcess->waitForStarted(-1); //connect(m_SftpProcess, SIGNAL(started())
    //how the fuck do i know when the sftp session is READY? surely it isn't just when the process is started, guh
}
void FfmpegSegmentUploader::stopSftpProcess()
{
    if(m_SftpIsReadyForCommands)
    {
        *m_SftpProcessTextStream << "bye" << endl << "bye" << endl; //idfk why, but when i was testing this it needed two byes. since the second one [probably?] won't hurt, fuck it :-P
    }
    else if(m_SftpProcess->isOpen())
    {
        m_SftpProcess->close(); //hmmm idk lol
    }
}
void FfmpegSegmentUploader::enQueuePreviousSegmentEntryForUpload()
{
    QPair<QString,QString> newPair;
    newPair.first = m_PreviousSegmentEntryTimestamp;
    newPair.second = m_PreviousSegmentEntry;
    m_SegmentsQueuedForUpload.enqueue(newPair);
    QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection);
}
void FfmpegSegmentUploader::startUploadingSegmentsOnceFfmpegStartsEncodingTheNextOne(const QString &filenameOfSegmentsEntryList, const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    if(remoteDestinationToMoveTo.isEmpty() || remoteDestinationToUploadTo.isEmpty())
    {
        emit d("can't have empty remote destinations");
        return;
    }
    if(userHostPathComboSftpArg.isEmpty())
    {
        emit d("can't have empty user host path (combo) sftp arg. should look like this: 'username@host' -- or just 'host' at a minimum");
        return;
    }
    m_UserHostPathComboSftpArg = userHostPathComboSftpArg;
    m_RemoteDestinationToUploadToWithSlashAppended = appendSlashIfMissing(remoteDestinationToUploadTo);
    m_RemoteDestinationToMoveToWithSlashAppended = appendSlashIfMissing(remoteDestinationToMoveTo);
    if(filenameOfSegmentsEntryList.isEmpty())
    {
        emit d("invalid filename of segments entry list: '" + filenameOfSegmentsEntryList + "'");
        return;
    }
    if(localPath.isEmpty())
    {
        emit d("can't use empty local path");
        return;
    }
    m_LocalPathWithSlashAppended = appendSlashIfMissing(localPath);
    m_SftpProcessPath = sftpProcessPath;
    if(sftpProcessPath.isEmpty())
    {
        m_SftpProcessPath = "/usr/bin/sftp";
    }

    if(m_FfmpegSegmentsEntryListFile)
    {
        delete m_FfmpegSegmentsEntryListFile;
        m_FfmpegSegmentsEntryListFile = 0;
    }
    m_FilenameOfSegmentsEntryList = filenameOfSegmentsEntryList;
    if(!m_FfmpegSegmentsEntryListFile)
    {
        if(!QFile::exists(m_FilenameOfSegmentsEntryList))
        {
            emit d("segment entry list file does not exist: '" + m_FilenameOfSegmentsEntryList + "'");
            return; //TODOreq: this, and the below return, and probably otherh places, should quit cleanly/now. do more than just return (sftp may be running for example)
        }
        m_FfmpegSegmentsEntryListFile = new QFile(m_FilenameOfSegmentsEntryList, this);
        if(!m_FfmpegSegmentsEntryListFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit d("failed to open file for reading: '" + m_FilenameOfSegmentsEntryList + "'");
            delete m_FfmpegSegmentsEntryListFile;
            m_FfmpegSegmentsEntryListFile = 0;
            return;
        }
        m_FfmpegSegmentsEntryListFileTextStream.setDevice(m_FfmpegSegmentsEntryListFile);
        m_FfmpegSegmentsEntryListFileTextStream.seek(0);
    }

    if(m_FfmpegSegmentsEntryListFileWatcher)
        delete m_FfmpegSegmentsEntryListFileWatcher;
    m_FfmpegSegmentsEntryListFileWatcher = new QFileSystemWatcher(this);
    m_FfmpegSegmentsEntryListFileWatcher->addPath(filenameOfSegmentsEntryList);
    connect(m_FfmpegSegmentsEntryListFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleSegmentsEntryListFileModified()));
    m_PreviousSegmentEntry.clear();

    if(m_SftpProcess)
        delete m_SftpProcess;
    m_SftpProcess = new QProcess(this);
    if(m_SftpProcessTextStream)
        delete m_SftpProcessTextStream;
    m_SftpProcessTextStream = new QTextStream(m_SftpProcess);
    m_SftpProcess->setReadChannel(QProcess::StandardOutput);
    connect(m_SftpProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleSftpProcessFinished(int,QProcess::ExitStatus)));
    connect(m_SftpProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleSftpProcessReadyReadStandardError()));
    connect(m_SftpProcess, SIGNAL(readyRead()), this, SLOT(handleSftpProcessReadyReadStandardOut()));
    m_SftpIsReadyForCommands = false;
    m_SftpPutInProgressSoWatchForRenameCommandEcho = false;
    m_SftpWasToldToQuit = false;
    startSftpProcessInBatchMode();
    emit d("ffmpeg segment uploader starting");
}
void FfmpegSegmentUploader::tellStatus()
{
    int queueSize = m_SegmentsQueuedForUpload.size();
    QString headOfUploadQueue = (queueSize > 0 ?  m_SegmentsQueuedForUpload.head().second : "");
    QString segmentInformations = "==Ffmpeg Segment Uploader Status==\n\tMost recent segment entry (which is probably still being encoded): " + m_PreviousSegmentEntry + "\n\tSize of upload queue: " + QString::number(queueSize) + "\n\t'Head' of the upload queue: " + headOfUploadQueue + "\n\tsftp connection status: " + (m_SftpIsReadyForCommands ? "" : "NOT ") + "ready for commands";
    emit d(segmentInformations);
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegments()
{
    m_SftpWasToldToQuit = true;

    //hack to deal with the very last segment in the list, which wouldn't have been uploaded since we wait until the next one to start encoding before we begin upload
    enQueuePreviousSegmentEntryForUpload();

#if 0
    //deal with queue already empty (won't get to our other code)
    if(m_SegmentsQueuedForUpload.size() == 0)
    {
        stopSftpProcess();
    }
#endif
}
void FfmpegSegmentUploader::handleSegmentsEntryListFileModified()
{
    if(!m_PreviousSegmentEntry.isEmpty())
    {
        enQueuePreviousSegmentEntryForUpload();
    }

    m_PreviousSegmentEntryTimestamp = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()/1000);
    m_PreviousSegmentEntry = m_FfmpegSegmentsEntryListFileTextStream.readLine();
}
void FfmpegSegmentUploader::tryDequeueAndUploadSingleSegment()
{
    if(m_FiveSecondRetryTimer->isActive())
    {
        m_FiveSecondRetryTimer->stop();
    }
    if(m_SegmentsQueuedForUpload.isEmpty() || m_SftpPutInProgressSoWatchForRenameCommandEcho)
        return;    
#if 0 //pseudo-code
     bool uploadedOne = false;
    if(scpUpload(queuedPair.second, m_RemoteDestinationToUploadToWithSlashAppended)) //sftp >
    {
        if(persistentSshConnectionMoveCmd(m_RemoteDestinationToUploadToWithSlashAppended + queuedPair.second, m_RemoteDestinationToMoveToWithSlashAppended + queuedPair.first + ".ogg" /*TODOoptional: argv or auto-detect extention*/))
        {
            uploadedOne = true;
            m_SegmentsQueuedForUpload.dequeue();
        }
        //else: how would this fail? if the ssh connection drops?
    }
    else
    {
        //perhaps delete partially written file? probably not but honestly no idea. depends on scp's implementation guh
    }
    if(!uploadedOne)
    {
        emit d("ffmpeg segment uploader failed to upload a segment, check network status etc");
        QTimer::singleShot(5000, Qt::VeryCoarseTimer, this, SLOT(tryDequeueAndUploadSingleSegment())); //TODOoptional: exponential backoff with max maybe, but eh tbh the more i see this, the more lag my videos will have "overall", so 5 seconds is good/fine for now...
    }
    if(!m_SegmentsQueuedForUpload.isEmpty())
    {
        QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection);
    }
#endif
    if(m_SftpIsReadyForCommands)
    {
        const QPair<QString,QString> &queuedPair = m_SegmentsQueuedForUpload.head();
        QString remoteUploadPath = "\"" + m_RemoteDestinationToUploadToWithSlashAppended + queuedPair.second + "\"";
        QString putCommand = "put \"" + m_LocalPathWithSlashAppended + queuedPair.second + "\" " + remoteUploadPath;
        QString renameCommand = "rename \"" + remoteUploadPath + "\" \"" + m_RemoteDestinationToMoveToWithSlashAppended + queuedPair.first + ".ogg\"";
        //queue upload then rename to sftp
        *m_SftpProcessTextStream << putCommand << endl << renameCommand << endl;
        m_SftpPutInProgressSoWatchForRenameCommandEcho = true;
    }
    else
    {
        emit d("segment queue couldn't upload because sftp is not ready for commands, trying again in 5 sec");
        if(!m_FiveSecondRetryTimer->isActive())
        {
            m_FiveSecondRetryTimer->start();
        }
    }
}
void FfmpegSegmentUploader::handleSftpProcessReadyReadStandardOut()
{
    while(!m_SftpProcessTextStream->atEnd())
    {
        QString currentLine = m_SftpProcessTextStream->readLine();
        if(!m_SftpIsReadyForCommands)
        {
            if(currentLine.startsWith("sftp>"))
            {
                m_SftpIsReadyForCommands = true;
                QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection); //might use this code path for initial connection (race condition), but if this is a reconnect, we might want to resume uploading if queue isn't empty
            }
        }
        else
        {
            //process or disregard standard out
            //TODOreq: seeing the "rename" command being echo'd back to us means that the upload that preceeded it finished [successfully if the process didn't abort], which means we can now start uploading the next segment in the queue (if any)
            if(m_SftpPutInProgressSoWatchForRenameCommandEcho && currentLine.startsWith("sftp> rename "))
            {
                m_SftpPutInProgressSoWatchForRenameCommandEcho = false;
                m_SegmentsQueuedForUpload.dequeue();
                if(m_SftpWasToldToQuit && m_SegmentsQueuedForUpload.size() == 0)
                {
                    emit d("FfmpegSegmentUploader now cleanly stopping since upload queue is empty...");
                    stopSftpProcess();
                    return;
                }
                //now onto the next one
                QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection);
            }
            //disregard the "uploading ..." lines. worthless
        }
    }
}
void FfmpegSegmentUploader::handleSftpProcessReadyReadStandardError()
{
    QByteArray allStandardErrorBA = m_SftpProcess->readAllStandardError();
    QString allStandardErrorString(allStandardErrorBA);
    emit d("sftp's standard error wrote: "+ allStandardErrorString);
}
void FfmpegSegmentUploader::handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_SftpIsReadyForCommands = false;
    m_SftpPutInProgressSoWatchForRenameCommandEcho = false;

    if(!m_SftpWasToldToQuit) //TODOreq: should maybe rely solely on exitCode/exitStatus instead of this bool? it might be zero on a connection drop though idfk
    {
        //sftp -b mode will quit with exitCode == 1 any time an operation does not succeed, and I'd imagine also if/when the connection is 'lost' (?)... though perhaps that isn't noticed until an operation fails? idk lol

        //reconnect by starting it again
        startSftpProcessInBatchMode();
    }
    else
    {
        m_SftpWasToldToQuit = false;
        emit d("...FfmpegSegmentUploader has cleanly stopped");
        emit stoppedUploadingFfmpegSegments();
    }
    if(exitCode != 0)
    {
        emit d("sftp exitted with non-zero code: " + QString::number(exitCode));
    }
    if(exitStatus != QProcess::NormalExit)
    {
        emit d("sftp crashed (but is probably being restarted)");
    }
}
