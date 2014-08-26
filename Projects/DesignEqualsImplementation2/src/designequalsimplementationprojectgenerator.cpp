#include "designequalsimplementationprojectgenerator.h"

#include <QFile>
#include <QTextStream>
#include <QHashIterator>

#include "designequalsimplementationclass.h"
#include "designequalsimplementationusecase.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationslotinvocationstatement.h"

//one instance = one generation
//TODOreq: sanitize that all slots in all use cases are 'named' before doing generation. hold off on this for now since "slot naming" might get refactored
//TODOoptional: any time curly braces don't have anything inside them, they should be on a single line with a space in between (like i already do for constructors). i can/should do the same for slots... it just saves vertical real estate
DesignEqualsImplementationProjectGenerator::DesignEqualsImplementationProjectGenerator(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, QObject *parent)
    : QObject(parent)
    , m_ProjectGenerationMode(projectGenerationMode)
    , m_DestinationDirectoryPath(destinationDirectoryPath)
{ }
bool DesignEqualsImplementationProjectGenerator::processClassStep0declaringClassInProject(DesignEqualsImplementationClass *designEqualsImplementationClass)
{
    if(!m_ClassesInThisProjectGenerate.contains(designEqualsImplementationClass))
    {
        m_ClassesInThisProjectGenerate.insert(designEqualsImplementationClass, QList<QString>());
    }
    return true;
}
bool DesignEqualsImplementationProjectGenerator::processUseCase(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase)
{
#if 0 //TODOreq: class instance a.0 signal -> class instance b.3 slot should use the 'put a project-wide dependency' tactic, because some use cases will "redefine"/re-use/reference-again (and it's up to us to merge at compile time) a connection. I think the same connection will only ever show up TWICE in a project if you are referencing an "already existing and defined slot" [in another use case] type thing (double clicking opens tab)... like that entire use case becomes a small part of the use case you're defining. There might be other times when a connection is defined twice [but we only want it to occur actually once]
    QListIterator<SignalSlotConnectionActivationTypeStruct> signalSlotConnectionActivationIterator(m_SignalSlotConnectionActivationsInThisUseCase);
    while(signalSlotConnectionActivationIterator.hasNext())
    {
        SignalSlotConnectionActivationTypeStruct currentSignalSlotConnectionActivation = signalSlotConnectionActivationIterator.next();
        //TODOreq: resolve where to put connect() code
        //Ex:
        //  0[x]) if both objects are private hasA members of the same parent object, put connect code in parent constructor-ish
        //  1[x]) if the signal class hasA the slot class, put in signal class constructor-ish
        //  2[x]) if the slot class hasA the signal class, put in slot constructor-ish
        //  3[]) TODOoptional: allow a class lifeline to draw a line to itself. it should be arch back in some way back to itself. it is a signal-slot or perhaps a slot invoke. this might all be solved implicitly when i implement multiple slots for a single signal emit. OLD(not wrong but thinking about it backwards): if signal-object and slot-object types are the same but instances differ, do not put in constructor but somewhere higher. the owner of said instances (common ancestor? i have a feeling some cases may require autogenerating signals in unrelated-but-in-between classes. i could be wrong)
        //

        //man i don't want to pollute "parent" classes but at the same time do want project to be a "class" for the first KISS refactor. blah i go back and forth on whether to KISS or to change up my game plan. the "common ancestor" scheme is the one that has the ability to pollute lots of "in between" classes that should not give a damn. object instances have uuids for their objectName and i use the findChild method?
    }
#endif

    DesignEqualsImplementationClassLifeLine *rootClassLifeline = designEqualsImplementationUseCase->m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsZeroIfNoneConnectedFromActorYet.first;
    if(!rootClassLifeline)
    {
        emit e(QObject::tr("You must connect an actor in use case '") + designEqualsImplementationUseCase->Name + QObject::tr("' before you can generate source code"));
        return false;
    }

    if(!recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(designEqualsImplementationUseCase, rootClassLifeline, designEqualsImplementationUseCase->m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsZeroIfNoneConnectedFromActorYet.second))
    {
        emit e("failed recursive walk kdlfjasldkf");
        return false;
    }

#if 0 //TODOinstancing
    DesignEqualsImplementationClassInstance *classInstance = classLifeline->myInstanceInClassThatHasMe();
    if(classInstance->m_InstanceType == DesignEqualsImplementationClassInstance::UseCasesRootClassLifeline)
    {
        //Use case entry point (must be top level instance (for now))
        //Foo *foo = new Foo();
        m_DesignEqualsImplementationProject->appendLineToTemporaryProjectGlueCode(classInstance->preferredTextualRepresentation() + " = new " + classInstance->m_MyClass->ClassName + "();");



    }
    else //TODOreq: non-use-case-entry-point top level objects? it kind of makes sense that all objects MUST be referenced by at least one use case in order to be generated... so maybe don't do non-use-case-entry-point top level objects...
    {
#if 0
        //Child instance -- TODOoptional: seems to make more sense that this is created more permanently when the class adds a hasA. in fact wait i think i already have this done...
        //In Foo's constructor:
        //Bar *bar = new Bar(this);
        classInstance->m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->appendLineToClassConstructorTemporarily(classInstance->preferredTextualRepresentation() + " = new " + classInstance->m_MyClass->ClassName + "(this);");
#endif
    }
#endif
#if 0
    QList<DesignEqualsImplementationClassSlot*> slotsInClassLifeLine = classLifeline->mySlotsAppearingInClassLifeLine();
    if(slotsInClassLifeLine.contains(m_UseCaseSlotEntryPoint_OrNegativeOneIfNoneConnectedFromActorYetctorYet.second))
    {
        DesignEqualsImplementationClassSlot *slotEntryPoint = m_UseCaseSlotEntryPoint_OrFirstIsNegativeOneIfNoneConnectedFromActorYetctorYet.second;
        //TODOreq: Make note of the dependency to the instance. Project keeps track of instances at this point using a temporary reference count (counted here, utilized in class's genereate source code)
    }
    else
    {
        //TODOreq: synchro error
        emit e("ERROR slfjsdlfkjdsflkjsdfl");
    }
}
#endif
#if 0
QListIterator<DesignEqualsImplementationClassLifeLine*, int> classLifelinesInUseCaseIterator(m_ClassLifeLinesAppearingInThisUseCase);
while(classLifelinesInUseCaseIterator.hasNext())
{
    DesignEqualsImplementationClassLifeLine *currentClassLifeline = classLifelinesInUseCaseIterator.next();

    //classLifeline->jitTempRegenerateInitializationCodeForClassInstanceInProject(); //Only modifies the upcoming recursive DesignEqualsImplementationClass::generateSourceCode call, it's changes not serialized into the Class

    //TODOreq: do something with m_A_SIGNAL_SLOT_ACTIVATION_IN_THIS_USE_CASE, like jitTempRegenerate the connection code as well
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlotInCurrentLifeLine, currentClassLifeline->mySlots())
    {
    }
