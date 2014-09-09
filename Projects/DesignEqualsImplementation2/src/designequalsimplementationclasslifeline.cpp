#include "designequalsimplementationclasslifeline.h"

#include <QDataStream>

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationproject.h"
#include "designequalsimplementationclass.h"
#include "designequalsimplementationusecase.h"
#include "designequalsimplementationclasslifelineunitofexecution.h"

//Tempted to say this should inherit from 'class', but idk fo sho (plus that would require refactor (frontend specifies TYPE, backend factories it (whereas now i'm just passing one pointer around heh)) so fuck it for now)
//TODOreq: need to define the concept of a "Unit of Execution". Multiple class lifelines can share a unit of execution (they do, if they are on the same thread and use AutoConnection). A unit of execution ends when an event on a different thread is added and no more events on that OUR thread are added after that (except as a result of the OTHER thread jumping back to us (same life line, now different unit of execution)). An object(or group of objects sharing a unit of execution) should never be in an invalid state when finishing a unit of execution, following this rule is the easiest/sanest way to achieve thread safety. One one hand, my initial Foo/Bar example does not require threading (and therefore the concept of unit of execution), but on the other I may find myself refactoring for it later since I definitely do want it. And of course: KISS xD
//A class lifeline represents an object instantiation. When a life line is added to a use case, the use case only puts  +1 on the lifeLineReferenceCount, which ultimately is (or should be) what decides whether or not the object is instantiated during the initialization period. The object's position in the class diagram heirarchy is what determines if/when it will be instantiated (and by who). Right now I only support one top level object (it is generally the first object whose slot the actor calls invokeMethod on). For now, all the other objects must be children (or sub-children) of that one top level object (this should change eventually, but low priority + KISS)
//A class lifeline does keep track of it's units of executions, but ordering does not and should not matter (Note/semi-OT: a project might have a primary use case, in which case it IMPLICITLY has a "first" unit of execution", but that is mere side effect). Since there are many use cases per project that can be run at any time, lifelines are shared among the use cases transparently and each use case only specifies units of execution FOR that class diagram FOR that use case. The ordering of every unit of execution in every use case must be independent. That's not to say you can call VideoLibrary::decodeFrame before VideoLibrary::parseFrame, but that the result of parseFrame, when asynchronously responded to you via a signal, is what is also passed to VideoLibrary::decodeFrame.... AND that N parseFrames (and/or decodeFrame) can be called at any point (well i mean, not right smack dab in the middle of another unit of execution, but any time BETWEEN (Qt's event system takes care of this for me)). In this case the VideoLibrary instance is not tied to a single file, but whether or not that is the case is design specific (OO vs. C-style context pointer passing -- use whatever works imo (I tend to lean toward OO myself))
//NVM: A use case has a class lifeline INSTANCE, per se, not just a reference to the class lifeline (or actually, one instance using reference counting would work)
//^In any case, a use case must reference class lifelines AND units of execution (the ones involved in the use case) therein
//Since ordering does not matter, units of execution are serialized/held-internally/etc/source-generated in simple top->bottom (as placed by user) manner, when iterating the project's list of use cases (comments could separate the blocks of methods saying they pertain to or were generated by use case X)
//Units of execution only do not have ordering with relation to each other, but that doesn't mean that a series of "unit of execution context switches" (slotInvoke or signal/slot activation) doesn't have ordering for a specific use case (they do!)
//In this Qt/thread-safe UML mode, any time a slotInvoke/signal-slot-activation is made, a new unit of execution begins on the target. The unit of execution for the source may optionally continue with other "statements" (do I need to merge "statements" with "use case events"? wtf is the difference anywyas man i'm dumb)
//OT: I keep flip flopping on whether or not the "Qt thread safe" UML mode is what I want. Does invokeMethod really slow things down that much? I honestly don't have any relative sense... err... can't put into relative terms... how much "10x slower than a virtual function call" is, BECAUSE I have no clue how much slower a virtual call is than a non-virtual call lolol. Still, considering I use virtual function calls in a heartbeat and never really thought of them as being slow, AND considering the thread safety and future-proof-in-breaking-things-apart-because-of-that-thread-safety, I wrap back around and am confident again that slotInvoke/signal-slot-activation is the way to go for practically everything! Qt::AutoConnection is pro, the end. *searches on internet "how much slower is virtual function call" ;-P*. Holy shit some guy's random test results: inline 8ms, direct 68ms, virtual 160ms... which puts invokeMethod on same thread at 1600ms, fffff. Still, there's something to be said about one-object (no threading primitives used in object) + thread safety ;-).... and of course the bottleneck is almost always elsewhere so fuck it (ex: decodeFrame can justify being a slot because decodeFrame is a heavy process). I never knew inlines were so much faster, wtf. C89 + Inline everywhere (jk no inline in C89 -_-) xD.
//TODOoptional (rainy day) (nvm): compile time (source code generation time actually) deciding of invokeMethod vs. direct method call. The generated code would be less portable (except the serialized design (generated from) would always remain portable), but the generator could rule out lots of places where invokeMethod isn't needed. Actually this sounds like a headache and counter-intuitive to all the thread safety I keep ranting about, fuck this idea
//I think where I get the most confused is when a slot invocation use case event is added to the ordered list of use case events. RIGHT THEN AND THERE ordering does [optionally] become irrelevant. Kinda. If I were to add statements beneath the slot invocation (on the invoker), do those now come before or after the invokeMethod call? The answer is both || undefined (the answer really is 'after' if same thread 'almost-always-before' if different thread). Still, when asking myself how to store it in the "ordered list of use case events", my brain keeps segfaulting because i want to do: olouce (ordered list of use case events): olouce.append(slotInvoke), olouce.append(otherStuffWITHINthatslot), olouce.append(stuffBelowFirstSlotInvoke). Whereas I'm just now beginning to realize that the first slot invoke STARTS another unit of execution, so we shouldn't cram it's contents in between two "statements" or "items" in the ordered list of use case statements. I suppose I need a tree more than a flat list, but I still can't perfectly decide how to make it fit
//A use case always starts with a slot, and therefore a unit of execution (a unit of execution reference). Entries in THAT unit of execution may or may not start other units of execution (they have the same unit of execution reference, but their "sender" is noted (whereas the first one is special and comes from "actor"). So really, the internal data structure (and serialized) are ultimately UNORDERED. Program order can be seen by simply iterating the first unit of execution, but because of multi-threading (slotInvoke + signal/slots harnesses), two units of execution may be active simultaneously! Ordering? bah.
//Internal memory should look like this: UseCase.SlotEntryPoint = qMakePair<ClassLifeline, SlotOnClass>; or something perhaps using the word "unit of execution", but the general idea is the same. Maybe: new UseCase.EntrySlotUnitOfExecution = new UnitOfExecution(classLifeline, slotOnClass);
//The only hard part is really making units of execution referencable by other units of execution, but that's not so bad just a matter of design
//So the internal makeup of a unit of execution:
/*
pseudo

ATTEMPT 0
class UnitOfExecution
{
    ClassLifeLine *m_ClassInstance;
    QList<Slot*> m_OrderedListOfUnitOfExecutionStatements; //constructor appends item 0, the slot entry point
};

But you see, that QList needs to be able to point to other units of execution, so storing the "slot" as type won't do. Needs moar multiple inheritence.

Actually shit I think my IStatement thing would work perfectly, except that for some reason I have IStatement as a child hasA of Slot right now, which is confusing (refactor methinks).

ATM:
slotInvoke/signal-slot-activation = "ordered use case events"
...wait wtf they are also used for IStatements... now I'm confused

AHAH, so after looking at my own code a bit it does appear that Ordered List Of Use Case Events is MOSTLY worthless. It appears to only be used in detecting whether or not the use case event being appended into it is the first (aka entry point)... and nothing else! Slot's ordered list of statements is where the real magic is at. I'm glad that the ordered list of use case events is largely unused, that should make the refactor easier

So blah

ATTEMPT 1
class UseCase
{
    QList<UnitOfExecution*> SemiOrderedListOfUnitsOfExecution;
}
class UnitOfExecution
{
    QList<IStatement*> OrderedListOfStatements;
}

slotInvoke/signal-slot-activation statements are added to OrderedListOfStatements, but they point to entries in SemiOrderedListOfUnitsOfExecution (by pointer or by index? pointer sounds more flexible)

And what about slot's ordered list of statements? Do they now not exist? A SLOT IS A UNIT OF EXECUTION (always needs to be kept in mind while designing), so maybe I just merge the two? STILL, unit of execution is to slot.... what class lifeline is to class. Context, (caller?), etc.
STILL AGAIN, the body of a slot (ordered list of statements) is still err.. usable on other instances, so the body isn't directly linked with the instance (nor are any of the slots invoked on a class lifeline specific to that lifeline). Man I'm just fucking confused as shit now

There shouldn't be UseCase::addEvent(event) anymore
Instead, UseCase::newUnitOfExecution() (or addUnitOfExecution(unit), whatever semantecs)
 and: unitOfExecution.addStatement(event) (insert later ;-P)

The source of the arrow drawn is the unit of execution that adds the statement

Should the unit of execution refer to the statement that starts it (which can be an actor too)? Or should it just be a 1-way association? I guess I should just do 1-way association until I see the need for 2-way, at which time I'll hack it in :-P (ez)

A unit of execution already refers to a class lifeline, so context (which instance am I?) can be achieved when needed (can't remember where, only that I want it!)

Ok so I think I've finally figured it out: Units of Execution contain statements (statements CAN refer to other units of executions). Use cases contain list of units of executions involved, but not necessarily ordered (order them how the user designs them, but design knowing that two units of execution may be running concurrently). Simple.

TODOoptional: I'm really heavily leaning towards QtConcurrent usage (so long as my generated classes don't depend on it :-P), but meh maybe ultimately it'd make the app slow as fuck because dispatching threads to process a small chunk, then killing thread right after that, rinse repeat. Right now I'm designing for "long standing objects" (application lifetime).. but perhaps some QtConcurrent integration would be warranted later on. My train of thought for that is simple and perhaps not thought out far enough: if the threads are there, USE THEM! Calculating which lifelines to put on which threads would be challenging though.. but doing run time benchmarking on first run (just like that one thing ;-P) is one way to figure out "which slots/objects take a while and so deserve their own thread", and small/short slots/objects can all share one thread. Even if I did implement this, QtConcurrent might not be the way to go, but instead just relying on preferred thread count and instantiating them "manually" (generated code does it) (and of course, developer box does the pre-benchmark (or hell, maybe only benchmark) stuffs). This kinda sounds like that shit Mozilla uses with Firefox, something along the lines of "usage based automated optimization". I think there's was more function based, whereas what I'm describing is targetting taking advantage of a multi-threaded environment. The reason we'd rely on benchmarks is simple: you never know how you're program is going to run (bottlenecks) until you run it. If the program CAN run all on the same thread, AND it CAN run with every object on it's own thread, this allows a benchmarking program to organize objects onto threads arbitrarily (NOTE: in some cases, not using QueuedConnection might give a pseudo-deadlock (an indefinite stream of DirectConnection calls keep a few Queued calls from ever being processed (in that case, the hacky solution is to make the DirectConnection calls use QueuedConnection xD)), and this very note might make the whole idea shit). OT'ish: does a same-thread-Queued-invoke cost as much as a different-thread-Queued-invoke? What's the difference in cost between a same-thread-Queued-invoke and a same-thread-Auto/Direct-invoke? TODOoptional: benchmark those two.

I'm hesitant to try to implement this refactor because it's on such a core/crucial part of the code, and I worry that I'll kill it and it will never see the light of day :(. But eh without it, this app is pretty damn worthless (not worthless if your design is finished and you're just mimicking a Dia/Umbrello design IN-ORDER ;-P (even then, can't yet add a statement after a unit of execution switching statement lolol)... but the point of this app is to make it so you don't have to use Dia/Umbrello to begin with

re: Simple. What happens to slot and his list of statements? Do unit of execution and slots merge? Oh right I remember now, unit of execution is just the same thing but with context. Still confused on how to design it completely/perfectly. WHO OWNS THE LIST OF STATEMENTS?

Is it:

ATTEMPT 2:
class UnitOfExecution
{
    Slot *slot;
    ClassLifeline *classLifeline;
}
... and slot keeps the list of statements?
I want to say yes but I worry that from within slot/statements, we may want context and lack it (unit of execution could pass himself in at time of generate/or-something).

Any arrow pointing to another class lifeline starts a new unit of execution on the class lifeline it points to (one exception: actor). No destination under arrow = signal-with-no-listener so don't worry about unit of execution
Any arrow's source (mandatory atm) that isn't an actor simply appends(inserts) the slotInvoke/signal-slot-activation to the source's already-existing unit of execution "list of statements"

What's so hard about that? O, rite, the refactor involved :(


The same unit of execution can make multiple appearances in the same project (in different use cases). In most of it's appearances, it is a slot invoked and signal responded and nothing more. In just one appearance, it is the use case entry point. It is NOT (and can not be!) the first unit of execution, except when it is the entry point to that use case. Every unit of execution CAN (but not should) represent a use case. TODOoptional: double clicking a "unit of execution appearance" opens the use case tab for that unit of execution (slot). A slot is the same thing as a unit of execution when you make the "object state guarantee" that slot will have the object ready for any event next in it's queue. They don't HAVE to be though (a slot might break that promise), but should for thread safety.

Use cases should have a pluggable "use case entry point slot". The use case might stay, but the first slot change. So to say a unit of execution is the same as a slot depends entirely on what happens when I do that unplug-plugin-new-impl-as-use-case-entry-point. Does the use case vanish since it's top level object was deleted, or is the top level object merely swapped out? SEE RIGHT THERE I'VE EXPLAINED IT PERFECTLY: are the statements part of the use case (which has units of execution) or slot (which is LIKE a unit of execution)? I actually flip flop both ways because yea you are designing Foo when you draw lines to his children, but then actually no you are designing a use case that just so happens to use Foo right now. Perhaps Foo implements a specific interface and you are really working on that interface (but at the time, Foo was what you chose to implement it). Both cases make perfect sense.

The concept of children should not exist (except for memory management), everything should be a top level object.
Objects should never know of the insides of one another (no hasA), and should communicate exlusively through application code (connections like a rube goldberg machine, with one or some invokeMethods to get the shebang started). The use cases do not define the slots themselves, only the signal/slot connections. invokeMethod should not be used because it makes one object know of the internals of another.
It simplifies connections, initialization, and differentiating between "use case code" and "slot body code". Slot body code (which can contain N signals ofc) can also contain private methods, and that is where the real C++ guts of the slot body actually lives. Perhaps I can manage a "drop down RIGHT into the C++ body of the slot" by using a before/after magic string to ease parsing, in which case the guts can go right in the guts.
The downside of not using invokeMethod is the bloat of auto-generated "requested" signals, but class modularity might make that worth it

^^When seen like that, a unit of execution points to a slot as it's entry point, yes, but it's a top level connection between Foo and Bar that makes Foo invoke barSlot.

TODOoptional: top level objects doesn't mean i can't let the user specify which ones should be created-first/destroyed-last. i only mean top level objects mainly to ease connections between objects

It's impossible to separate a slot body from a unit of execution when you're using Foo hasA Bar type stuff in the UML design. Once Foo and Bar become top level objects, Foo can be swapped out of that unit-of-execution/use-case and Bar can stay in place

So basically the use cases do nothing more than specify the "MODULAR units of execution" as they interact with each other in semi-ordered-with-program-flow "left to right and down" UML. The connection code is easy to generate when everything's top-level, and the rest is just simple as fuck UML editor stuff (class editor)


BUT WHAT OF THE SIGNAL-WITH-NO-LISTENERS-AT-TIME-OF-DESIGN? Is that part of the class or the use case? Arguably use case, arguably both. Does it remain if the signal owner is removed from the use case (I mean that both ways: remain in the use case and remain in the class now no longer in the use case)? Man that's a thinker

*/

