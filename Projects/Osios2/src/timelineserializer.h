#ifndef TIMELINESERIALIZER_H
#define TIMELINESERIALIZER_H

#include <QObject>

#include "itimelinenode.h"

class QIODevice;

class Osios;

class TimelineSerializer : public QObject
{
    Q_OBJECT
public:
    static ITimelineNode *peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(QIODevice *ioDeviceToDeserializeFrom);

    explicit TimelineSerializer(QObject *parent = 0);
#if 0
    bool serializeTimelineToDisk(Osios *osiosContainingTimelineToBeSerialized, QIODevice *ioDeviceToSerializeTo);
    bool deserializeTimelineFromDisk(Osios *osiosContainingTimelineToBeSerialized, QIODevice *ioDeviceToDeserializeFrom);
#endif
private:
    static ITimelineNode *instantiateDerivedTimelineNodeTypeByTypeId(TimelineNodeTypeEnum::TimelineNodeTypeEnumActual timelineNodeTypeId);
};

#endif // TIMELINESERIALIZER_H
