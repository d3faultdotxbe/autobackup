#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include <QMutexLocker>
#include "designequalsimplementation.h"

#include "designequalsimplementationcommon.h"

#define DesignEqualsImplementationClass_QDS(qds, direction, designEqualsImplementationClass) \
qds direction designEqualsImplementationClass.Properties; \
qds direction designEqualsImplementationClass.HasA_PrivateMemberClasses; \
qds direction designEqualsImplementationClass.PrivateMethods; \
qds direction designEqualsImplementationClass.Slots; \
qds direction designEqualsImplementationClass.Signals; \
return qds;

#define HasA_PrivateMemberClasses_ListEntryType_QDS(qds, direction, hasA_PrivateMemberClasses_ListEntryType) \
qds direction hasA_PrivateMemberClasses_ListEntryType.VariableName; \
qds direction *hasA_PrivateMemberClasses_ListEntryType.m_DesignEqualsImplementationClass; \
return qds;

DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationVisuallyRepresentedItem()
{ }
bool DesignEqualsImplementationClass::generateSourceCode(const QString &destinationDirectoryPath)
{
    //Header
    QString headerFilePath = destinationDirectoryPath + headerFilenameOnly();
    QFile headerFile(headerFilePath);
    if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open header for writing: " + headerFilePath);
        return false;
    }
    QTextStream headerFileTextStream(&headerFile);

    //Source
    QString sourceFilePath = destinationDirectoryPath + ClassName.toLower() + ".cpp";
    QFile sourceFile(sourceFilePath);
    if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open source for writing: " + sourceFilePath);
        return false;
    }
    QTextStream sourceFileTextStream(&sourceFile);

#if 0 //endl vs. "\n"
    //PrivateMembers
    //comes before both source/header headers because needs to append/insert forward declares (header) and includes (source)
    QString headerForwardDeclares; //thought about using a QList<QString> and doing the formatting in "Header's header", but it really makes no difference so KISS
    Q_FOREACH(PrivateMemberType currentPrivateMember, PrivateMembers)
    {
        //Forward declares
        headerForwardDeclares.append("class " + )
    }
