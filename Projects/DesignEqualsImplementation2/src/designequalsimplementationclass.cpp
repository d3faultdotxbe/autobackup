#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include <QMutexLocker>
#include "designequalsimplementation.h"

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationsignalemissionstatement.h"

#define DesignEqualsImplementationClass_QDS(qds, direction, designEqualsImplementationClass) \
qds direction designEqualsImplementationClass.Properties; \
qds direction designEqualsImplementationClass.m_HasA_Private_Classes_Members; \
qds direction designEqualsImplementationClass.m_HasA_Private_PODorNonDesignedCpp_Members; \
qds direction designEqualsImplementationClass.PrivateMethods; \
qds direction designEqualsImplementationClass.m_MySlots; \
qds direction designEqualsImplementationClass.m_MySignals; \
return qds;

#define HasA_Private_Classes_Members_ListEntryType_QDS(qds, direction, hasA_Private_Classes_Members_ListEntryType) \
qds direction hasA_Private_Classes_Members_ListEntryType.VariableName; \
qds direction *hasA_Private_Classes_Members_ListEntryType.m_MyClass; \
return qds;

#define HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(qds, direction, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType) \
qds direction hasA_Private_PODorNonDesignedCpp_Members_ListEntryType.VariableName; \
qds direction hasA_Private_PODorNonDesignedCpp_Members_ListEntryType.Type; \
return qds;