//A class lifeline is a per-use-case thing that merely stores which "slots on the object it represents" the use case uses. It is of little design use EXCEPT WHEN determining if we have the target as a child as a hasA attiribute, in which case we rely on it explicitly (and it shines). A class lifeline represents a class instantiation. When you click and drag a class onto the use case scene, a modal dialog asks you which of the available instances (class lifelines) you want to use. Your options are existing top level class lifelines from other use cases, new top level class lifeline, existing children of other classes, or we can even add ourself as a child (hasA) to another class on the fly
//^If we don't have any variable name for the target, we are restricted to signal-slot activation (but i could see hacks allowing variable name pass in on the fly xD (there are too many customzations/hacks in this app to keep track of))
DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationProject *parentProject, DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationUseCase *parentUseCase, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, */QPointF position, QObject *parent)
    : QObject(parent)
    , m_ParentProject(parentProject)
    , m_InstanceType(NoInstanceChosen)
{
    privateConstructor(designEqualsImplementationClass, parentUseCase, position);
}
#if 0
DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassLifeLine(const QString &instanceVariableName, DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationUseCase *parentUseCase, QPointF position, QObject *parent)
    : QObject(parent)
    , m_InstanceType(ChildMemberOfOtherClassLifeline)
    , m_InstanceInOtherClassIfApplicable(instanceVariableName)
{
    privateConstructor(designEqualsImplementationClass, parentUseCase, position);
}
#endif
/*DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, QPointF position, QObject *parent)
    : QObject(parent)
    , m_DesignEqualsImplementationClass(designEqualsImplementationClass)
    , m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline)
    , m_Position(position)
{
    insertSlotReference(firstSlot);
}*/
QPointF DesignEqualsImplementationClassLifeLine::position() const
{
    return m_Position;
}
DesignEqualsImplementationClass *DesignEqualsImplementationClassLifeLine::designEqualsImplementationClass() const
{
    return m_DesignEqualsImplementationClass;
}
DesignEqualsImplementationUseCase *DesignEqualsImplementationClassLifeLine::parentUseCase() const
{
    return m_ParentUseCase;
}
#if 0 //TODOinstancing
void DesignEqualsImplementationClassLifeLine::setmyInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(DesignEqualsImplementationClassInstance *hasA_Private_Classes_Members_ListEntryType)
{
    m_MyInstanceInClassThatHasMe = hasA_Private_Classes_Members_ListEntryType;
}
DesignEqualsImplementationClassInstance *DesignEqualsImplementationClassLifeLine::myInstanceInClassThatHasMe() const
{
    return m_MyInstanceInClassThatHasMe;
}
#endif
void DesignEqualsImplementationClassLifeLine::insertSlotToClassLifeLine(int indexToInsertInto, DesignEqualsImplementationClassSlot *newSlot)
{
    //int indexActuallyInsertedTo = qMax(0, qMin(indexToInsertInto, m_MySlotsAppearingInClassLifeLine.size())); //bounds checking. even though qt does it internally, i still want to know the actual index for the emit
    //m_MySlotsAppearingInClassLifeLine.insert(indexActuallyInsertedTo, newSlot);
    int actualIndex = qMax(0, qMin(m_MySlotsAppearingInClassLifeLine.size(), indexToInsertInto));
    m_MySlotsAppearingInClassLifeLine.insert(actualIndex, newSlot);
    emit slotInsertedIntoClassLifeLine(actualIndex, newSlot);
}
void DesignEqualsImplementationClassLifeLine::removeSlotFromClassLifeLine(DesignEqualsImplementationClassSlot *slotToRemove)
{
    m_MySlotsAppearingInClassLifeLine.removeAll(slotToRemove); //TODOreq: does it make sense for a slot to be on the same class lifeline more than once in a single use case? recursion = yes? if that's the case, i need to specify an index instead of removeAll here
    emit slotRemovedFromClassLifeLine(slotToRemove);
}
#if 0
void DesignEqualsImplementationClassLifeLine::replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith)
{
    DesignEqualsImplementationClassSlot *firstAutoGeneratedSlotProbably = m_MySlots.at(indexToReplace);
    m_MySlots.replace(indexToReplace, slotToReplaceItWith);
    delete firstAutoGeneratedSlotProbably; //TODOreq: make sure we've copied out anything valuable it may have contained, including merging if necessary
}
#endif
MySlotsAppearingInClassLifeLine_List DesignEqualsImplementationClassLifeLine::mySlotsAppearingInClassLifeLine() const
{
    return m_MySlotsAppearingInClassLifeLine; //The ordering is mostly undefined, but should basically be however the user designs it (moving them in the design could (should?) move them in the list)
}
bool DesignEqualsImplementationClassLifeLine::hasBeenAssignedInstance()
{
    if(m_InstanceType != NoInstanceChosen)
        return true;
    return false;
}
DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassInstanceTypeEnum DesignEqualsImplementationClassLifeLine::instanceType() const
{
    return m_InstanceType;
}
void DesignEqualsImplementationClassLifeLine::setInstanceType(DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassInstanceTypeEnum instanceType)
{
    m_InstanceType = instanceType;
}
HasA_Private_Classes_Member *DesignEqualsImplementationClassLifeLine::instanceInOtherClassIfApplicable() const
{
    return m_InstanceInOtherClassIfApplicable;
}
void DesignEqualsImplementationClassLifeLine::setInstanceInOtherClassIfApplicable(HasA_Private_Classes_Member *instanceInOtherClassIfApplicable)
{
    m_InstanceType = ChildMemberOfOtherClassLifeline;
    m_InstanceInOtherClassIfApplicable = instanceInOtherClassIfApplicable;
}
QString DesignEqualsImplementationClassLifeLine::instanceVariableName()
{
    QString ret = "";
    if(m_InstanceType == UseCasesRootClassLifeline)
        ret = "this";
    else if(m_InstanceType == ChildMemberOfOtherClassLifeline)
        ret = m_InstanceInOtherClassIfApplicable->VariableName;
    return ret;
}
void DesignEqualsImplementationClassLifeLine::privateConstructor(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationUseCase *parentUseCase, QPointF position)
{
    m_DesignEqualsImplementationClass = designEqualsImplementationClass;
    m_ParentUseCase = parentUseCase;
    //TODOinstancing: , m_MyInstanceInClassThatHasMe(myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline) //Top level object, in this context, I guess really just means "not instantiated in this use case"... or maybe it means "not instantiated by any of the designed classes"... but I'm leaning more towards the first one
    m_Position = position; //Could just keep one qreal "horizontalPosition"

    DesignEqualsImplementationClassSlot *defaultSlotInClassLifeLine = m_DesignEqualsImplementationClass->createwNewSlot(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING);
    //defaultSlotInClassLifeLine->ParentClass = designEqualsImplementationClass;
    insertSlotToClassLifeLine(0, defaultSlotInClassLifeLine); //every lifeline has at least one slot. TODOrq: unit of execution "ordering" does not make sense when you consider that the same object/lifeline could be used in different use cases... fml. HOWEVER since each use case is responsible for holding a set of class life lines, doesn't that mean that all units of execution in a class life line belong to the same use case? EVEN THEN, the nature of threading means we can't make ordering guarantees... blah
}
void DesignEqualsImplementationClassLifeLine::createNewHasAPrivateMemberAndAssignItAsClassLifelineInstance(DesignEqualsImplementationClass *parentClassChosenToGetNewHasAprivateMember, DesignEqualsImplementationClass *typeOfNewPrivateHasAMember, const QString &nameOfNewPrivateHasAMember)
{
    assignPrivateMemberAsClassLifelineInstance(parentClassChosenToGetNewHasAprivateMember->createHasA_Private_Classes_Member(typeOfNewPrivateHasAMember, nameOfNewPrivateHasAMember));
}
void DesignEqualsImplementationClassLifeLine::assignPrivateMemberAsClassLifelineInstance(HasA_Private_Classes_Member *chosenExistingHasA_Private_Classes_Member)
{
    setInstanceInOtherClassIfApplicable(chosenExistingHasA_Private_Classes_Member);
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassLifeLine &classLifeline)
{
#if 0
#define DesignEqualsImplementationClassLifeLine_QDS(qds, direction, classLifeline) \
if(classLifeline.m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline) { \
    qds direction *(classLifeline.m_InstanceInOtherClassIfApplicable); \
        /*actually is streamed implicitly in m_InstanceInOtherClassIfApplicable: qds direction classLifeline.m_InstanceVariableName;*/ \
} \
qds direction classLifeline.m_Position; \
qds direction classLifeline.m_DesignEqualsImplementationClass; \
qds direction classLifeline.m_ParentUseCase; \
qds direction classLifeline.m_MySlotsAppearingInClassLifeLine; \
return qds;
#endif

#if 0
    qds direction classLifeline.m_InstanceType;
#endif
    quint8 instanceType = static_cast<quint8>(classLifeline.m_InstanceType);
    out << instanceType;
    //DesignEqualsImplementationClassLifeLine_QDS(out, <<, classLifeline)
    if(classLifeline.m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
    {
        out << *classLifeline.m_InstanceInOtherClassIfApplicable;
    }
    out << classLifeline.m_Position;
    out << classLifeline.m_ParentProject->serializationClassIdForClass(classLifeline.m_DesignEqualsImplementationClass);
    //is 'known' because use case hasA class lifeline: out << classLifeline.m_ParentProject->serializationUseCaseIdForUseCase(classLifeline.parentUseCase());
    QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
    Q_FOREACH(DesignEqualsImplementationClassSlot* currentSlot, classLifeline.m_MySlotsAppearingInClassLifeLine)
    {
        slotsAppearingInClassLifeline.append(qMakePair(classLifeline.m_ParentProject->serializationClassIdForClass(currentSlot->ParentClass), currentSlot->ParentClass->serializationSlotIdForSlot(currentSlot)));
    }
    out << slotsAppearingInClassLifeline;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassLifeLine &classLifeline)
{
    quint8 instanceType;
    in >> instanceType;
    classLifeline.m_InstanceType = static_cast<DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassInstanceTypeEnum>(instanceType);
    //DesignEqualsImplementationClassLifeLine_QDS(in, >>, classLifeline)
    if(classLifeline.m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
    {
        in >> classLifeline.m_InstanceInOtherClassIfApplicable;
    }
    in >> classLifeline.m_Position;
    int classId;
    in >> classId;
    classLifeline.m_DesignEqualsImplementationClass = classLifeline.m_ParentProject->classInstantiationFromSerializedClassId(classId);
    QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
    in >> slotsAppearingInClassLifeline;
    Q_FOREACH(SerializableSlotIdType currentSlotReference, slotsAppearingInClassLifeline)
    {
        classLifeline.m_MySlotsAppearingInClassLifeLine.append(classLifeline.m_ParentProject->classInstantiationFromSerializedClassId(currentSlotReference.first)->slotInstantiationFromSerializedSlotId(currentSlotReference.second));
    }
    return in;
}
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassLifeLine *classLifeline)
{
    out << *classLifeline;
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassLifeLine *classLifeline)
{
    classLifeline = new DesignEqualsImplementationClassLifeLine();
    in >> *classLifeline;
    return in;
}
