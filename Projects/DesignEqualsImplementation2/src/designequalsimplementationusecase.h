#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QPointF>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationProject;
class SignalSlotCombinedEventHolder;
class DesignEqualsImplementationClassInstance;
class DesignEqualsImplementationActor;
class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;

typedef QPair<int /*index into class lifelines. (NOPE, the list is connection/instance apathetic: shouldn't this always be zero? why do we even have the class lifeline indexes? don't we just need the first and from there we can recurse/iterate?)*/, DesignEqualsImplementationClassSlot*/*the slot entry point*/> UseCaseEntryPointTypedef;

class DesignEqualsImplementationUseCase : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCase(QObject *parent = 0);
    explicit DesignEqualsImplementationUseCase(DesignEqualsImplementationProject *project, QObject *parent = 0);

    enum UseCaseEventTypeEnum //TODOoptional: move to guicommon, since also used as dialog mode
    {
        UseCaseSlotEventType,
        UseCaseSignalEventType,
        UseCaseSignalSlotEventType
    };

    DesignEqualsImplementationProject *designEqualsImplementationProject() const;
    QList<DesignEqualsImplementationClassLifeLine*> classLifeLines() const;

    //TODOoptional: private + getter/setter blah
    QString Name;
    //QList<QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> > SemiOrderedUseCaseEvents;
    //QList<DesignEqualsImplementationClassSlot*> m_Slots
    //DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    //DesignEqualsImplementationClassSignal *ExitSignal;

    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    struct SignalSlotConnectionActivationTypeStruct
    {
        //Signal
        int SignalStatement_Key0_IndexInto_m_ClassLifeLines;
        DesignEqualsImplementationClassSlot *SignalStatement_Key1_SourceSlotItself;
        int SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements;

        //Slots
        int SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines;
        DesignEqualsImplementationClassSlot *SlotInvokedThroughConnection_Key1_DestinationSlotItself;
    };

    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    UseCaseEntryPointTypedef m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsNegativeOneIfNoneConnectedFromActorYet;
    DesignEqualsImplementationClassSlot *m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements;
    int m_ExitSignalsIndexIntoOrderedListOfStatements;

    //Interfaces:
    //QList<DesignEqualsImplementationSignalSlotInterface*> m_CustomizableButAutoGeneratedInterfacesFromImpl; //correlates with, as in contains the signalslotinterface bases of, m_UnitsOfExecutionMakingApperanceInUseCase. since the qlist qpair just below me has a key that goes to my list, it might just fit into m_UnitsOfExecutionMakingApperanceInUseCase as well
    //Instead of the above, we could do
    //QList<QPair<DesignEqualsImplementationSignalSlotInterface* /*Interface*/, DesignEqualsImplementationClassLifeLineUnitOfExecution* /*Impl*/> > m_InterfacesAndImplementationsForThisUseCase; //implies that use case keeps track of implementations to satisfy the interfaces, but needn't (could query "project" for them at generate time and fail if they aren't satisfied). is there any benefit to project keeping track of them? eh makes use case more modular arguably, but having an implementation handy is sometimes the most difficult thing (but shouldn't be if you code other areas properly (and of course share code))
    //QList<QPair<int /*IndexIntoInterfacesList*/, int /*IndexIndtoInterfaceStatementsListOfTheEmitStatementThatCorrelatesWithTheSlotUnitOfExecutionEntryPointOfSomeOtherUnitOfExecutionInAboveList (TODOreq: keep this syncrhronized with above what it correlates to)*/> > m_A_SIGNAL_SLOT_ACTIVATION_IN_THIS_USE_CASE; //key goes to above list (the commented out one(s)). now it goes to m_SlotsMakingApperanceInUseCase only
    QList<SignalSlotConnectionActivationTypeStruct> m_SignalSlotConnectionActivationsInThisUseCase; //TODOreq: keep synchronized with inserts/etc xD

    //Implementations (any kind of relationship you desire ("bundled" with impl, generated-when-needed, etc) with interface, including none (project could supply them to use cases, who only ever know of interfaces)):
    QList<DesignEqualsImplementationClassLifeLine*> m_ClassLifeLines;
    //QList<DesignEqualsImplementationClassSlot*> m_SlotsMakingApperanceInUseCase; //design ordered top down apperances. will probably use two PointFs in the future for more accurate serializing
    //QList<DesignEqualsImplementationClassLifeLine*> m_ClassLifeLinesAppearingInThisUseCase;

    void privateConstructor(DesignEqualsImplementationProject *project);

    void insertEventPrivate(UseCaseEventTypeEnum useCaseEventType, int indexToInsertStatementInto, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSlot *destinationSlot_OrZeroIfDestIsActorOrEventIsPlainSignal, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot, int signalSlotActivation_ONLY_indexInto_m_ClassLifeLines_OfSignal = -1, int signalSlotActivation_ONLY_indexInto_m_ClassLifeLines_ofSlot = -1);
    //void addEventPrivateWithoutUpdatingExitSignal(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot = SignalEmissionOrSlotInvocationContextVariables());
    void insertAlreadyFilledOutSlotIntoUseCase(DesignEqualsImplementationClassSlot *slotEntryPointThatKindaSortaMakesItNamed);
    void addClassLifeLineToUseCase(DesignEqualsImplementationClassLifeLine *classLifeLineToAddToUseCase);
