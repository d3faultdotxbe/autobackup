#ifndef ISNAPPABLESOURCEGRAPHICSITEM_H
#define ISNAPPABLESOURCEGRAPHICSITEM_H

#include <QPointF>

class IRepresentSnapGraphicsItemAndProxyGraphicsItem;

class ISnappableSourceGraphicsItem
{
public:
    virtual IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForMousePoint(QPointF eventScenePos)=0;
};

#endif // ISNAPPABLESOURCEGRAPHICSITEM_H
