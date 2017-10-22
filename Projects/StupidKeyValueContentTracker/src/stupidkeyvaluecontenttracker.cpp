#include "stupidkeyvaluecontenttracker.h"

#include <QCoreApplication>
#include <QDateTime>

#include "keyvaluestoremutation_add.h"
#include "keyvaluestoremutation_modify.h"
#include "keyvaluestoremutation_remove.h"
#include "keyvaluestoremutationfactory.h"

//TODOreq: this class uses (wraps/abstracts/hides) "mutations" of a KeyValue store. all underlying data must fit into a TimeAndData_Timeline json doc. Data is b64 encoded in that title, we do NOT add keys "next to" 'time' and 'data'. data will be, in MOST OF MY USES, a json sub-obj! I had written "a b64 json obj", but actually that's not necessary since json has the same "escape keys" as json (duh). fuck yea +1 json. but still worth noting that time and data are the ONLY 2 top level keys
//TODOoptional: libfuse could present it as a fs... key = filepath, value = file contents. hahaha and these fs dev noobs are STILL making "fs attribute extension" shit left and right hahahaha (doing it wrong)
//TODOoptimization: maybe use couchbase as a backend, since IIRC if your design uses an "append-only" strat, then couchbase is actually pro af. With AnonymousBitcoinComputing I wasn't using an append-only strat, and it eventually bit me in the ass (my designs sucked and were hacky). in any case, I'm not touching the QSettings-based quick-n-dirty database backend until the app has become more mature and stable. year/month/day folderization (timeline-entries (files in the dirs) are named using QCryptographicsHash of TimeAndData and QTemporaryFile for good no-overwrite safety) is a good 1-machine optimization. LevelDB also comes to mind, but I recall looking at it and feeling like the design was simple (ezily stealable) but the reference implementation was missing the crucial part (writing to disk) and reading other people's code is a pita sometimes
//TODOprobably: commitMessage is optional. just like filenames, I can't be arsed to type them 99% of the time (but I certainly like having the option to)
StupidKeyValueContentTracker::StupidKeyValueContentTracker(QObject *parent)
    : QObject(parent)
    , m_Timeline(new TimeAndData_Timeline(this))
{
    connect(this, &StupidKeyValueContentTracker::retrieveAllTimelineEntriesRequested, m_Timeline, &TimeAndData_Timeline::readAndEmitAllTimelineEntries);
    connect(m_Timeline, &TimeAndData_Timeline::finishedReadingAllTimelineEntries, this, &StupidKeyValueContentTracker::handleAllTimelineEntriesRead);

    connect(this, &StupidKeyValueContentTracker::appendJsonObjectToTimelineRequested, m_Timeline, &TimeAndData_Timeline::appendJsonObjectToTimeline);
    connect(m_Timeline, &TimeAndData_Timeline::finishedAppendingJsonObjectToTimeline, this, &StupidKeyValueContentTracker::handleFinishedAppendingJsonObjectToTimeline_aka_emitCommitFinished);

    QCoreApplication::setOrganizationName("StupidKeyValueContentTrackerOrganization");
    QCoreApplication::setOrganizationDomain("StupidKeyValueContentTrackerDomain");
    QCoreApplication::setApplicationName("StupidKeyValueContentTracker");
    QSettings::setDefaultFormat(QSettings::IniFormat);
}
StupidKeyValueContentTracker::~StupidKeyValueContentTracker()
{
    //persist m_CurrentData to disk, and set haveCache to true if we actually wrote anything. this allows us to restart the app without re-reading the ENTIRE timeline
    //TODOmb: QtSystemSignalHandler so we can still persist to cache on CTRL+C (SIGTERM) etc? honestly if the cache isn't being written to AS THE APP IS RUNNING (which is NOT currently the case), then writing to cache at last-minute like that might not be a good idea. I think CTRL+C has to "not block" or some such? eh maybe it's fine since we're intercepting the signals, I forget...
    if(!m_CurrentData.isEmpty())
    {
        QSettings settings;
        settings.setValue(StupidKeyValueContentTracker_SETTINGSKEY_HAVECACHE, true);
        settings.beginGroup(StupidKeyValueContentTracker_SETTINGSKEY_CACHEGROUP);
        for(CurrentDataType::const_iterator it = m_CurrentData.constBegin(); it != m_CurrentData.constEnd(); ++it)
        {
            settings.setValue(it.key(), it.value());
        }
        settings.endGroup();
    }
}
void StupidKeyValueContentTracker::populateCurrentDataWithCache(QSettings &settings)
{
    //note: haveCache was already shown to be true
    settings.beginGroup(StupidKeyValueContentTracker_SETTINGSKEY_CACHEGROUP);
    QStringList allCurrentDataKeys = settings.childKeys();
    Q_FOREACH(const QString &key, allCurrentDataKeys)
    {
        m_CurrentData.insert(key, settings.value(key).toString());
    }
    settings.endGroup();
}
void StupidKeyValueContentTracker::commitStagedKeyValueStoreMutations_ThenEmitCommitFinished(const QString &commitMessage)
{
    //TODOreq: all or nothing! I mean that IS the point of a commit... but still I doubt I'll see failures at first, those are corner cases
    //nvm: qint64 currentMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); //time in the TimeAndData_Timeline context (nvm)!!!
    QJsonObject commitData; //nvm: data in the TimeAndData_Timeline context (nvm)!!! <-- we still need to squeeze the "commit message" into the "data" in the TimeAndData_Timeline, and we're not there quite yet
    populateCommitDataUsingStagedKeyValueStoreMutations(commitData);
    commitActual_ThenEmitCommitFinished(commitData, commitMessage);
}
void StupidKeyValueContentTracker::populateCommitDataUsingStagedKeyValueStoreMutations(QJsonObject &bulkMutations)
{
    for(StagedMutationsType::const_iterator it = m_StagedKeyValueStoreMutation.constBegin(); it != m_StagedKeyValueStoreMutation.constEnd(); ++it)
    {
        IKeyValueStoreMutation *mutation = it.value().data();
        mutation->appendYourselfToBulkMutationsJsonObject(it.key(), bulkMutations);
    }
}
void StupidKeyValueContentTracker::commitActual_ThenEmitCommitFinished(const QJsonObject &commitData, const QString &commitMessage)
{
    QJsonObject commitDataActual;
    commitDataActual.insert(StupidKeyValueContentTracker_JSONKEY_COMMITMESSAGE, commitMessage);
    commitDataActual.insert(StupidKeyValueContentTracker_JSONKEY_BULKMUTATIONS, QJsonValue(commitData));

    //TODOreq: write that BulkMutations to TimeAndData_Timeline.... somehow...
    //something like:
    //return m_Timeline.tryAppend(currentMSecsSinceEpoch, commitDataActual);
    //^maybe a signal appendToTimelineRequested? sameshit but yea. but the way we got here though was a synchronous/error-checking private call, so maybe NOT using a signal is best? BUT tbh async is almost always better, so even though it'd be a small refactor async is def doable. ima actually CODE m_Timeline before I decide...

    emit appendJsonObjectToTimelineRequested(commitDataActual);
}
void StupidKeyValueContentTracker::applyStagedMutationsToCurrentData()
{
    for(StagedMutationsType::const_iterator it = m_StagedKeyValueStoreMutation.constBegin(); it != m_StagedKeyValueStoreMutation.constEnd(); ++it)
    {
        IKeyValueStoreMutation *mutation = it.value().data();
        mutation->mutateCurrentStupidKeyValueContent(it.key(), &m_CurrentData);
    }
}
void StupidKeyValueContentTracker::initialize()
{
    QSettings settings;
    bool haveCache = settings.value(StupidKeyValueContentTracker_SETTINGSKEY_HAVECACHE, false).toBool();
    if(haveCache)
    {
        //populate m_CurrentData with the QSettings-based cache
        populateCurrentDataWithCache(settings);
        emit o("populated KeyValue store from cache");
        emit initializationFinished(true);
    }
    else
    {
        //iterate over all m_Timeline entries and populate m_CurrentData accordingly
        emit retrieveAllTimelineEntriesRequested();
        //TODOreq: don't allow add/commit/etc UNTIL handleCurrentDataRetrieved is called. not sure if this should be enforced in code or just in comments :-/
        emit o("populated KeyValue store by re-reading entire timeline");
    }
}
void StupidKeyValueContentTracker::add(const QString &key, const QString &data)
{
    //add staged mutation for adding the key

    if(m_CurrentData.contains(key))
    {
        emit e("key already exists: " + key);
        emit addFinished(false);
        return;
    }
    if(m_StagedKeyValueStoreMutation.contains(key))
    {
        emit e("you are already mutating key '" + QString(key) + "', so commit first or unstage it");
        emit addFinished(false);
        return;
    }
    QSharedPointer<KeyValueStoreMutation_Add> addMutation(new KeyValueStoreMutation_Add(data)); //TODOreq: KeyValueStoreMutation_Delete, KeyValueStoreMutation_Modify
    m_StagedKeyValueStoreMutation.insert(key, addMutation);
    emit addFinished(true);
}
void StupidKeyValueContentTracker::modify(const QString &key, const QString &newValue)
{
    //add staged mutation for modifying the key

    //TODOreq: should we 'point to' a 'parent value' to ensure we modify what we intended on modifying and not some raced-in new value? "rolling" hashes are needed, which means I need to refactor some. this could also apply to 'remove', but not to 'add' (since no KNOWN parent is necessary, only a necessetated lack of a parent)

    if(!m_CurrentData.contains(key))
    {
        emit e("key doesn't exist: " + key);
        emit modifyFinished(false);
        return;
    }
    if(m_StagedKeyValueStoreMutation.contains(key))
    {
        //TODOmb: allow a staged mutation to be modified before ever being comitted? kinda makes sense, but maybe a separate "unstage" command is better? obv modify is SUPPOSED to work on COMMITTED values, not 'staged' ones (but it could work either way, I need to decide TODOreq)
        emit e("you are already mutating key '" + QString(key) + "', so commit first or unstage it");
        emit modifyFinished(false);
        return;
    }
    QSharedPointer<KeyValueStoreMutation_Modify> modifyMutation(new KeyValueStoreMutation_Modify(newValue));
    m_StagedKeyValueStoreMutation.insert(key, modifyMutation);
    emit modifyFinished(true);
}
void StupidKeyValueContentTracker::removeKey(const QString &key)
{
    //add staged mutation for removing the key

    if(!m_CurrentData.contains(key))
    {
        emit e("key doesn't exist: " + key);
        emit removeKeyFinished(false);
        return;
    }
    if(m_StagedKeyValueStoreMutation.contains(key))
    {
        //TODOmb: allow a staged mutation to be removed before ever being comitted? kinda makes sense, but maybe a separate "unstage" command is better? obv rm is SUPPOSED to work on COMMITTED values, not 'staged' ones (but it could work either way, I need to decide TODOreq)
        emit e("you are already mutating key '" + QString(key) + "', so commit first or unstage it");
        emit removeKeyFinished(false);
        return;
    }
    QSharedPointer<KeyValueStoreMutation_Remove> removeMutation(new KeyValueStoreMutation_Remove());
    m_StagedKeyValueStoreMutation.insert(key, removeMutation);
    emit removeKeyFinished(true);
}
void StupidKeyValueContentTracker::commit(const QString &commitMessage)
{
    commitStagedKeyValueStoreMutations_ThenEmitCommitFinished(commitMessage);
}
void StupidKeyValueContentTracker::readKey(const QString &key, const QString &revision)
{
    if(!revision.isEmpty())
    {
        //TODOreq: get the value of the key at that specific revision
        return;
    }
    //else: "HEAD" in git lingo
    CurrentDataType::const_iterator it = m_CurrentData.constFind(key);
    bool found = (it != m_CurrentData.constEnd());
    if(!found)
        emit e("key not found: '" + key + "'");
    QString data = found ? (*it) : QString();
    emit readKeyFinished(found, key, revision, data);
}
void StupidKeyValueContentTracker::handleAllTimelineEntriesRead(const AllTimelineEntriesType &allTimelineEntries)
{
    //parse allTimelineEntries and populate m_CurrentData accordingly
    for(AllTimelineEntriesType::const_iterator it = allTimelineEntries.constBegin(); it != allTimelineEntries.constEnd(); ++it)
    {
        //TODOreq: what to do with the timestamp here? does StupidKeyValueContentTracker even want/need it? maybe that's what I'll query by? "what was the value of this key at this point in time?" (and you don't have to specify the EXACT time of the commit ofc). idk yet tbh
        //TODOreq: we could also access the commitMessage here, but just like the timestamp I'm not sure we care about it... yet?
        const QJsonObject &data = it.value();
        const QJsonObject &mutations = data.value(StupidKeyValueContentTracker_JSONKEY_BULKMUTATIONS).toObject(); //TODOreq: should we care about errors? such as the mutations key not existing? blah mind = exploded
        for(QJsonObject::const_iterator it2 = mutations.constBegin(); it2 != mutations.constEnd(); ++it2)
        {
            QString key = it2.key();
            const QJsonObject &mutationTypeAndMutationValue = it2.value().toObject();
            QJsonObject::const_iterator it3 = mutationTypeAndMutationValue.constBegin(); //TODOreq: error checking of it3 maybe? and also would we allow multiple different types of mutations to a single key here, or will there always be just 1? I think just 1
            QString mutationType = it3.key();
            QString mutationValue = it3.value().toString(); //TODOreq: error checkin mb
            StupidKeyValueContentTracker_StagedMutationsValueType stagedMutationForImmediateApplication = KeyValueStoreMutationFactory::createKeyValueStoreMutation(mutationType, mutationValue);
            stagedMutationForImmediateApplication->mutateCurrentStupidKeyValueContent(key, &m_CurrentData);
        }
    }
    emit initializationFinished(true);
}
void StupidKeyValueContentTracker::handleFinishedAppendingJsonObjectToTimeline_aka_emitCommitFinished(bool success)
{
    if(!success)
    {
        emit e("error: failed to commitStagedKeyValueStoreMutation()! your key/values are still 'staged'. your db is probably fucked tho, psbly half of staged values were written, half not");
        emit commitFinished(false);
        return;
    }
    emit o("successfully committed 'staged' key value store mutations");
    applyStagedMutationsToCurrentData();
    m_StagedKeyValueStoreMutation.clear();
    emit commitFinished(success); //xD "success" daisy chaining <3
}
