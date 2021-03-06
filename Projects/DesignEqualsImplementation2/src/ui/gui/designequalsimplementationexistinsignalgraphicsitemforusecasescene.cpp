#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"

#include <QPen>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>

#include "designequalsimplementationguicommon.h"
#include "usecasegraphicsscene.h"
#include "designequalsimplementationslotinvokegraphicsitemforusecasescene.h"
#include "sourceexistingsignalsnappingindicationvisualrepresentation.h"
#include "signalstatementnotchmultiplextergraphicsrect.h"
#include "../../designequalsimplementationclasssignal.h"
#include "../../designequalsimplementationusecase.h"
#include "../../designequalsimplementationclasslifeline.h"

#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SIGNAL_COLOR Qt::blue

#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_NOTCH_MULTIPLEXTER_RECT_HALF_WIDTH 7.5

//is used as spacing between notches, and also above top notch and below bottom notch
#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_VERTICAL_SPACER 5

//TODOreq: snap to 'notch'. Ideally we could daisy chain infinite signals together, but the more common case and much more important is to allow a list of slots to be connected to the signal
//TODOoptional: maybe all the lines share a width, such as the 'longest width (from longest signal name)'. There are some lines that go to the same class lifeline, and some that go to other class lifelines. The width measuring would be per-classlifeline ofc. Slightly OT: I do plan on doing auto-gui, which means I don't need to serialize positions of anything (maybe class diagram view can be placed manually, but I'm leaning against use case manual placing. Placing objects so they can be read is a very time consuming task and is more or less a waste of fucking time. auto-arranging is ez tbh
//TODOoptional: when doing auto-arranging stuff, say if you rename a signal name and that drastically shortens the horizontal distance from one classlifeline to the next (assuming it has a slot going to it ofc), then we should BEFORE find the center-most item on the screen, do the rename + auto-arranging, and then AFTER center the graphics view back on that center-most item. The benefit of this is that what they were working on should still be right in front of them, and not potentially (worst case) lots of scroll screens away
//TODOreq: make m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal a snappable item (it simply proxies for the signal emit statement obviously), otherwise we can't snap to the non-first notches
DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *sourceClassLifeline, DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom, int indexStatementInsertedInto, DesignEqualsImplementationClassSignal *underlyingSignal, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_UnderlyingSignal(underlyingSignal)
    , m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal(0)
    , m_ParentUseCaseGraphicsScene(parentUseCaseGraphicsScene)
    , m_SourceClassLifeline(sourceClassLifeline)
    , m_SlotThatSignalWasEmittedFrom(slotThatSignalWasEmittedFrom)
    , m_IndexStatementInsertedInto(indexStatementInsertedInto)
{
    m_Pen = pen();
    m_Pen.setWidth(3);
    m_Pen.setColor(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SIGNAL_COLOR);
    setPen(m_Pen);

    QGraphicsTextItem *signalNameTextGraphicsItem = new QGraphicsTextItem(underlyingSignal->Name, this); //TODOoptional: visualize args of signal too? maybe not to keep it brief, since i'm setting the width of the arrow by it
    QRectF myChildrenBoundingRect = childrenBoundingRect();

    //TODOreq: draw differently depending on whether or not there is one slot connected, and of course account for multiple slots
    //TODOreq: we are a line, but the other components of the signal are children of us and can be any shape. we set it's pos relative to us though
    qreal startX = 0.0;
    if(parentItem()) //this seems hacky, but the parent calling setPos on us (a line) apparently doesn't work. i think as an exception to the rule, lines are "positioned" via their setLine
    {
        startX = parentItem()->boundingRect().width()/2; //TODOoptional: if the signal is being emitted to "the left" this would be negative
    }

    //move text to be 'above' the line
    signalNameTextGraphicsItem->setPos(startX, -myChildrenBoundingRect.height());

    //make line connect at the right edge of the slot
    setLine(QLineF(QPointF(startX, 0), QPointF(myChildrenBoundingRect.width()+startX+DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_LINE_WIDTH_MARGIN_AROUND_SIGNAL_NAME_TEXT, 0)));

    redoVisualStuffz0rz();
}
int DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID;
}
IRepresentSnapGraphicsItemAndProxyGraphicsItem *DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::makeSnappingHelperForMousePoint(QPointF eventScenePos) //mostly copy/paste job from the slot graphics item equivalent (not to be confused with slot INVOKE STATEMENT graphics item equivalent)
{
    if(m_VerticalPositionsOfSnapPoints.size() == 1)
    {
        // 0 slots attached
        IRepresentSnapGraphicsItemAndProxyGraphicsItem *sourceSnappingIndicationVisualRepresentation = new SourceExistingSignalSnappingIndicationVisualRepresentation(this, m_IndexStatementInsertedInto, this, 0);
        sourceSnappingIndicationVisualRepresentation->visualRepresentation()->setPos(QPointF(line().p2().x()/*+(sourceSnappingIndicationVisualRepresentation->visualRepresentation()->boundingRect().width()/2)*/, m_VerticalPositionsOfSnapPoints.at(0)));
        return sourceSnappingIndicationVisualRepresentation;
    }
    else //implies m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal != 0
    {
        // > 0 slots attached
        return m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->makeSnappingHelperForMousePoint(eventScenePos);
    }
    return 0;
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::underlyingSignal() const
{
    return m_UnderlyingSignal;
}
QList<qreal> DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::verticalPositionsOfSnapPoints() const
{
    return m_VerticalPositionsOfSnapPoints;
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::slotThatSignalWasEmittedFrom() const
{
    return m_SlotThatSignalWasEmittedFrom;
}
DesignEqualsImplementationClassLifeLine *DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::sourceClassLifeline() const
{
    return m_SourceClassLifeline;
}
int DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::indexStatementInsertedInto() const
{
    return m_IndexStatementInsertedInto;
}
void DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::redoVisualStuffz0rz()
{
    //TODOreq: draw slots attached to this signal emit statement. as children of the signal? other parents of slots invocation statements would be actor and slot and private method (or anything with list of statements), so idfk if I should make it parented to this signal or what
    m_VerticalPositionsOfSnapPoints.clear();
    QList<SlotConnectedToSignalTypedef*> slotsConnectedToSignal = m_SourceClassLifeline->parentUseCase()->slotsConnectedToSignal(m_SourceClassLifeline, m_SlotThatSignalWasEmittedFrom, m_IndexStatementInsertedInto);
    if(slotsConnectedToSignal.isEmpty())
    {
        //0 slots connected
        QRectF slotCircleNotchGraphicsItemRect(-DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, -DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2);
        QGraphicsEllipseItem *slotCircleNotchGraphicsItem = new QGraphicsEllipseItem(slotCircleNotchGraphicsItemRect, this);
        qreal notchY = 0.0;
        slotCircleNotchGraphicsItem->setPos(line().p2().x(), notchY);
        slotCircleNotchGraphicsItem->setPen(m_Pen);
        slotCircleNotchGraphicsItem->setBrush(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SIGNAL_COLOR);
        m_VerticalPositionsOfSnapPoints.append(notchY);
    }
    else
    {
        // > 0 slots connected
        int numSignalRhsNotches = ((slotsConnectedToSignal.size()*2)+1); //just like statements, the times two plus one gives us space for insertion before/after/in-between

        qreal notchMultiplexerRectHeight = (numSignalRhsNotches*DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2) + (numSignalRhsNotches*(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_VERTICAL_SPACER+2));
        qreal halfHeight = notchMultiplexerRectHeight/2;
        QRectF notchMultiplexerRect(-DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_NOTCH_MULTIPLEXTER_RECT_HALF_WIDTH, -halfHeight, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_NOTCH_MULTIPLEXTER_RECT_HALF_WIDTH*2.0, notchMultiplexerRectHeight);
        if(!m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal)
        {
            m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal = new SignalStatementNotchMultiplexterGraphicsRect(this, notchMultiplexerRect, this);
            m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->setPen(m_Pen);
            m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->setBrush(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SIGNAL_COLOR);
        }
        else
        {
            m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->setRect(notchMultiplexerRect);
        }
        //m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->setPos(line().p2().x(), 0);
        m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->setPos(line().p2().x()+(notchMultiplexerRect.width()/2), halfHeight);

        qreal currentNotchVerticalPositionRelativeToNotchMultiplexerRect = notchMultiplexerRect.top() + DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_VERTICAL_SPACER + DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS;

        qreal notchX = m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->rect().right()+(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2);

        bool even = true;
        int currentIndexIntoSlotsConnectedToSignal = 0;
        while(m_NotchGraphicsItems.size() < numSignalRhsNotches)
        {
            QRectF slotCircleNotchGraphicsItemRect(-DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, -DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2);
            QGraphicsEllipseItem *slotCircleNotchGraphicsItem = new QGraphicsEllipseItem(slotCircleNotchGraphicsItemRect, m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal);
            slotCircleNotchGraphicsItem->setPen(m_Pen);
            slotCircleNotchGraphicsItem->setBrush(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SIGNAL_COLOR);
            m_NotchGraphicsItems.append(slotCircleNotchGraphicsItem);
        }
        for(int i = 0; i < numSignalRhsNotches; ++i)
        {
            QGraphicsEllipseItem *slotCircleNotchGraphicsItem = m_NotchGraphicsItems.at(i);
            slotCircleNotchGraphicsItem->setPos(notchX, currentNotchVerticalPositionRelativeToNotchMultiplexerRect);

            if(even)
            {
                //even means we show a notch that CAN have a slot attached. since we draw the notch during both even and odd, we do nothing here (except toggle the bool and keep update our vertical positions mapping for snapping)
                even = false;

                m_VerticalPositionsOfSnapPoints.append(currentNotchVerticalPositionRelativeToNotchMultiplexerRect);

            }
            else
            {
                //odd means we show a notch with a slot already attached
                even = true;

                SlotConnectedToSignalTypedef *currentSlotConnectedToSignal = slotsConnectedToSignal.at(currentIndexIntoSlotsConnectedToSignal);
                DesignEqualsImplementationClassSlot *aSlotConnectedToThisSignal = currentSlotConnectedToSignal->second;
                DesignEqualsImplementationClassLifeLine *classLifelineOfSlot = m_ParentUseCaseGraphicsScene->useCase()->classLifeLines().at(currentSlotConnectedToSignal->first);

                if(m_SlotInvokeGraphicsItems.size() == currentIndexIntoSlotsConnectedToSignal)
                {
                    DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene *aSlotConnectedToThisSignalGraphicsItem = new DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(m_ParentUseCaseGraphicsScene, classLifelineOfSlot, aSlotConnectedToThisSignal, slotCircleNotchGraphicsItem);
                    m_SlotInvokeGraphicsItems.append(aSlotConnectedToThisSignalGraphicsItem);
                }
                else
                {
                    m_SlotInvokeGraphicsItems.at(currentIndexIntoSlotsConnectedToSignal)->reset(m_ParentUseCaseGraphicsScene, classLifelineOfSlot, aSlotConnectedToThisSignal);
                }

                //DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene *aSlotConnectedToThisSignalGraphicsItem = new DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(parentUseCaseGraphicsScene, classLifelineOfSlot, aSlotConnectedToThisSignal, this);
                //aSlotConnectedToThisSignalGraphicsItem->setPos(notchX, currentNotchVerticalPositionRelativeToNotchMultiplexerRect);

                ++currentIndexIntoSlotsConnectedToSignal;
            }

            currentNotchVerticalPositionRelativeToNotchMultiplexerRect += (DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS + DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_VERTICAL_SPACER + DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS);
        }
    }
}
int DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::getInsertSubIndexForMouseScenePos(QPointF eventScenePos)
{
    return m_VerticalPositionsOfSnapPoints.size()-1;

#if 0 //old: whenever the signal line is targetted, we always use the 'last' index available
    if(m_VerticalPositionsOfSnapPoints.size() == 1)
    {
        // 0 slots attached
        return 0;
    }
    else
    {
        // > 0 slots attached
        return m_
        //return m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->getInsertSubIndexForMouseScenePos(eventScenePos);
    }
#endif
}
qreal DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::calculateHeightPossiblyIncludingNotchMultiplexer()
{
    qreal ret = 0.0;
    if(m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal) //TODOreq: not sure this is valid yet, but shit maybe it is...
    {
        ret += m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal->boundingRect().height();
    }
    return ret;
}