signals:
    void actorAdded(DesignEqualsImplementationActor *actor);
    void classLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);

    void slotInvokeEventAdded(DesignEqualsImplementationClassSlot *slotUseCaseEvent);
    void signalEmitEventAdded(DesignEqualsImplementationClassSignal *signalUseCaseEvent);
    void signalSlotEventAdded(SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent);

    void eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);

    void e(const QString &);
public slots:
    void addActorToUseCase(QPointF position);
    void addClassInstanceToUseCaseAsClassLifeLine(DesignEqualsImplementationClass *classToAddToUseCase, QPointF position);
    void insertSlotInvocationEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    void insertSignalSlotActivationEvent(int indexToInsertEventAt, DesignEqualsImplementationClassSlot *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables, int indexInto_m_ClassLifeLines_OfSignal, int indexInto_m_ClassLifeLines_OfSlot);
    void insertSignalEmitEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables);

    void setUseCaseSlotEntryPoint(int classLifeLinesIndex, DesignEqualsImplementationClassSlot *useCaseSlotEntryPoint);
    void setExitSignal(DesignEqualsImplementationClassSlot *sourceSlot, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables);
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase);

typedef QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> UseCaseEventListEntryType; //OrderedUseCaseEvents has a copy/paste of this type, because I couldn't forward declare it or whatever (wtf)


class SignalSlotCombinedEventHolder : public QObject
{
    Q_OBJECT
public:
    explicit SignalSlotCombinedEventHolder(QObject *parent = 0) : QObject(parent) { }
    explicit SignalSlotCombinedEventHolder(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, QObject *parent = 0)
        : QObject(parent)
        , m_DesignEqualsImplementationClassSignal(designEqualsImplementationClassSignal)
        , m_DesignEqualsImplementationClassSlot(designEqualsImplementationClassSlot)
    { }
    ~SignalSlotCombinedEventHolder() { /*these two combined children are deleted by class diagram perspective: delete m_DesignEqualsImplementationClassSignal; delete m_DesignEqualsImplementationClassSlot;*/ }
    DesignEqualsImplementationClassSignal *m_DesignEqualsImplementationClassSignal;
    DesignEqualsImplementationClassSlot *m_DesignEqualsImplementationClassSlot;
};
QDataStream &operator<<(QDataStream &out, const SignalSlotCombinedEventHolder &useCase);
QDataStream &operator>>(QDataStream &in, SignalSlotCombinedEventHolder &useCase);

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASE_H
