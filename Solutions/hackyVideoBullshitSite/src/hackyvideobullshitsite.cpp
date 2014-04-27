#include "hackyvideobullshitsite.h"

#include <Wt/WServer>

#include <QCoreApplication>
#include <QSettings>
#include <QAtomicPointer>

#include "standardinputnotifier.h"

#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch "VideoSegmentsImporterFolderToWatch"
#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace "VideoSegmentsImporterFolderScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo "AirborneVideoSegmentsBaseDir_aka_videoSegmentsImporterFolderToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir "MyBrainArchiveBaseDir"
#define HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile "LastModifiedTimestampsFile"

#define HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS "/-!-!-!-!-!-!-!-placeholder/path-!-!-!-!-!-!-!"

HackyVideoBullshitSite::HackyVideoBullshitSite(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_ArgC(argc)
    , m_ArgV(argv)
    , m_WtControllerAndStdOutOwnerThread(new ObjectOnThreadHelper<WtControllerAndStdOutOwner>(this))
    , m_AdImageGetAndSubscribeManagerThread(new ObjectOnThreadHelper<AdImageGetAndSubscribeManager>(this))
    , m_VideoSegmentsImporterFolderWatcherThread(new ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher>(this))
    , m_LastModifiedTimestampsWatcherThread(new ObjectOnThreadHelper<LastModifiedTimestampsWatcher>(this))
    , m_ObjectOnThreadSynchronizer(new ObjectOnThreadSynchronizer(this))
    , m_VideoSegmentsImporterFolderToWatch(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_VideoSegmentsImporterFolderScratchSpace(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_MyBrainArchiveBaseDir_NoSlashAppended(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_LastModifiedTimestampsFile(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_StdIn(new StandardInputNotifier(this))
{
    qRegisterMetaType<char **>("char **");

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit())); //unneeded

    //QCoreApplication::setOrganizationName("HackyVideoBullshitSiteOrganization");
    //QCoreApplication::setOrganizationDomain("HackyVideoBullshitSiteDomain.tld");
    //QCoreApplication::setApplicationName("HackyVideoBullshitSite");
    //QSettings::setDefaultFormat(QSettings::IniFormat);

    {
        QSettings hackyVideoBullshitSiteSettings(QSettings::IniFormat, QSettings::UserScope, "HackyVideoBullshitSiteOrganization", "HackyVideoBullshitSite"); //because too lazy to integrate with Wt args... TODOoptional yea do it because editing settings files is teh suck (especially for automation). Still, future proofing for that by passing the settings by args to child objects instead of the static "setX" way of using qsettings (commented out above)

        bool atLeastOneDidntExist = false;

        m_VideoSegmentsImporterFolderToWatch = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_VideoSegmentsImporterFolderToWatch == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, m_VideoSegmentsImporterFolderToWatch);
        }

        m_VideoSegmentsImporterFolderScratchSpace = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_VideoSegmentsImporterFolderScratchSpace == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, m_VideoSegmentsImporterFolderScratchSpace);
        }

        m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);
        }

        m_MyBrainArchiveBaseDir_NoSlashAppended = removeTrailingSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_MyBrainArchiveBaseDir_NoSlashAppended == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, m_MyBrainArchiveBaseDir_NoSlashAppended);
        }

        m_LastModifiedTimestampsFile = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_LastModifiedTimestampsFile == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        if(atLeastOneDidntExist)
        {
            cerr << "exitting with error because the settings in '" + hackyVideoBullshitSiteSettings.fileName().toStdString() + "' were not properly defined. look for the string '" HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS "' and fill in the proper paths. also look above for a more specific error" << endl;
            handleFatalError();
            return;
        }
    }

    connect(m_WtControllerAndStdOutOwnerThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleWtControllerAndStdOutOwnerIsReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_WtControllerAndStdOutOwnerThread);
    m_WtControllerAndStdOutOwnerThread->start(); //yes that backend thread might start and emit readyForConnections, but 'this' thread won't process it until our constructor is finished!

    //don't have any that aren't dependent on the whole being ready?, simply don't connect :).
    connect(m_AdImageGetAndSubscribeManagerThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleAdImageGetAndSubscribeManagerIsReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_AdImageGetAndSubscribeManagerThread);
    m_AdImageGetAndSubscribeManagerThread->start();

    connect(m_VideoSegmentsImporterFolderWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleVideoSegmentsImporterFolderWatcherReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_VideoSegmentsImporterFolderWatcherThread);
    m_VideoSegmentsImporterFolderWatcherThread->start();

    connect(m_LastModifiedTimestampsWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLastModifiedTimestampsWatcherReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_LastModifiedTimestampsWatcherThread);
    m_LastModifiedTimestampsWatcherThread->start();

    connect(m_ObjectOnThreadSynchronizer, SIGNAL(allObjectsOnThreadsReadyForConnections()), this, SLOT(handleAllBackendObjectsOnThreadsReadyForConnections())); //death to boolean flags like 'm_XisReady' and BlockingQueuedConnection initialization!

    connect(m_StdIn, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInput(QString)));
}
void HackyVideoBullshitSite::handleWtControllerAndStdOutOwnerIsReadyForConnections()
{
    WtControllerAndStdOutOwner *wtControllerAndStdOutOwner = m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(e(QString)), wtControllerAndStdOutOwner, SLOT(handleE(QString)));
    connect(this, SIGNAL(o(QString)), wtControllerAndStdOutOwner, SLOT(handleO(QString)));
    connect(wtControllerAndStdOutOwner, SIGNAL(fatalErrorDetected()), this, SLOT(handleFatalError()));
}
void HackyVideoBullshitSite::handleVideoSegmentsImporterFolderWatcherReadyForConnections()
{
    VideoSegmentsImporterFolderWatcher *videoSegmentsImporterFolderWatcher = m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly();
    //connect(videoSegmentsImporterFolderWatcher, &VideoSegmentsImporterFolderWatcher::d, this, &HackyVideoBullshitSiteBackend::d); //hmm now d seems too close to d_ptr xD
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(this, SIGNAL(finishStoppingRequested()), videoSegmentsImporterFolderWatcher, SLOT(finishStopping())); //TODOreq: should maybe be blocking queued, but i don't think that's necessary since i think QThread::quit is an event and that means that it won't be processed until after finishStopping... finishes. now terminate on the other hand is a different story :-P
    QMetaObject::invokeMethod(videoSegmentsImporterFolderWatcher, "initializeAndStart", Q_ARG(QString, m_VideoSegmentsImporterFolderToWatch), Q_ARG(QString, m_VideoSegmentsImporterFolderScratchSpace), Q_ARG(QString, m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo));
}
void HackyVideoBullshitSite::handleLastModifiedTimestampsWatcherReadyForConnections()
{
    LastModifiedTimestampsWatcher *lastModifiedTimestampsWatcher = m_LastModifiedTimestampsWatcherThread->getObjectPointerForConnectionsOnly();
    connect(lastModifiedTimestampsWatcher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    //OLD-when-atomic: similarly, instead of a pointer to a video segment file, i could point to a plugin to expand capability without shutting the server down :-P
    //o hai atomicz <3 (*throws up*)
    WtControllerAndStdOutOwner::setTimestampsAndPathsSharedAtomicPointer(lastModifiedTimestampsWatcher->getTimestampsAndPathsAtomicPointer());
    QMetaObject::invokeMethod(lastModifiedTimestampsWatcher, "startWatchingLastModifiedTimestampsFile", Q_ARG(QString, m_LastModifiedTimestampsFile));
}
void HackyVideoBullshitSite::handleAdImageGetAndSubscribeManagerIsReadyForConnections()
{
    //derp passes itself to WtControllerAndStdOutOwner, but we dunno if he's ready yet :-P (FUKKEN SOLVED FINALLY <3 <3 <3)

    connect(this, SIGNAL(beginStoppingRequested()), m_AdImageGetAndSubscribeManagerThread->getObjectPointerForConnectionsOnly(), SLOT(beginStopping()));
}
void HackyVideoBullshitSite::handleAllBackendObjectsOnThreadsReadyForConnections()
{
    WtControllerAndStdOutOwner::setAdImageGetAndSubscribeManager(m_AdImageGetAndSubscribeManagerThread->getObjectPointerForConnectionsOnly());
    WtControllerAndStdOutOwner::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);
    WtControllerAndStdOutOwner::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(m_MyBrainArchiveBaseDir_NoSlashAppended);

    QMetaObject::invokeMethod(m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly(), "initializeAndStart", Q_ARG(int, m_ArgC), Q_ARG(char **, m_ArgV));

    connect(this, SIGNAL(beginStoppingRequested()), QCoreApplication::instance(), SLOT(quit()), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
}
void HackyVideoBullshitSite::handleStandardInput(const QString &line)
{
    QString lineToLower = line.toLower();
    if(lineToLower == "h")
    {
        emit o("eat shit");
        return;
    }
    if(lineToLower == "q")
    {
        emit o("starting to quit...");
        disconnect(m_StdIn, SIGNAL(standardInputReceivedLine(QString)));
        emit beginStoppingRequested(); //bleh good design would warrant a way to synchronize that all backends are done with beginStopping and ready for finishStopping. i overengineered this solution (it now does thread::quit shit), but also that's only a hypothetical enhancement, i don't need to wait for the backends to be ready for finish stopping (finish stopping no longer exists <3, is just destructor now)
        return;
    }
}
void HackyVideoBullshitSite::handleAboutToQuit()
{
    //i'm too cool for this (unneeded)...
    //but let's assume that we didn't have faith in our destruction order and want to make sure m_ObjectOnThreadSynchronizer is destructed first, since not destructing it first will segfault us as it tries to call QThread::quit on... (O SHIT NVM THEY ARE SLOTS HAHA I AM DUMB I CAN CALL SLOTS SAFELY ALWAYS NO MATTER THE ORDER)
    if(m_ObjectOnThreadSynchronizer)
    {
        delete m_ObjectOnThreadSynchronizer; //not necessary because of parenting (is parenting lifo? i would guess yes... but idk)
        m_ObjectOnThreadSynchronizer = 0;
    }
}
void HackyVideoBullshitSite::handleFatalError()
{
    QCoreApplication::exit(1);
}

