﻿#include "timeanddata_timeline.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QCoreApplication> //for QSettings global config
#include <QSettings> //TODOmb: migrate away from the simplicity of a QSettings-based application database?
#include <QCryptographicHash>
#include <QStringList>
#include <QStack>

#include "timeanddata_timelinerequestresponsecontracts.h"

#define TimeAndData_Timeline_GROUP "TimeAndData_TimelineGroup"

using namespace TimeAndData_TimelineRequestResponseContracts;

//note: there is no delete, only append
//I was originally going to make "data" be a QString, but I think I'm changing that now to be a QJsonObject. anything that can be a string can be a json object, so it's not a huge requirement. it is just easier to work with and requires less serialization/deserialization when working with the "users of" this lib. serialize ONCE, parse ONCE. if data was QString then we'd need to serialize [at least] twice and parse [at least] twice. I hope this decision doesn't come back to bite me in the ass...
//^I'm kinda considering changing data into being b64 encoded. It just kinda makes more sense that the LOWEST DB LEVEL doesn't need to worry about Json parse errors for HIGH LEVEL code. both in the error handling sense AND in the speed sense. it might be more optimal to only "parse when needed", however arguably it's also an optimization to only "[de]serialize once" like I have it currently written. I guess ultimately the most important deciding factor is whether or not I want to 'expect' malformed json! If all json is only ever generated by machine, I shouldn't have a problem (unless there's a power failure, but QSaveFile can save the day in that case anyways :-D)... but yea json actually invites uses to modify it. and you know a part of me is actualy fine with that and I might even be modifying json myself someday. There are literally infinite layer 7 apps that can be built using this lib, and at least 1 of them probably has a justified use case for users editting json (but still, json sanitization tools are ez to make as well lmfao (sanitize before accepting user modifications)). Idk this is all basically just a TODOoptimization at this point because I don't need to account for malformed json (YET)
TimeAndData_Timeline::TimeAndData_Timeline(QObject *parent)
    : QObject(parent)
    , m_Contracts(new Contracts(this))
{
    QCoreApplication::setOrganizationName("TimeAndData_TimelineOrganization");
    QCoreApplication::setOrganizationDomain("TimeAndData_TimelineDomain");
    QCoreApplication::setApplicationName("TimeAndData_Timeline");
    QSettings::setDefaultFormat(QSettings::IniFormat); //TODOreq: search EVERY project I've coded for the use of the above 3 configs (setOrgName, setDomainName, setAppName) and make sure that this setDefaultFormat is always called there too (unless the app doesn't warrant it, but I think most do. Ini files are just much more portable than "registrty edits" on that one gross operating system. Hell I think it should default to Ini format on all platforms and the Native formats should be opted INTO (since they [can] _SUCK_) TODOmb: file bugreport to default to Ini (or pull request)
}
//our data structure is optimized to be read in REVERSE chronological order. forwards chronological order is possible but expensive/slow
void TimeAndData_Timeline::readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrder()
{
    ReadAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrderScopedResponder scopedResponder(m_Contracts->readAndEmitAllTimelineEntriesInInLessEfficientForwardsChronologicalOrder());

    QSettings settings;
    settings.beginGroup(TimeAndData_Timeline_GROUP);
    {
        TimelineEntryIdType latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId = settings.value(TimeAndData_Timeline_SETTINGSKEY_LATEST_TIMELINE_ENTRY_ID_AKA_HEAD_IN_GIT_LINGO, nullAkaEmptyAkaRootTimelineEntryId()).toString();
        scopedResponder.response()->setLatestTimelineEntryId(latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId);
        if(latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId == nullAkaEmptyAkaRootTimelineEntryId())
        {
            //there are no timeline entries (first run etc)
            scopedResponder.response()->setSuccess(true);
            return /*emit*/;
        }

        settings.beginGroup(TimeAndData_Timeline_SETTINGSGROUP_TIMELINE_ENTRIES);
        {
            QStack<TimelineEntryIdType> timelineEntryIds; //TODOoptimization: don't require "all timeline entry IDs" to be loaded into memory simultaneously. QCache might be of use, but idk the "perfect" solution to this, since timeline entries only contain their PARENT timeline entry id and not their "children" timeline entry ids. It might be more speed/hdd inefficient to use QCache/etc and re-look up keys etc over and over, BUT it's better (memory management wise) than requiring over 9000gb of _keys_ (previous versio of this method had all the _values_ simultaneously loaded into memory as well, kek) be loaded into memory SIMULTANEOUSLY! However it should be noted that QSettings is _PROBABLY_ internally already using a QCache [type thing], so my efforts might be wasted here until I move away from QSettings-based db
            TimelineEntryIdType currentTimelineEntryId = latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId;
            while(currentTimelineEntryId != nullAkaEmptyAkaRootTimelineEntryId())
            {
                timelineEntryIds.push(currentTimelineEntryId);

                QString parsedParentTimelineEntryId;
                {
                    QString timeAndDataAndParentIdJsonString = settings.value(currentTimelineEntryId).toString();
                    QJsonObject timeAndDataAndParentIdJson;
                    if(!parseJsonString(timeAndDataAndParentIdJsonString, &timeAndDataAndParentIdJson))
                    {
                        return /*emit*/;
                    }

                    QJsonValue jsonValueMaybe = timeAndDataAndParentIdJson.value(TimeAndData_Timeline_JSONKEY_PARENT_TIMELINE_ENTRY_ID);
                    if(jsonValueMaybe.isUndefined())
                        parsedParentTimelineEntryId = nullAkaEmptyAkaRootTimelineEntryId();
                    else
                        parsedParentTimelineEntryId = jsonValueMaybe.toString();
                }

                currentTimelineEntryId = parsedParentTimelineEntryId;
            }
            //when we get here, the QStack contains ALL timelineEntryIds in our timeline, and the ROOT timeline entry is on the "top" of the stack
            while(!timelineEntryIds.isEmpty())
            {
                TimelineEntryIdType currentTimelineEntryId = timelineEntryIds.pop();

                QString timeAndDataAndParentIdJsonString = settings.value(currentTimelineEntryId).toString();
                QJsonObject timeAndDataAndParentIdJson;
                if(!parseJsonString(timeAndDataAndParentIdJsonString, &timeAndDataAndParentIdJson))
                {
                    return /*emit*/;
                }
                //std::unique_ptr<TimeAndDataAndParentId_TimelineEntry> timelineEntry;
                TimeAndDataAndParentId_TimelineEntry timelineEntry;
                if(!populateTimelineEntryFromJsonObject(timeAndDataAndParentIdJson, timelineEntry))
                {
                    return /*emit*/;
                }
                //TODOmb: we could do [read-time] VERIFICATION here, sha3 the value against it's key
                emit timelineEntryRead(timelineEntry);
            }
        }
        settings.endGroup();
    }
    settings.endGroup();

    scopedResponder.response()->setSuccess(true);
    return /*emit*/;
}
void TimeAndData_Timeline::appendJsonObjectToTimeline(const QJsonObject &data)
{
    AppendJsonObjectToTimelineScopedResponder scopedResponder(m_Contracts->appendJsonObjectToTimeline());

    //TO DOnereq(read huge comment below): should I have time passed in to me or should I determine it myself here/now?
    QDateTime currentDateTimeUtc = QDateTime::currentDateTimeUtc(); //TODOreq: _always_ UTC in backend, then convert-on-demand to timezones when showing user
    //TODOreq: should I factor in a cryptographic hash of the data so that time/key collisions don't overwrite?
    //I'm kinda thinking a "one file per timeline entry" fs heirarchy, in which case colliding timestamps could be handled in any number of ways (0000.bin, 0001.bin (therein, time+data serialized), etc). if I want to get fancy I could do year/month folderization and put the cryptographic hash in the filename. folderization is really just an optimization though (a worthwhile one if you ask me). fuck I need to DECIDE. do I want to allow 2 different items to have the same timestamp? of fucking course. which strat do I want? blah there are pros an cons of each, but worse there are SERIOUSLY HUGE IMPLICATIONS of each (unless I abstract it properly).
    //do I want to have mid-timeline insertions, or only appends? I think yes appends only except MAYBE in some special admin/FIXING-SOMETHING-BROKEN/dangerous mode? so that means WE are the ones to gen the timestamp, ok changing my api now to reflect that
    //it's STILL possible to get dupe timestamps of course. you know what fuck it I like year/month/day folderization, then numMSecElapsedToday_sha512orwhatever.bin -- ex: 2017/9/23/69420_09f9deadbeefbabesha512hex.bin
    //MAYBE, if after analyzing data the type can be DETERMINED to be plaintext, we use a .txt extension for it and if not a .bin extension? or we can use a .json if determined to be .json? oh where the fuck does it end??? binary/text is one thing, but nah json nah (but then again json is "special" as it's my internal type etc, so maybe ONE exception for json. bin=unknown, txt=txt-not-json, json=json). wait no it is ALWAYS json. we task the USER OF this lib with being json-safe, b64-encoding if needed. so yea there is only ONE extension type xD: .json <3. note that even though it's json and human readable, it's still intended to be READ-ONLY (modifying would invalidate the sha512!! use StupidKeyValueContentTracker if you want to "MODIFY" (revise) a "file". TODOreq: there is a data race maybe if 2+ StupidKeyValueContentTracker instances share a Timeline instance and call append simultaneously. one of them might "mutate" the other mutation (rather than the state before it), which is uninntended. TODOreq: we need a "appendAtExactlyThisPositionOrFail" (the position is polled from Timeline, just before commit. so it will almost always go through, but the occassional data races are prevented! we just error out, possibly retrying (now instead mutating the mutation (or rather, it's state after applied) that JUSt beat us to the append) depending on the intent and if there are conflicts)
    //man this blew up in my face, I'm glad it was kept as simple as it is though :). just time and data :)

    //I've thought more on this and decided not to do year/month/day/msecElapsedSinceStartOfDay.json, yet
    //nor will I use a cryptographic hash, yet
    //I'm going to do something quick and simple and PREDICT that I will refactor it when I implement readKey and/or readTimeline(what to use as input for timeline? (this will "come to me" when I'm coding readKey probably))

    QSettings settings;
    settings.beginGroup(TimeAndData_Timeline_GROUP);
    {
        TimelineEntryIdType latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId = settings.value(TimeAndData_Timeline_SETTINGSKEY_LATEST_TIMELINE_ENTRY_ID_AKA_HEAD_IN_GIT_LINGO, nullAkaEmptyAkaRootTimelineEntryId()).toString();

        qint64 time = currentDateTimeUtc.toMSecsSinceEpoch();
        TimeAndDataAndParentId_TimelineEntry timelineEntry(time, data, latestTimelineEntryId_OrNullAkaEmptyAkaRootTimelineEntryId);

        QJsonObject timeAndDataAndParentIdJsonObject = timelineEntry.toJsonObject();
        QJsonDocument jsonDoc(timeAndDataAndParentIdJsonObject);
        QByteArray jsonByteArray = jsonDoc.toJson(QJsonDocument::Compact/*TODOreq: Indented if ever moved out of QSettings*/);

        QString currentTmelineEntryId = timelineEntry.timelineEntryId();

        settings.beginGroup(TimeAndData_Timeline_SETTINGSGROUP_TIMELINE_ENTRIES);
        {
            settings.setValue(currentTmelineEntryId, QString(jsonByteArray) /*nope (works but FUGLY/binary): data.toVariantMap()*/);
        }
        settings.endGroup();
        settings.setValue(TimeAndData_Timeline_SETTINGSKEY_LATEST_TIMELINE_ENTRY_ID_AKA_HEAD_IN_GIT_LINGO, currentTmelineEntryId);

        scopedResponder.response()->setTimelineEntry(timelineEntry);
    }
    settings.endGroup();

    scopedResponder.response()->setSuccess(true);
    return /*emit*/;
}
TimeAndData_Timeline::~TimeAndData_Timeline()
{
    //non-inline destructor necessary to use a forwards declared type (Contracts) in scoped object
}
bool TimeAndData_Timeline::parseJsonString(const QString &jsonInputString, QJsonObject *out_JsonObject)
{
    QByteArray timeAndDataAndParentIdJsonBA = jsonInputString.toUtf8();
    QJsonParseError jsonParseError;
    QJsonDocument timeAndDataAndParentIdJsonDoc = QJsonDocument::fromJson(timeAndDataAndParentIdJsonBA, &jsonParseError);
    if(jsonParseError.error != QJsonParseError::NoError)
    {
        //TODOreq: handle parse errors good. we should never see this unless the user manually modified the json
        QString eMsg = "\n";
        eMsg.append(jsonInputString);
        eMsg.append("\n");
        eMsg.append("json parse error: " + jsonParseError.errorString());
        emit e(eMsg);
        //qFatal("json parse error");
        return false;
    }
    *out_JsonObject = timeAndDataAndParentIdJsonDoc.object();
    return true;
}
bool TimeAndData_Timeline::populateTimelineEntryFromJsonObject(const QJsonObject &timeAndDataAndParentIdJson, TimeAndDataAndParentId_TimelineEntry &out_TimelineEntry)
{
    bool convertOk = false;
    QString timeString = timeAndDataAndParentIdJson.value(TimeAndData_Timeline_JSONKEY_TIME).toString();
    //out_TimelineEntry->setTime(timeString.toLongLong(&convertOk));
    qint64 time = timeString.toLongLong(&convertOk);
    if(!convertOk)
    {
        //TODOreq: handle parse errors good. we should never see this unless the user manually modified the json
        emit e("timestamp parse error: " + timeString);
        //qFatal("timestamp parse error");
        return false;
    }
    const QJsonObject &data = timeAndDataAndParentIdJson.value(TimeAndData_Timeline_JSONKEY_DATA).toObject();
    QJsonValue parentTimelineEntryIdMaybe = timeAndDataAndParentIdJson.value(TimeAndData_Timeline_JSONKEY_PARENT_TIMELINE_ENTRY_ID);
    TimelineEntryIdType parentTimelineEntryId;
    if(parentTimelineEntryIdMaybe.isUndefined())
        parentTimelineEntryId = nullAkaEmptyAkaRootTimelineEntryId();
    else
        parentTimelineEntryId = parentTimelineEntryIdMaybe.toString();
#if 0
    if(!out_TimelineEntry) //TODOoptimization: don't use heap. but I also don't want TimeAndDataAndParentId_TimelineEntry to have a default constructor xD (nvm it HAS TO have a default constructor because we use it as a "response arg type" in RequestResponse Contracts)
        out_TimelineEntry.reset(new TimeAndDataAndParentId_TimelineEntry(time, data, parentTimelineEntryId));
    else
    {
#endif
        out_TimelineEntry.setTime(time);
        out_TimelineEntry.setData(data);
        out_TimelineEntry.setParentTimelineEntryId(parentTimelineEntryId);
#if 0
    }
#endif
    return true;
}
#if 0
QString TimeAndData_Timeline::quickHashEncoded(const QByteArray &input)
{
    QByteArray hash = QCryptographicHash::hash(input, QCryptographicHash::Sha3_256);
    QString ret(hash.toHex());
    return ret;
}
#endif
TimelineEntryIdType TimeAndData_Timeline::nullAkaEmptyAkaRootTimelineEntryId() const
{
#if 0
    QByteArray dummyByteArray("dummyData");
    TimelineEntryIdType nullTimelineEntryId = quickHashEncoded(dummyByteArray);
    nullTimelineEntryId.fill('0');
    return nullTimelineEntryId;
#endif
    QJsonObject dummyObject;
    dummyObject.insert("dummyKey", "dummyValue");
    TimelineEntryIdType dummyParentTimelineEntryId("dummyParentTimelineEntryId"); //incorrectly 'sized' TimelineEntryIdType, but it doesn't matter!
    TimeAndDataAndParentId_TimelineEntry dummyEntry(0, dummyObject, dummyParentTimelineEntryId);
    TimelineEntryIdType correctlySIZEDtimelineEntryId = dummyEntry.timelineEntryId();
    correctlySIZEDtimelineEntryId.fill('0');
    return correctlySIZEDtimelineEntryId;
}
QJsonObject TimeAndDataAndParentId_TimelineEntry::toJsonObject() const
{
    //TODOoptimization: when instantiating via ::fromJsonObject, we can inspect the input json object to see if it ONLY has 3x root json entries: time/data/parent-timeline-entry-id. also gotta check for arrays n shit maybe too idk but be safe about it. so anyways yea the optimization is that we could return them the very same jsonObject that they handed us, and it would be COW! since we're potentially handing around deeply nested json structures as 'data', this is definitely desirable! So basically, in ::fromJson, we "save" the input if it's not being used to store other data (our implicit reference to it guarantees us it's const-ness: other threads would make a copy before writing!). but if there's other keys in there, fuck it, we make our own json object on the stack and "save" that one only. we wouldn't need our m_Time/m_Data/m_ParentTimelineEntryId member variables as the data would "live" in the QJsonObject that "toJsonObject" (muahahah) "returns". Inheritting from QJsonObject, as I initially was tempted to do, is too dangerous. But this hasA a QJsonObject strat/optimization is perfectly adequate. it fits nicely with the 'stale' code  stuff in the class going on as well
    //^alternatively, we could bum off the 'stale' property to keep a cached QJsonObject around xD. the above optimization is much better tho

    QJsonObject jsonObject;
    jsonObject.insert(TimeAndData_Timeline_JSONKEY_TIME, QString::number(m_Time)); //fuck human readable time formats, because without a timezone (as is often the case), they are ambiguious (yes I know Qt provides a format with timezone, but if we parse human readable then we INVITE people to change the date format as input and it will "work on their machine" without the timezone -_-. note: we store the number as a string because json can only handle int (not [necessarily] int64) and double)
    jsonObject.insert(TimeAndData_Timeline_JSONKEY_DATA, m_Data);
    jsonObject.insert(TimeAndData_Timeline_JSONKEY_PARENT_TIMELINE_ENTRY_ID, m_ParentTimelineEntryId);
    return jsonObject;
}
TimelineEntryIdType TimeAndDataAndParentId_TimelineEntry::timelineEntryId()
{
    if(m_CachedTimelineEntryId_IsStale)
    {
        m_CachedTimelineEntryId = privateGenerateTimelineEntryId();
        m_CachedTimelineEntryId_IsStale = false;
    }
    return m_CachedTimelineEntryId;
}
TimelineEntryIdType TimeAndDataAndParentId_TimelineEntry::privateGenerateTimelineEntryId() const
{
    QJsonObject jsonObject = toJsonObject();
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonByteArray = jsonDoc.toJson(QJsonDocument::Compact/*TODOreq: Indented if ever moved out of QSettings. HOWEVER, for "sha3" purposes we should maybe ALWAYS use Compact. so that "whitespace outside of keys/values" isn't factored in (but if Qt is ALWAYS the one doing the indenting, we could probably be sane about that)*/);
    QByteArray hash = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Sha3_256).toHex(); //TODOoptimization: base58 check encoding. b64 is stupid in that it has "/" in it, making it unsuitable for filenames
    TimelineEntryIdType ret(hash);
    return ret;
}
TimelineEntryIdType TimeAndDataAndParentId_TimelineEntry::parentTimelineEntryId() const
{
    return m_ParentTimelineEntryId;
}
void TimeAndDataAndParentId_TimelineEntry::setParentTimelineEntryId(const TimelineEntryIdType &newParentTimelineEntryId)
{
    m_ParentTimelineEntryId = newParentTimelineEntryId;
    m_CachedTimelineEntryId_IsStale = true;
}
QJsonObject TimeAndDataAndParentId_TimelineEntry::data() const
{
    return m_Data;
}
void TimeAndDataAndParentId_TimelineEntry::setData(const QJsonObject &newData)
{
    m_Data = newData;
    m_CachedTimelineEntryId_IsStale = true;
}
qint64 TimeAndDataAndParentId_TimelineEntry::time() const
{
    return m_Time;
}
void TimeAndDataAndParentId_TimelineEntry::setTime(const qint64 &newTime)
{
    m_Time = newTime;
    m_CachedTimelineEntryId_IsStale = true;
}