//TODOoptional: auto-pimpl, since pimpl is cheap/awesome (and gives us implicit sharing when done properly) and increases source/binary compatibility. MAYBE it should be opt-in, but probably opt-out instead?
#if 0
DesignEqualsImplementationClassInstance *DesignEqualsImplementationClass::createClassInstance(DesignEqualsImplementationClassInstance *parent, const QString &optionalVariableName) //top-level objects don't need a variable name if there's only one use case being generated in lib mode, for example
{
    return new DesignEqualsImplementationClassInstance(this, parent, optionalVariableName);
}
#endif
QString DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(const QString &signalObjectVariableName, const QString &signalNameIncludingNormalizedArgs, const QString &slotObjectVariableName, const QString &slotNameIncludingNormalizedArgs)
{
    return QString("connect(" + signalObjectVariableName + ", SIGNAL(" + signalNameIncludingNormalizedArgs + "), " +  slotObjectVariableName + ", SLOT(" + slotNameIncludingNormalizedArgs + "));");
}
DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationVisuallyRepresentedItem()
{ }
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    //Q_FOREACH(HasA_Private_Classes_Members_ListEntryType *currentMember, HasA_PrivateMemberClasses)
    //{
    //    delete currentMember->m_DesignEqualsImplementationClass;
    //}
    qDeleteAll(m_HasA_Private_Classes_Members);
    qDeleteAll(m_HasA_Private_PODorNonDesignedCpp_Members);
    qDeleteAll(PrivateMethods);
    qDeleteAll(m_MySlots);
    qDeleteAll(m_MySignals);
}
HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *DesignEqualsImplementationClass::createNewHasAPrivate_PODorNonDesignedCpp_Member(const QString &typeString, const QString &variableName)
{
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *newHasA_Private_PODorNonDesignedCpp_Members_ListEntryType = new HasA_Private_PODorNonDesignedCpp_Members_ListEntryType(typeString, variableName);
    m_HasA_Private_PODorNonDesignedCpp_Members.append(newHasA_Private_PODorNonDesignedCpp_Members_ListEntryType);
    return newHasA_Private_PODorNonDesignedCpp_Members_ListEntryType;
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClass::createNewSignal(const QString &newSignalName)
{
    DesignEqualsImplementationClassSignal *newSignal = new DesignEqualsImplementationClassSignal(this);
    newSignal->Name = newSignalName;
    newSignal->ParentClass = this;
    m_MySignals.append(newSignal);
    emit signalAdded(newSignal);
    return newSignal;
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationClass::createwNewSlot(const QString &newSlotName)
{
    DesignEqualsImplementationClassSlot *newSlot = new DesignEqualsImplementationClassSlot(this);
    newSlot->Name = newSlotName;
    addSlot(newSlot);
    return newSlot;
}
void DesignEqualsImplementationClass::addSlot(DesignEqualsImplementationClassSlot *slotToAdd)
{
    //connect(slotToAdd, SIGNAL(e(QString)))
    m_MySlots.append(slotToAdd);
    slotToAdd->ParentClass = this;
    emit slotAdded(slotToAdd);
}
void DesignEqualsImplementationClass::removeSlot(DesignEqualsImplementationClassSlot *slotToRemove)
{
    m_MySlots.removeOne(slotToRemove);
    slotToRemove->ParentClass = 0; //TODOreq: a slot without a parent is undefined
    //emit slotRemoved(slotToRemove);
}
HasA_Private_Classes_Member *DesignEqualsImplementationClass::createHasA_Private_Classes_Member(DesignEqualsImplementationClass *hasA_Private_Class_Member, const QString &variableName)
{
    //TODOreq: ensure all callers haven't already done the "new"

    //TODOinstancing: DesignEqualsImplementationClassInstance *newInstance = new DesignEqualsImplementationClassInstance(hasA_Private_Class_Member, this, variableName);
    //m_HasA_Private_Classes_Members.append(newInstance);
    HasA_Private_Classes_Member *newMember = new HasA_Private_Classes_Member(); //TODOoptional: all these properties should maybe be required as constructor args
    newMember->m_MyClass = hasA_Private_Class_Member;
    newMember->VariableName = variableName;
    newMember->setParentClass(this);
    m_HasA_Private_Classes_Members.append(newMember); //TODOreq: re-ordering needs to resynchronize

    return newMember;
}
QList<HasA_Private_Classes_Member*> DesignEqualsImplementationClass::hasA_Private_Classes_Members()
{
    return m_HasA_Private_Classes_Members;
}
QList<HasA_Private_PODorNonDesignedCpp_Members_ListEntryType*> DesignEqualsImplementationClass::hasA_Private_PODorNonDesignedCpp_Members()
{
    return m_HasA_Private_PODorNonDesignedCpp_Members;
}
QList<DesignEqualsImplementationClassSignal *> DesignEqualsImplementationClass::mySignals()
{
    return m_MySignals;
}
QList<DesignEqualsImplementationClassSlot*> DesignEqualsImplementationClass::mySlots()
{
    return m_MySlots;
}
//Hmm now that I come to actually using this, why lose the pointers and resort to strings :)? Only thing though is that I need to refactor so that hasAClasses, properties, and localVariables(undefined-atm) all derive from some shared base "variable" xD. And shit local variables won't be introduced until C++ drop down mode is implemented (or at least designed), and even then they still might not ever show up in a slot-unit-of-execution-thingo.
#if 0
QList<QString> DesignEqualsImplementationClass::allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString()
{
    //Properties and PrivateMembers
    QList<QString> ret;

    //TODOreq: properties. i'm not too familiar with Q_PROPERTY, even though obviously i want to support it. i'm not sure, but i think i want the "read" part of the property here...

    Q_FOREACH(HasA_Private_Classes_Members_ListEntryType currentMember, HasA_PrivateMemberClasses)
    {
        ret.append(currentMember.VariableName);
    }
    return ret;
}
#endif
QString DesignEqualsImplementationClass::headerFilenameOnly()
{
    return ClassName.toLower() + ".h";
}
void DesignEqualsImplementationClass::emitAllClassDetails()
{
    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(out, <<, designEqualsImplementationClass);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(in, >>, designEqualsImplementationClass);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    return out << *designEqualsImplementationClass;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    designEqualsImplementationClass = new DesignEqualsImplementationClass();
    return in >> *designEqualsImplementationClass;
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_Classes_Member &hasA_Private_Classes_Members_ListEntryType)
{
    HasA_Private_Classes_Members_ListEntryType_QDS(out, <<, hasA_Private_Classes_Members_ListEntryType)
}
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member &hasA_Private_Classes_Members_ListEntryType)
{
    HasA_Private_Classes_Members_ListEntryType_QDS(in, >>, hasA_Private_Classes_Members_ListEntryType)
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_Classes_Member *&hasA_Private_Classes_Members_ListEntryType)
{
    return out << *hasA_Private_Classes_Members_ListEntryType;
}
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Member *&hasA_Private_Classes_Members_ListEntryType)
{
    hasA_Private_Classes_Members_ListEntryType = new HasA_Private_Classes_Member();
    return in >> *hasA_Private_Classes_Members_ListEntryType;
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(out, <<, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
}
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(in, >>, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    return out << *hasA_Private_PODorNonDesignedCpp_Members_ListEntryType;
}
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    hasA_Private_PODorNonDesignedCpp_Members_ListEntryType = new HasA_Private_PODorNonDesignedCpp_Members_ListEntryType();
    return in >> *hasA_Private_PODorNonDesignedCpp_Members_ListEntryType;
}
