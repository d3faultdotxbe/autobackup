#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationprojectgenerator.h" //only for a few string utils (/lazy)

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
DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent, DesignEqualsImplementationProject *parentProject)
    : Type(parent)
    , IDesignEqualsImplementationVisuallyRepresentedItem()
    , m_ParentProject(parentProject)
{ }
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(PrivateMethods);
    qDeleteAll(m_MySlots);
    qDeleteAll(m_MySignals);
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClass::createNewSignal(const QString &newSignalName, const QList<FunctionArgumentTypedef> &newSignalArgs)
{
    DesignEqualsImplementationClassSignal *newSignal = new DesignEqualsImplementationClassSignal(this);
    newSignal->Name = newSignalName;
    Q_FOREACH(const FunctionArgumentTypedef &newSignalArg, newSignalArgs)
    {
        newSignal->createNewArgument(m_ParentProject->getOrCreateTypeFromName(newSignalArg.NonQualifiedType), newSignalArg.QualifiedType, newSignalArg.Name);
    }
    addSignal(newSignal);
    return newSignal;
}
void DesignEqualsImplementationClass::addSignal(DesignEqualsImplementationClassSignal *signalToAdd)
{
    signalToAdd->ParentClass = this;
    m_MySignals.append(signalToAdd);
    emit signalAdded(signalToAdd);
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationClass::createwNewSlot(const QString &newSlotName, const QList<FunctionArgumentTypedef> &newSlotArgs)
{
    DesignEqualsImplementationClassSlot *newSlot = new DesignEqualsImplementationClassSlot(this);
    newSlot->Name = newSlotName;
    Q_FOREACH(const FunctionArgumentTypedef &newSlotArg, newSlotArgs)
    {
        newSlot->createNewArgument(m_ParentProject->getOrCreateTypeFromName(newSlotArg.NonQualifiedType), newSlotArg.QualifiedType, newSlotArg.Name);
    }
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
void DesignEqualsImplementationClass::addNonFunctionMember(NonFunctionMember *nonFunctionMember)
{
    //DesignEqualsImplementationClass accepts any kind of NonFunctionMember
    addNonFunctionMemberPrivate(nonFunctionMember);
}
DesignEqualsImplementationClassProperty *DesignEqualsImplementationClass::createNewProperty(Type *propertyType, const QString &qualifiedTypeString, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange)
{
    DesignEqualsImplementationClassProperty *newProperty = new DesignEqualsImplementationClassProperty(propertyType, qualifiedTypeString, propertyName, this, this, hasInit, optionalInit, readOnly, notifiesOnChange);
    addNonFunctionMember(newProperty);
    return newProperty;
}
QList<DesignEqualsImplementationClassProperty*> DesignEqualsImplementationClass::properties() const
{
    //TODOoptimization: if callers call this multiple times in a row, they should re-use the first call. a different way to optimize yet is for the callers to do the qobject_cast'ing, but not all uses would warrant that (but if we're already iterating nonfunctionmembers, for example)
    QList<DesignEqualsImplementationClassProperty*> ret;
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, nonFunctionMembers())
    {
        if(DesignEqualsImplementationClassProperty *memberAsPropertyMaybe = qobject_cast<DesignEqualsImplementationClassProperty*>(currentNonFunctionMember))
        {
            ret << memberAsPropertyMaybe;
        }
    }
    return ret;
}
#if 0
HasA_Private_Classes_Member *DesignEqualsImplementationClass::createHasA_Private_Classes_Member(DesignEqualsImplementationClass *memberClassType, const QString &variableName_OrLeaveBlankForAutoNumberedVariableName)
{
    //TODOreq: ensure all callers haven't already done the "new"

    //TODOinstancing: DesignEqualsImplementationClassInstance *newInstance = new DesignEqualsImplementationClassInstance(hasA_Private_Class_Member, this, variableName);
    //m_HasA_Private_Classes_Members.append(newInstance);

    QString chosenVariableName = variableName_OrLeaveBlankForAutoNumberedVariableName;
    if(variableName_OrLeaveBlankForAutoNumberedVariableName.trimmed().isEmpty())
    {
        chosenVariableName = autoNameForNewChildMemberOfType(memberClassType);
    }

    HasA_Private_Classes_Member *newMember = new HasA_Private_Classes_Member(); //TODOoptional: all these properties should maybe be required as constructor args
    newMember->m_MyClass = memberClassType;
    newMember->VariableName = chosenVariableName;
    newMember->setParentClass(this);
    m_NonFunctionMembers.append(newMember); //TODOreq: re-ordering needs to resynchronize

    return newMember;
}
#endif
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
QString DesignEqualsImplementationClass::autoNameForNewChildMemberOfType(DesignEqualsImplementationClass *childMemberClassType) //TODOreq: check for collission with properties and signals/slots too, which are all technically "members" so I may refactor to account for that fact
{
    int indexCurrentlyTestingForNameCollission = -1;
    while(true)
    {
        QString maybeVariableName = "m_" + DesignEqualsImplementationProjectGenerator::firstCharacterToUpper(childMemberClassType->Name) + QString::number(++indexCurrentlyTestingForNameCollission); //m_Foo0, m_Foo1, etc. TODOoptional: random 5 letter word from dictionary chosen, append two numbers also, so they are easier to differentiate/remember when using auto mode (although i probably won't use it myself (unless i'm in a rush)). //TODooptional: should the first m_Foo have a zero on the end or no? I'd say yes keep the zero, just makes it simpler LATER
        bool seenThisTime = false;
        Q_FOREACH(NonFunctionMember *currentNonFunctionMember, nonFunctionMembers())
        {
            if(currentNonFunctionMember->VariableName == maybeVariableName)
            {
                seenThisTime = true;
                break;
            }
        }
        if(!seenThisTime)
        {
            return maybeVariableName;
        }
    }
}
QString DesignEqualsImplementationClass::nextTempUnnamedSlotName()
{
    QString ret;
    int i = -1;
    do
    {
        ret = UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX + QString::number(++i);
    }
    while(slotWithThisNameExists(ret));
    return ret;
}
void DesignEqualsImplementationClass::setClassName(const QString &newClassName)
{
    Name = newClassName;
}
void DesignEqualsImplementationClass::emitAllClassDetails()
{
    //TODOreq
}