#endif
#if 0 //OLD AS FUCK:
    bool firstUseCaseEvent = true;
    Q_FOREACH(EventAndTypeTypedef *orderedUseCaseEvent, OrderedUseCaseEvents)
    {
        switch(orderedUseCaseEvent->first)
        {
        case UseCaseSlotEventType: //changes context of next use case event
        {
            DesignEqualsImplementationClassSlot *slotUseCaseEvent = static_cast<DesignEqualsImplementationClassSlot*>(orderedUseCaseEvent->second);
            if(firstUseCaseEvent)
            {
                //we have no context, but want the first slot to be our context! TODOreq: there is still ui/cli context to consider, in which case we would have a context already at this point. if this project is generating in lib mode, we don't
                //generate slot context/empty-impl from current use case event, then set that slot/context/empty-impl as current context
                firstUseCaseEvent = false;
            }
            else
            {
                //generate slot context/empty-impl from current use case event, then in current context do nameless-signal or invokeMethod to the slot/context/empty-impl, then set that slot/context/empty-impl as current context
            }
        }
            break;
        case UseCaseSignalEventType: //does not change context of next use case event
        {
            DesignEqualsImplementationClassSignal *signalUseCaseEvent = static_cast<DesignEqualsImplementationClassSignal*>(orderedUseCaseEvent->second);
            //in current context, emit the signal
        }
            break;
        case UseCaseSignalSlotEventType: //changes context of next use case event. the only difference from UseCaseSignalSlotEventType and UseCaseSlotEventType is that the signal is a named part of the design for UseCaseSignalSlotEventType, whereas UseCaseSlotEventType either uses an autogenerated nameless signal or invokeMethod (doesn't matter which i choose)
        {
            SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent = static_cast<SignalSlotCombinedEventHolder*>(orderedUseCaseEvent->second);

        }
            break;
        case UseCaseExitSignalEventType: //handled differently (although i could...)
            break;
        }
    }
    if(ExitSignal)
    {
        //TODOreq
    }
