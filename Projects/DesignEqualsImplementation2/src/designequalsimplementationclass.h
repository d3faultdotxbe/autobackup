#ifndef DESIGNEQUALSIMPLEMENTATIONCLASS_H
#define DESIGNEQUALSIMPLEMENTATIONCLASS_H

#include <QObject>
#include "idesignequalsimplementationvisuallyrepresenteditem.h"

#include <QPair>

#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassprivatemethod.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "hasa_private_classes_member.h"

#define DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX "failed to generate source for: "

class DesignEqualsImplementationProject;

typedef QPair<QString /*type*/, QString /*name*/> MethodArgumentTypedef; //derp defined like twenty different places...
//typedef QPair<QString /*member variable name*/, DesignEqualsImplementationClass* /*member variable type*/> HasA_Private_Classes_Members_ListEntryType;

//TODOinstancing: class DesignEqualsImplementationClassInstance;

class DesignEqualsImplementationClass : public QObject, public IDesignEqualsImplementationVisuallyRepresentedItem
{
    Q_OBJECT
public:
    //static DesignEqualsImplementationClassInstance *createClassInstance(DesignEqualsImplementationClassInstance *parent = 0, const QString &optionalVariableName = QString());
    static QString generateRawConnectStatementWithEndingSemicolon(const QString &signalObjectVariableName, const QString &signalNameIncludingNormalizedArgs, const QString &slotObjectVariableName, const QString &slotNameIncludingNormalizedArgs);

    explicit DesignEqualsImplementationClass(QObject *parent = 0, DesignEqualsImplementationProject *parentProject = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    virtual ~DesignEqualsImplementationClass();

    DesignEqualsImplementationClassSignal *createNewSignal(const QString &newSignalName = QString(), const QList<MethodArgumentTypedef> &newSignalArgs = QList<MethodArgumentTypedef>());
    void addSignal(DesignEqualsImplementationClassSignal *signalToAdd);

    DesignEqualsImplementationClassSlot *createwNewSlot(const QString &newSlotName = QString(), const QList<MethodArgumentTypedef> &newSlotArgs = QList<MethodArgumentTypedef>());
    void addSlot(DesignEqualsImplementationClassSlot *slotToAdd);
    void removeSlot(DesignEqualsImplementationClassSlot *slotToRemove);

    //TODOoptional: private + getter/setter blah
    DesignEqualsImplementationProject *m_ParentProject;
    QString ClassName;

    DesignEqualsImplementationClassProperty *createNewProperty(const QString &propertyType, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);
    void addProperty(DesignEqualsImplementationClassProperty *propertyToAdd);

    HasA_Private_Classes_Member *createHasA_Private_Classes_Member(DesignEqualsImplementationClass *hasA_Private_Class_Member, const QString &variableName);
    QList<HasA_Private_Classes_Member *> hasA_Private_Classes_Members();
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSignal*> mySignals();
    QList<DesignEqualsImplementationClassSlot*> mySlots();

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QString headerFilenameOnly();


    //TODOoptional: should be private
    QList<DesignEqualsImplementationClassProperty*> Properties;
    QList<HasA_Private_Classes_Member*> m_HasA_Private_Classes_Members;
    QList<DesignEqualsImplementationClassSignal*> m_MySignals;
    QList<DesignEqualsImplementationClassSlot*> m_MySlots;

    inline int serializationSlotIdForSlot(DesignEqualsImplementationClassSlot *theSlot) { return m_MySlots.indexOf(theSlot); }
    inline DesignEqualsImplementationClassSlot *slotInstantiationFromSerializedSlotId(int slotId) { return m_MySlots.at(slotId); }
private:
    friend class DesignEqualsImplementationProjectGenerator;
signals:
    void propertyAdded(DesignEqualsImplementationClassProperty*);
    void hasAPrivateMemberClassAdded(HasA_Private_Classes_Member*); //declare meta type?
    void privateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void slotAdded(DesignEqualsImplementationClassSlot*);
    void signalAdded(DesignEqualsImplementationClassSignal*);
    void e(const QString &);
public slots:
    void emitAllClassDetails();
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass);

Q_DECLARE_METATYPE(DesignEqualsImplementationClass*)

#if 0 //TODOinstancing
class DesignEqualsImplementationClassInstance : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit DesignEqualsImplementationClassInstance()
        : IHaveTypeAndVariableNameAndPreferredTextualRepresentation()
        , m_InstanceType(NoInstanceChosen)
        , m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline(0) { }
    explicit DesignEqualsImplementationClassInstance(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClass *parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, const QString &variableName)
        : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName)
        , m_MyClass(designEqualsImplementationClass)
        , m_InstanceType(parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline ? ChildMemberOfOtherClassLifeline : UseCasesRootClassLifeline)
        , m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline(parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline)
    {
        //m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.second = myIndexIntoParentHasAThatIsMe;
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.removeAll(this);
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.append(this);
        }
#endif
    }
    virtual ~DesignEqualsImplementationClassInstance()
    {
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.removeAll(this);
        }
#endif
    }
    virtual QString typeString()
    {
        return m_MyClass->ClassName + " *";
    }

    DesignEqualsImplementationClass *m_MyClass;
    //QPair<DesignEqualsImplementationClass*, int> m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline;
    DesignEqualsImplementationClassInstanceTypeEnum m_InstanceType;
    DesignEqualsImplementationClass *m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);

Q_DECLARE_METATYPE(DesignEqualsImplementationClassInstance*)
#endif

#endif // DESIGNEQUALSIMPLEMENTATIONCLASS_H