#endif

    //Header's header (wat) + constructor
    QString myNameHeaderGuard = ClassName.toUpper() + "_H";
    headerFileTextStream    << "#ifndef " << myNameHeaderGuard << endl
                            << "#define " << myNameHeaderGuard << endl
                            << endl
                            << "#include <QObject>" << endl //TODOoptional: non-QObject classes? hmm nah because signals/slots based
                            << endl;
    //Header's header's forward declares
    bool atLeastOneHasAPrivateMemberClass = !HasA_PrivateMemberClasses.isEmpty(); //spacing
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentPrivateMember, HasA_PrivateMemberClasses)
    {
        //class Bar;
        headerFileTextStream << "class " << currentPrivateMember->m_DesignEqualsImplementationClass->ClassName << ";" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        headerFileTextStream << endl; //OCD <3
    headerFileTextStream    << "class " << ClassName << " : public QObject" << endl
                            << "{" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl
                            << "public:" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << ClassName << "(QObject *parent = 0);" << endl;
                            //<< DESIGNEQUALSIMPLEMENTATION_TAB << "~" << Name << "();" << endl;
    //Header's hasAPrivateMemberClass declarations
    if(atLeastOneHasAPrivateMemberClass)
        headerFileTextStream << "private:" << endl;
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentPrivateMember, HasA_PrivateMemberClasses)
    {
        //Bar *m_Bar;
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentPrivateMember->preferredTextualRepresentation() << ";" << endl;
    }

    //Source's header+constructor (the top bits, not the ".h" counter-part)
    sourceFileTextStream    << "#include \"" << headerFilenameOnly() << "\"" << endl
                            << endl;
    //Source's header PrivateMemberClasses includes
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentPrivateMember, HasA_PrivateMemberClasses)
    {
        //#include "bar.h"
        sourceFileTextStream << "#include \"" << currentPrivateMember->m_DesignEqualsImplementationClass->headerFilenameOnly() << "\"" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        sourceFileTextStream << endl;
    sourceFileTextStream    << ClassName << "::" << ClassName << "(QObject *parent)" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl;
    //Source's header PrivateMemberClasses constructor initializers
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentPrivateMember, HasA_PrivateMemberClasses)
    {
        //, m_Bar(new Bar(this))
        sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ", " << currentPrivateMember->VariableName << "(new " << currentPrivateMember->m_DesignEqualsImplementationClass->ClassName << "(this))" << endl; //TODOreq: for now all my objects need a QObject *parent=0 constructor, but since that's also a [fixable] requirement for my ObjectOnThreadGroup, no biggy. Still, would be nice to solve the threading issue and to allow constructor args here (RAII = pro)
    }
    sourceFileTextStream    << "{ }" << endl;

    //Signals
    if(!Signals.isEmpty())
        headerFileTextStream << "signals:" << endl;
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, Signals)
    {
        //void fooSignal();
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSignal->methodSignatureWithoutReturnType() << ";" << endl;
    }

    //Slots
    if(!Slots.isEmpty())
        headerFileTextStream << "public slots:" << endl;
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, Slots)
    {
        //Declare slot
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSlot->methodSignatureWithoutReturnType() << ";" << endl;
        //Define slot
        sourceFileTextStream    << "void " << ClassName << "::" << currentSlot->methodSignatureWithoutReturnType() << endl
                                << "{" << endl;
        Q_FOREACH(IDesignEqualsImplementationStatement *currentSlotCurrentStatement, currentSlot->OrderedListOfStatements)
        {
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentSlotCurrentStatement->toRawCppWithEndingSemicolon() << endl;
        }
        sourceFileTextStream    << "}" << endl;
    }

    //Header's footer
    headerFileTextStream    << "};" << endl
                            << endl
                            << "#endif // " << myNameHeaderGuard << endl;

    //Recursively generate source for all children HasA_PrivateMemberClasses
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentPrivateMember, HasA_PrivateMemberClasses)
    {
        if(!currentPrivateMember->m_DesignEqualsImplementationClass->generateSourceCode(destinationDirectoryPath))
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX + currentPrivateMember->m_DesignEqualsImplementationClass->ClassName);
            return false;
        }
    }

    return true;
}
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    //Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType *currentMember, HasA_PrivateMemberClasses)
    //{
    //    delete currentMember->m_DesignEqualsImplementationClass;
    //}
    qDeleteAll(HasA_PrivateMemberClasses);
    qDeleteAll(PrivateMethods);
    qDeleteAll(Slots);
    qDeleteAll(Signals);
}
//Hmm now that I come to actually using this, why lose the pointers and resort to strings :)? Only thing though is that I need to refactor so that hasAClasses, properties, and localVariables(undefined-atm) all derive from some shared base "variable" xD. And shit local variables won't be introduced until C++ drop down mode is implemented (or at least designed), and even then they still might not ever show up in a slot-unit-of-execution-thingo.
#if 0
QList<QString> DesignEqualsImplementationClass::allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString()
{
    //Properties and PrivateMembers
    QList<QString> ret;

    //TODOreq: properties. i'm not too familiar with Q_PROPERTY, even though obviously i want to support it. i'm not sure, but i think i want the "read" part of the property here...

    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType currentMember, HasA_PrivateMemberClasses)
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
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
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
QDataStream &operator<<(QDataStream &out, const HasA_PrivateMemberClasses_ListEntryType &hasA_PrivateMemberClasses_ListEntryType)
{
    HasA_PrivateMemberClasses_ListEntryType_QDS(out, <<, hasA_PrivateMemberClasses_ListEntryType)
}
QDataStream &operator>>(QDataStream &in, HasA_PrivateMemberClasses_ListEntryType &hasA_PrivateMemberClasses_ListEntryType)
{
    HasA_PrivateMemberClasses_ListEntryType_QDS(in, >>, hasA_PrivateMemberClasses_ListEntryType)
}
QDataStream &operator<<(QDataStream &out, const HasA_PrivateMemberClasses_ListEntryType *&hasA_PrivateMemberClasses_ListEntryType)
{
    return out << *hasA_PrivateMemberClasses_ListEntryType;
}
QDataStream &operator>>(QDataStream &in, HasA_PrivateMemberClasses_ListEntryType *&hasA_PrivateMemberClasses_ListEntryType)
{
    hasA_PrivateMemberClasses_ListEntryType = new HasA_PrivateMemberClasses_ListEntryType();
    return in >> *hasA_PrivateMemberClasses_ListEntryType;
}