#endif
    return true;
}
bool DesignEqualsImplementationProjectGenerator::writeClassesToDisk()
{
    QHashIterator<DesignEqualsImplementationClass*, QList<QString> > classesToGenerateIterator(m_ClassesInThisProjectGenerate);
    while(classesToGenerateIterator.hasNext())
    {
        classesToGenerateIterator.next();
        DesignEqualsImplementationClass *currentClass =  classesToGenerateIterator.key();
        if(!writeClassToDisk(currentClass))
        {
            emit e("failed to write class to disk: " + currentClass->ClassName);
            return false;
        }
    }
    return true;
}
DesignEqualsImplementationProject::ProjectGenerationMode DesignEqualsImplementationProjectGenerator::projectGenerationMode() const
{
    return m_ProjectGenerationMode;
}
void DesignEqualsImplementationProjectGenerator::setProjectGenerationMode(const DesignEqualsImplementationProject::ProjectGenerationMode &projectGenerationMode)
{
    m_ProjectGenerationMode = projectGenerationMode;
}
QString DesignEqualsImplementationProjectGenerator::destinationDirectoryPath() const
{
    return m_DestinationDirectoryPath;
}
void DesignEqualsImplementationProjectGenerator::setDestinationDirectoryPath(const QString &DestinationDirectoryPath)
{
    m_DestinationDirectoryPath = DestinationDirectoryPath;
}
//use case mode meaning we are (at the time of writing) only resolving where to put connect statements. we could use this time to resolve what classes get generated, but we instead rely on a project member (list) to keep track of which objects to generate. this allows objects of pure uml with no use cases to still be generated (perhaps they want to code all logic themselves but still want a class generator)
//^i think it is undefined right now if i enter the use case mode of slot iterating (below) without first adding all relevant classes to the project, but it would be easy to fix that anyways with proper access guards to the "declaring" of a connect statement in an [jit-declared] object (whose type was just recently made known to use through the iterating of some slot-invocation heirarchy)
bool DesignEqualsImplementationProjectGenerator::recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, DesignEqualsImplementationClassLifeLine *classLifeline, DesignEqualsImplementationClassSlot *slotToWalk)
{
    //NOTE: all class lifelines in the use case (for every use case in the project) have been assigned instances if/when we get here

    //Now iterate that use case entry point's statements and follow any slot invokes (and signal emits ar special too :-P)
    int currentStatementIndex = -1;
    Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, slotToWalk->orderedListOfStatements())
    {
        ++currentStatementIndex;

        if(currentStatement->isSignalEmit())
        {
            //TODOreq: signal -> slot, just "continue;" if no slot attached
            DesignEqualsImplementationSignalEmissionStatement *signalEmitStatement = static_cast<DesignEqualsImplementationSignalEmissionStatement*>(currentStatement);

            //Signal/slot connection activation
            Q_FOREACH(const DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct &currentSignalSlotConnectionActivation, designEqualsImplementationUseCase->m_SignalSlotConnectionActivationsInThisUseCase)//TODOreq: ensure all these "key checks" are properly synchronized throughout app lifetime. getters and setters fuck yea~
            {
                if(currentSignalSlotConnectionActivation.SignalStatement_Key0_IndexInto_m_ClassLifeLines == classLifeline) //signal key 0 check
                {
                    if(currentSignalSlotConnectionActivation.SignalStatement_Key1_SourceSlotItself == slotToWalk) //signal key 1 check
                    {
                        if(currentSignalSlotConnectionActivation.SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements == currentStatementIndex) //signal key 2 check
                        {
                            //Getting here means the signal is connected to a slot

                            DesignEqualsImplementationClassLifeLine *destinationSlotClassLifeline = designEqualsImplementationUseCase->m_ClassLifeLines.at(currentSignalSlotConnectionActivation.SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines); //slot key 0
                            DesignEqualsImplementationClassSlot *destinationSlot = currentSignalSlotConnectionActivation.SlotInvokedThroughConnection_Key1_DestinationSlotItself; //slot key 1

                            //0a
                            //Check if they are both members of the same parent, the simplest connection resolving case and number 0 on my list
                            if(destinationSlotClassLifeline->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline && classLifeline->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline) //precondition that they both have a parent
                            {
                                if(destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->parentClass() == classLifeline->instanceInOtherClassIfApplicable()->parentClass())
                                {
                                    DesignEqualsImplementationClass *sharedParentOfSignalAndSlotForGettingConnectStatementInConstructorish = destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->parentClass();

#if 0
                                    //3
                                    if(destinationSlotClassLifeline->instanceInOtherClassIfApplicable() != classLifeline->instanceInOtherClassIfApplicable()-> && sharedParentOfSignalAndSlotForGettingConnectStatementInConstructorish)
                                    {

                                    }
                                    else //0b
                                    {
#endif
                                        appendConnectStatementToClassInitializationSequence(sharedParentOfSignalAndSlotForGettingConnectStatementInConstructorish, DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(classLifeline->instanceInOtherClassIfApplicable()->VariableName, signalEmitStatement->signalToEmit()->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames), destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->VariableName, destinationSlot->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames)));
//                                    }
                                }
                            }

                            //TODOreq: 1, 2, 3 etc on taht list

                            //1
                            //if the signal class hasA the slot class, put in signal class constructor-ish
                            if(destinationSlotClassLifeline->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline) //precondition that slot has a parent
                            {
                                //if(destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->parentClass()->hasA_Private_Classes_Members().contains())
                                if(classLifeline->designEqualsImplementationClass()->hasA_Private_Classes_Members().contains(destinationSlotClassLifeline->instanceInOtherClassIfApplicable()))
                                {
                                    //the class with the signal hasA the class with the slot
                                    //so the signal's constructor gets the connect statement
                                    //classLifeline->designEqualsImplementationClass()->appendLineToClassConstructorTemporarily(DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(classLifeline->instanceInOtherClassIfApplicable()->VariableName, currentStatement->toRawCppWithoutEndingSemicolon(), destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->VariableName, destinationSlot->methodSignatureWithoutReturnType()));
                                    appendConnectStatementToClassInitializationSequence(classLifeline->designEqualsImplementationClass(), DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(QString("this"), signalEmitStatement->signalToEmit()->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames), destinationSlotClassLifeline->instanceInOtherClassIfApplicable()->VariableName, destinationSlot->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames)));
                                }

                            }

                            //2
                            //if the slot class hasA the signal class, put in slot constructor-ish
                            if(classLifeline->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline) //precondition that signal has a parent
                            {
                                if(destinationSlotClassLifeline->designEqualsImplementationClass()->hasA_Private_Classes_Members().contains(classLifeline->instanceInOtherClassIfApplicable()))
                                {
                                    //the class with the slot hasA the class with the signal
                                    //so the slot's constructor gets the connect statement
                                    appendConnectStatementToClassInitializationSequence(destinationSlotClassLifeline->designEqualsImplementationClass(), DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(classLifeline->instanceInOtherClassIfApplicable()->VariableName, signalEmitStatement->signalToEmit()->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames), QString("this"), destinationSlot->methodSignatureWithoutReturnType(IDesignEqualsImplementationMethod::MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames)));
                                }
                            }

#if 0 //needs refactoring

                            //TODOoptional: signal daisy chaining. i dunno how it would work in the GUI though

                            QString signalVariableNameInConnectStatement;

                            //TODOoptional: clean up so that whether or not it's top level object isn't determined TWICE
                            if(slotParentClassInstance->m_InstanceType == DesignEqualsImplementationClassInstance::ChildMemberOfOtherClassLifeline) //HACK maybe, idk i just think this might need to go somewhere else (earlier, so that multi project mode still generates it)
                            {
                                //slot is NOT top level object, so signal variable name is 'this'. TODOreq: oops, the 'this' might be the slot parent instead of signal parent (as in the example/comment below 'build connect statement'). needz moar dynamism
                                signalVariableNameInConnectStatement = "this";
                            }
                            else
                            {
                                //slot is top level object, so use actual variable name
                                signalVariableNameInConnectStatement = signalParentClassInstance->VariableName;
                            }

                            //build connect statement
                            //connect(m_Bar, SIGNAL(barSignal(bool)), this, SLOT(handleBarSignal(bool)));
                            QString connectStatement = "connect(" + signalVariableNameInConnectStatement + ", SIGNAL(" + signalEmitStatement->signalToEmit()->methodSignatureWithoutReturnType() + "), " + slotParentClassInstance->VariableName + ", SLOT(" + destinationSlot->methodSignatureWithoutReturnType() + "));";


                            if(slotParentClassInstance->m_InstanceType == DesignEqualsImplementationClassInstance::ChildMemberOfOtherClassLifeline)
                            {
                                //slot not top level object, put connect statement in signal parent constructor
                                signalParentClassInstance->m_MyClass->appendLineToClassConstructorTemporarily(connectStatement);
                            }
                            else
                            {
                                //slot is top level object, put connect statement in glue code
                                m_DesignEqualsImplementationProject->appendLineToTemporaryProjectGlueCode(connectStatement);
                            }
#endif
                            if(!recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(designEqualsImplementationUseCase, destinationSlotClassLifeline, destinationSlot))
                            {
                                emit e("failed recursive walk sdlfjsdlkfjdslfj");
                                return false;
                            }
                        }
                    }
                }
            }
        }
        else if(currentStatement->isSlotInvoke()) //slot invoke = slot parent MUST be child/member (or at least have pointer to invoked instance) of invoker
        {
            //TODOreq: record dependency on the slot's class INSTANCE (however, idk wtf to do since Bar is a child of Foo). perhaps this entire dependency recording thing is not needed [for instantiation] (might still be needed for connections)
            //TODOreq: queue destination slot for iterating/recursing just like we're already doing
            DesignEqualsImplementationSlotInvocationStatement *nextSlotInvocationStatement = static_cast<DesignEqualsImplementationSlotInvocationStatement*>(currentStatement);
            if(!recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(designEqualsImplementationUseCase, nextSlotInvocationStatement->slotInvocationContextVariables().ClassLifelineWhoseSlotIsAboutToBeInvoked, nextSlotInvocationStatement->slotToInvoke()))
            {
                emit e("failed recursive walk werweurjdfkljsldkfj");
                return false;
            }
        }
    }
    return true;
}
bool DesignEqualsImplementationProjectGenerator::writeClassToDisk(DesignEqualsImplementationClass *currentClass)
{
    //Header
    QString headerFilePath = destinationDirectoryPath() + currentClass->headerFilenameOnly();
    QFile headerFile(headerFilePath);
    if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open header for writing: " + headerFilePath);
        return false;
    }
    QTextStream headerFileTextStream(&headerFile);

    //Source
    QString sourceFilePath = destinationDirectoryPath() + currentClass->ClassName.toLower() + ".cpp";
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
    QString myNameHeaderGuard = currentClass->ClassName.toUpper() + "_H";
    headerFileTextStream    << "#ifndef " << myNameHeaderGuard << endl
                            << "#define " << myNameHeaderGuard << endl
                            << endl
                            << "#include <QObject>" << endl //TODOoptional: non-QObject classes? hmm nah because signals/slots based
                            << endl;
    //Header's header's forward declares
    bool atLeastOneHasAPrivateMemberClass = !currentClass->hasA_Private_Classes_Members().isEmpty(); //spacing
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateMember, currentClass->hasA_Private_Classes_Members())
    {
        //class Bar;
        headerFileTextStream << "class " << currentPrivateMember->m_MyClass->ClassName << ";" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        headerFileTextStream << endl; //OCD <3
    headerFileTextStream    << "class " << currentClass->ClassName << " : public QObject" << endl
                            << "{" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl;

    //Header's Q_PROPERTY declarations
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        //Q_PROPERTY..
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_PROPERTY(" + currentProperty->Type << " " << firstCharacterToLower(currentProperty->Name) << " READ " << getterNameForProperty(currentProperty->Name); //here marks the first time i've ever actually used the Q_PROPERTY macro <3. i'm always just too lazy and don't see the benefit... but since code generator, the cost becomes/became zero xD
        if(!currentProperty->ReadOnly)
            headerFileTextStream << " WRITE " << setterNameForProperty(currentProperty->Name);
        if(currentProperty->NotifiesOnChange)
            headerFileTextStream << " NOTIFY " << changedSignalForProperty(currentProperty->Name);
        headerFileTextStream << ")" << endl;
    }

    headerFileTextStream << "public:" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << currentClass->ClassName << "(QObject *parent = 0);" << endl;
    headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "virtual ~" << currentClass->ClassName << "();" << endl;

    //Header's property getters and setters declarations
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        //Property getter
        //int x() const;
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentProperty->Type << " " << getterNameForProperty(currentProperty->Name) << "() const;" << endl;

        //Property setter
        if(!currentProperty->ReadOnly)
        {
            //void setX(int newX);
            headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << setterNameForProperty(currentProperty->Name) << "(" << currentProperty->Type << " " << "new" << firstCharacterToUpper(currentProperty->Name) << ");" << endl;
        }
    }

    bool privateAccessSpecifierWritten = false;
    if(!currentClass->Properties.isEmpty())
    {
        headerFileTextStream << "private:" << endl;
        privateAccessSpecifierWritten = true;
    }

    //Header's property member declarations
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentProperty->Type << " " << memberNameForProperty(currentProperty->Name) << ";" << endl;
    }
    if(!currentClass->Properties.isEmpty() && atLeastOneHasAPrivateMemberClass) //TODOsanity: make a list of each of the visibility specifier entries, then process AT THE END whether or not to write a visibility specifier. there should be an "empty line" entry (like the one just below) that should be able to be "trimmed" if no statements are following (like the second half of this if statement does). the same kind of thing can/should be used to determine whether or not to do "{ }" or "{\n" (if any statements in block)
    {
        headerFileTextStream << endl;
    }

    //Header's hasAPrivateMemberClass declarations
    if(atLeastOneHasAPrivateMemberClass && !privateAccessSpecifierWritten)
    {
        headerFileTextStream << "private:" << endl;
        privateAccessSpecifierWritten = true;
    }
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateMember, currentClass->hasA_Private_Classes_Members())
    {
        //Bar *m_Bar;
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentPrivateMember->preferredTextualRepresentationOfTypeAndVariableTogether() << ";" << endl;
    }

    //Source's header+constructor (the top bits, not the ".h" counter-part)
    sourceFileTextStream    << "#include \"" << currentClass->headerFilenameOnly() << "\"" << endl
                            << endl;
    //Source's header PrivateMemberClasses includes
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateMember, currentClass->hasA_Private_Classes_Members())
    {
        //#include "bar.h"
        sourceFileTextStream << "#include \"" << currentPrivateMember->m_MyClass->headerFilenameOnly() << "\"" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        sourceFileTextStream << endl;
    sourceFileTextStream    << currentClass->ClassName << "::" << currentClass->ClassName << "(QObject *parent)" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl;
    //Source's header Properties constructor initializers
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        if(currentProperty->HasInit)
        {
            //, m_SomeBoolProperty(true)
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ", " << memberNameForProperty(currentProperty->Name) << "(" << currentProperty->OptionalInit << ")" << endl; //TODOoptional: a way to use one of currentClass's constructor args (or even multiple but omg my brain) as the init for a given property. should be 'smart'
        }
    }
    //Source's header PrivateMemberClasses constructor initializers
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateMember, currentClass->hasA_Private_Classes_Members())
    {
        //, m_Bar(new Bar(this))
        sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ", " << currentPrivateMember->VariableName << "(new " << currentPrivateMember->m_MyClass->ClassName << "(this))" << endl; //TODOreq: for now all my objects need a QObject *parent=0 constructor, but since that's also a [fixable] requirement for my ObjectOnThreadGroup, no biggy. Still, would be nice to solve the threading issue and to allow constructor args here (RAII = pro)
    }

    //Source constructor -- children connection statements (or just constructor statements, but as of writing they are only connect statements)
    QList<QString> classConstructorLines = m_ClassesInThisProjectGenerate.value(currentClass);
    if(classConstructorLines.isEmpty())
    {
        sourceFileTextStream    << "{ }" << endl;
    }
    else
    {
        sourceFileTextStream << "{" << endl;
        Q_FOREACH(const QString &currentLine, classConstructorLines)
        {
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentLine << endl;
        }
        sourceFileTextStream << "}" << endl;
    }

    //Source properties getters and setters definitions
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        //int Class::x() const
        //{
        //  return m_X;
        //}
        sourceFileTextStream << currentProperty->Type << " " << currentClass->ClassName << "::" << getterNameForProperty(currentProperty->Name) << "() const" << endl << "{" << endl << DESIGNEQUALSIMPLEMENTATION_TAB << "return " << memberNameForProperty(currentProperty->Name) << ";" << endl << "}" << endl;

        if(!currentProperty->ReadOnly)
        {
            //void Class::setX(int newX)
            //{
            //  if(newX != m_X)
            //  {
            //    m_X = newX;
            //    emit xChanged(m_X);
            //  }
            //}
            const QString &newSetPropertyVariableName = "new" + firstCharacterToUpper(currentProperty->Name);
            sourceFileTextStream << "void " << currentClass->ClassName << "::" << setterNameForProperty(currentProperty->Name) << "(" << currentProperty->Type << " " << "new" << firstCharacterToUpper(currentProperty->Name) << ")" << endl << "{" << endl << DESIGNEQUALSIMPLEMENTATION_TAB << "if(" << newSetPropertyVariableName << " != " << memberNameForProperty(currentProperty->Name) << ")" << endl << DESIGNEQUALSIMPLEMENTATION_TAB << "{" << endl << DESIGNEQUALSIMPLEMENTATION_TAB << DESIGNEQUALSIMPLEMENTATION_TAB << memberNameForProperty(currentProperty->Name) << " = " << newSetPropertyVariableName << ";" << endl;
            if(currentProperty->NotifiesOnChange)
            {
                sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << DESIGNEQUALSIMPLEMENTATION_TAB << "emit " << changedSignalForProperty(currentProperty->Name) << "(" << memberNameForProperty(currentProperty->Name) << ");" << endl;
            }
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "}" << endl << "}" << endl;
        }
    }

    bool signalsAccessSpecifierWritten = false;
    //Signals
    if(!currentClass->mySignals().isEmpty())
    {
        headerFileTextStream << "signals:" << endl;
        signalsAccessSpecifierWritten = true;
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, currentClass->mySignals())
    {
        //void fooSignal();
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSignal->methodSignatureWithoutReturnType() << ";" << endl;
    }

    //Property change notification signals
    bool gapBetweenRegularSignalsAndPropertyChangeSignalsWritten = false;
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
    {
        if(currentProperty->NotifiesOnChange)
        {
            if(!signalsAccessSpecifierWritten)
            {
                headerFileTextStream << "signals:" << endl;
                signalsAccessSpecifierWritten = true;
                gapBetweenRegularSignalsAndPropertyChangeSignalsWritten = true; //we don't want the gap because there are no regular signals!
            }
            else if(!gapBetweenRegularSignalsAndPropertyChangeSignalsWritten)
            {
                headerFileTextStream << endl;
                gapBetweenRegularSignalsAndPropertyChangeSignalsWritten = true;
            }

            headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << changedSignalForProperty(currentProperty->Name) << "(" << currentProperty->Type << " " << "new" + firstCharacterToUpper(currentProperty->Name) << ");" << endl;
        }
    }

    //Slots
    if(!currentClass->mySlots().isEmpty())
        headerFileTextStream << "public slots:" << endl;
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, currentClass->mySlots())
    {
        //Declare slot
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSlot->methodSignatureWithoutReturnType() << ";" << endl;
        //Define slot
        sourceFileTextStream    << "void " << currentClass->ClassName << "::" << currentSlot->methodSignatureWithoutReturnType() << endl
                                << "{" << endl;
        Q_FOREACH(IDesignEqualsImplementationStatement *currentSlotCurrentStatement, currentSlot->orderedListOfStatements())
        {
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentSlotCurrentStatement->toRawCppWithEndingSemicolon() << endl;
        }
        if(currentSlot->finishedOrExitSignal_OrZeroIfNone())
        {
            DesignEqualsImplementationSignalEmissionStatement finishedOrExitSignalEmitStatement(currentSlot->finishedOrExitSignal_OrZeroIfNone(), currentSlot->finishedOrExitSignalEmissionContextVariables());
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << finishedOrExitSignalEmitStatement.toRawCppWithEndingSemicolon() << endl;
        }
        sourceFileTextStream    << "}" << endl;
    }

    //Header's footer
    headerFileTextStream    << "};" << endl
                            << endl
                            << "#endif // " << myNameHeaderGuard << endl;

#if 0
    //Recursively generate source for all children HasA_PrivateMemberClasses
    Q_FOREACH(HasA_Private_Classes_Member *currentPrivateMember, currentClass->hasA_Private_Classes_Members())
    {
        if(!currentPrivateMember->m_MyClass->generateSourceCode(destinationDirectoryPath)) //TODOreq: a single press of generate source code should never write the same class file more than once (since i am probably calling generate source code more than once for some classes)
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX + currentPrivateMember->m_MyClass->ClassName);
            return false;
        }
    }
#endif
    return true;
}
void DesignEqualsImplementationProjectGenerator::appendConnectStatementToClassInitializationSequence(DesignEqualsImplementationClass *classToGetConnectStatementInInitializationSequence, const QString &connectStatement)
{
    QList<QString> currentListOfConnectStatements = m_ClassesInThisProjectGenerate.value(classToGetConnectStatementInInitializationSequence);
    currentListOfConnectStatements.append(connectStatement);
    m_ClassesInThisProjectGenerate.insert(classToGetConnectStatementInInitializationSequence, currentListOfConnectStatements);
}
