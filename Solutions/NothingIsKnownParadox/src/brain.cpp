#include "brain.h"

Brain::Brain(QObject *parent)
    : QObject(parent)
{
    //all these queued connections are to let execution return to the event loop (which lets the stack unwind, preventing the app from running out memory and crashing)
    connect(this, SIGNAL(whatIsThisQuestioned()), this, SLOT(thinkDeepAboutWhatThisIs()), Qt::QueuedConnection);
    connect(this, SIGNAL(askSelfWhatIsKnown()), this, SLOT(evaluateKnownThings()), Qt::QueuedConnection);
    connect(this, SIGNAL(newInformationLearned()), this, SLOT(evaluateKnownThings()), Qt::QueuedConnection); //new information can, and often does, invalidate old information
    connect(this, SIGNAL(oldInformationTurnedOutToBeFalse()), this, SLOT(evaluateKnownThings()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(this, "realizeSomethingIsHappening", Qt::QueuedConnection);
}
bool Brain::isTrue(const QString &claimBeingAnalyzed)
{
    if(claimBeingAnalyzed == NOTHING_IS_KNOWN_CLAIM)
    {
        return m_ThingsKnown.isEmpty();
    }
    //else if(logic == etc)
    //{ }
    return false;
}
void Brain::makeClaim(const QString &theClaim)
{
    if(isTrue(theClaim))
    {
        addKnownThing(theClaim);
    }
}
void Brain::addKnownThing(const QString &thingKnown)
{
    m_ThingsKnown << thingKnown;
    emit o("I now know: '" + thingKnown + "'");
    emit newInformationLearned();
}
void Brain::removeKnownThing(const QString &thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse)
{
    m_ThingsKnown.removeOne(thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse);
    emit o("I used to think: '" + thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse + "', but it turned out to be false");
    emit oldInformationTurnedOutToBeFalse();
}
void Brain::realizeSomethingIsHappening()
{
    //A brain randomly instantiates into existence, realizes something is happening, and asks...
    emit o("What is this?");
    emit whatIsThisQuestioned();
}
void Brain::thinkDeepAboutWhatThisIs()
{
    emit o("Hmm, well what do I know?");
    emit askSelfWhatIsKnown();
}
void Brain::evaluateKnownThings()
{
    if(m_ThingsKnown.isEmpty())
    {
        makeClaim(NOTHING_IS_KNOWN_CLAIM); //I'm tempted to have some kind of "observe that nothing is known" signal/slot, but actually not having those signals/slots illustrates the bootstrapping nature of our brain
    }
    else
    {
        Q_FOREACH(const QString &thingKnown, m_ThingsKnown)
        {
            if(isTrue(thingKnown))
            {
                emit o("I already know: " + thingKnown); //I don't think this app gets to this block of code, but eh I couldn't think what else to put here
            }
            else
            {
                removeKnownThing(thingKnown);
            }
        }
        //An evolved implementation would utilize all the observations in the above foreach and possibly theorize something new here
    }
}
