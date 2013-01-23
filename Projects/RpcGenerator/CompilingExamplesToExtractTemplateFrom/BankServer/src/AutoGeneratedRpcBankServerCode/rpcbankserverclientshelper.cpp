#include "rpcbankserverclientshelper.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(QObject *parent)
    : IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries(parent), m_RpcBankServerProtocolKnowerFactory(this), m_MultiServerAbstraction(&m_RpcBankServerProtocolKnowerFactory, this), m_Initialized(false)
{
#if 0
    m_ActionDispensers = new RpcBankServerActionDispensers(this);
    takeOwnershipOfActionsAndSetupDelivery();
#endif
    m_BroadcastDispensers = new RpcBankServerBroadcastDispensers(this);
}
#if 0
void RpcBankServerClientsHelper::takeOwnershipOfActionsAndSetupDelivery()
{
    //This method is actually quite worthless now that ClientsHelper and ProtocolKnower have merged... However I'll keep the functionality there JUST IN CASE the destination isn't hte same as the owner in the future. Right now it is, so there doesn't need to be a distinction between the two (as there is in the underlying code... just like broadcast dispensers (except those actually need it))

    m_CreateBankAccountMessageDispenser = m_ActionDispensers->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    m_GetAddFundsKeyMessageDispenser = m_ActionDispensers->takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(this);
}
#endif
#if 0
void RpcBankServerClientsHelper::messageReceived(QByteArray *message, quint32 clientId)
{
    //this design sucks. I don't need clientId to be passed around like this, I just need the deliver() signals to be connected to the right "connection objects" depending on which "connection object" read the request. the only exception to that really is broadcasts, which would iterate/round-robin a list typically not used. It saves me a hash lookup for each message because the signals/slot mechanism performs the lookup (that has to be performed anyways) for me. It would take some redesigning to the dispener logic, but is definitely do-able :-/. Man I've got like 20 fucking optimizations/changes in progress right now, my brain's going to explode. And to think, this application worked at one point. There was one commit where it fucking worked! No ack scheme (aside from shitty TCP ack), but it worked! I'm starting to wonder if I'll ever see it in working state ever again. This is a mess. Sorting this mess out SEEMS like a prerequisite to my life. Perhaps it is, but perhaps a complete startover from scratch would save me a lot of sanity? Meh I doubt it, perhaps a design from scratch accomodating all these pending changes? Followed of course by a new implementation of it? Or do I keep hacking/chipping away at this POS? This is a great point to sync/backup/stop/etc for the holidays (going out of town etc). Fuck My Life
}
#endif
void RpcBankServerClientsHelper::processCreateBankAccountMessage(CreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId)
{
    //our first check is for the lazy ack ack, as it is the most likely. we COULD check that it's in business still (I guess this depends on if the retry bit is set?), but since that's less likely we'll check it next

    //first just see if it's there. it will most likely be there. if it isn't, it's very likely in business pending...
    CreateBankAccountMessage *messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = m_CreateBankAccountMessagesAwaitingLazyResponseAck.value(createBankAccountMessage->Header.MessageId, 0);

    if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
    {
        //so it's in the ack awaiting ack list, now we need to determine if we're lazy ack'ing an old message or if we want to re-send the response. this is why we lazy ack to begin with...

        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->toggleBit() == createBankAccountMessage->toggleBit())
        {
            //if they are the same, retryBit is implied and we just return the one that's already existing.
            //TODOreq: wtf do I do with the new one? just recycle it right here right now? I think so but am unsure...

            //it might seem inefficient that we have two copies of the message at this point... BUT WE (client) DON'T KNOW THAT THE FIRST ONE ARRIVED MOTHERFUCKER... so we did have to send the whole request again. HOWEVER, the server has determined it's worthless.
            //TODOmessageErrors: this is our response retry case

            createBankAccountMessage->doneWithMessage(); //doesn't have response filled out anyways~ piss off

            //we re-send the old one, but still don't remove it from the ack pending ack. what if this one gets lost too? actually TODOreq: since this would be our second attempt at a response, there is definitely not going to be a third SO it might be ok to say doneWithMessage at this point??????? it would mean that the next time we see the MessageId, it will be treated as though it was the first~ <---- all of this is only true if we only retry exactly once. err, try exactly twice.

            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->deliver(); //TODOreq: the handler of deliver() expects only to see messages coming from business. It removes them from the pending in business. It shouldn't matter that we remove our messageId/message from a hash that we aren't in, but it is a worthwhile optimization to NOT do so. It also re-appends us to our ack-awaiting-ack list... so maybe I should just call myTransmit here directly instead???? just don't have that clientId, but I think I'm going to be adding that as a member of IActionMessage anyways???? (it does not apply to broadcast I don't think? (aside from them special client-prioritized broadcasts that I'm not even sure I can use yet lmfao, which'd probably include a db lookup of some sort to figure out who that client is anyways? really no fucking clue))
        }
        else
        {
            //they are not the same, so we know we're on the next message. We can now recycle the old one: we know we don't need it anymore. The ack has officially been ack'd :-D
            m_CreateBankAccountMessagesAwaitingLazyResponseAck.remove(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->Header.MessageId);
            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();


            //TODOreq: now that that's taken care of, we begin processing the new message

            //TODOmessageErrors: this is our typical non-error case, but for every time after the first time we use that MessageId

            dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
        }
    }
    else
    {
        //not in ack lazy awaiting ack list, so it might be in business pending... or TODOreq: it might be our first time seeing it and in neither!!!

        //hack: the name of the pointer doesn't match our usage, but no point in allocating another one lol...
        messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = m_PendingCreateBankAccountMessagesInBusiness.value(createBankAccountMessage->Header.MessageId, 0);

        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        {
            //TODOreq: still in business. should we respond with a 'give us more time plox because if this doens't work itself out, shit is FUCKED and everything should shut down'? TODOreq: so does that imply that the client sends us yet another (3rd!!!) try? And if THAT ONE doesn't work, then we're really really fucked? Or should I ignore it (store it?) and set up a timer on the business and then report fucked? I like the idea of staying in communication with the client at this point, even though we don't have much to say except "it's the business, not us". :-/. This is probably one of the hardest use cases to decide what to do... but I guess I'd say opt towards giving him more time before shutting the fuck down... or even trying another server
            //I'm going to be so ridiculously happy if I never ever encounter this code path. I mean sure during development you are bound to fuck up... but your business MUST respond in production... else we shut everything the fuck down (TODOreq: this is a good as fuck reason to keep applications... servers... whatever.... appIds.... AWAY FROM EACH OTHER. so if one service goes down it doesn't bring down everything else that is working perfectly fine. I guess I could just do it on a per-appId basis... if I really want to keep everything "one-giant-binary-to-rule-them-all" roflrofl good luck stealing my source if the source never touches the server, only binaries do. even though that has nothing to do with me doing them in separate binaries lol. but yea I mean I think you get more efficiency and therefore scalability and therefore profits by having all your services on ONE GIANT DHT. I don't even want to think about 'providing low latency access to overseas countries' though. I guess those would just be copy/pasta different servers/clusters altogether? It'd work for services like email etc where the user has their own account... but for shit like ABC AdDoubles, we need worldwide coordination so it's MUCH easier to just have a giant ass single dht like I said. The latency has to be dealth with either way. Either inter-cluster realtime synchronization (NO-FUCKING-THANKS (not to be confused with cross-cluster backing up, which yea i'll have running 24/7 both ways (or 3 ways etc), but they are DELAYED, not realtime!!! (also: still dunno how rolling back or whatever would work with a delayed/lagged copy. but shit i know one thing: it's better to have a few seconds/minutes outdated copy than NO COPY AT ALL (in the case of fire/explosion/etc lmfao)))) or have the user do it (yes))

        }
        else
        {
            ////TODOmessageErrors: non-error case, first time seeing this MessageId
            dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
        }
    }


    //TODOreq: somewhere in the above I need to actually toggle the bit. Or wait no, client does that right???? still worth noting.


    //check in business still if retry bit is set? actually maybe i don't even need the retry bit anymore if I'm using a toggling-per-messageId-use design. if I get the same messageId without the bit toggled, then retry is IMPLIED????


    m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

    //I think this is old, idfk:
    //TODOreq: perhaps check the list already by value to see if we're already working on the same request?
    //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
    //the list is mutex-grabbed+cleared AFTER we transmit the message back to
    //actually this idea sucks
    //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

    //emit createBankAccount(createBankAccountMessage);
}
void RpcBankServerClientsHelper::dispatchCreateBankAccountMessageToBusiness(CreateBankAccountMessage *createBankAccountMessage)
{
    //add it to pending in business...
    m_PendingCreateBankAccountMessagesInBusiness.insert(createBankAccountMessage->Header.MessageId, createBankAccountMessage);

    emit createBankAccount(createBankAccountMessage);
}
void RpcBankServerClientsHelper::processGetAddFundsKeyMessage(GetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.insert(getAddFundsKeyMessage, uniqueRpcClientId);
    emit getAddFundsKey(getAddFundsKeyMessage);
}
#if 0
void RpcBankServerClientsHelper::createBankAccountDelivery()
{
    //perhaps here is best place to keep track of pendings? because after this spot they have all converged and i'd have to do a switch or something (not necessarily if i increaes the size of each message by keeping a pointer to it's pending list. bad idea. better to do it here)

    //so these transmits right now make it APPEAR i should just connect every .deliverSignal() to a single slot... but for keeping track of pending messages (which i have yet to do), that will turn out not to be the case

    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);

    //Should this go after myTransmit? It probably doesn't make a difference...
    m_CreateBankAccountMessagesAwaitingLazyResponseAck.append(createBankAccountMessage->Header.MessageId, createBankAccountMessage);
    m_PendingCreateBankAccountMessagesInBusiness.remove(createBankAccountMessage->Header.MessageId);

    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    //createBankAccountMessage->doneWithMessage();

    //I think the below is old too, but shit I can't remember the ACK scheme solution I came up with xD. Wasn't it two lists: m_PendingInBusiness and m_AlreadyProcessedAndReturnedButWeCheckInHereWhenTheSameIdComesWeKnowClientGotIt --- or was that for broadcasts? Where the fuck did I put that design. How much of it do I have coded :-/?????? Fuck.

    //^^^^^^^^^^^^^^
    //TODOreq: unsure of ordering of transmit and removing from pending hash. probably doesn't matter? no fucking clue tbh. changed it from .remove to .value and then to .take -- i guess i don't have a choice on the ordering? i think it only matters that the backend impl has in fact finished with the pending. so even if one does get through, the backend impl will catch it definitely
    //^^i like having them overlap by 1 whole line. I doubt it does ANYTHING AT ALL, but i like to know that it's in the ack-awaiting-ack list before we remove it from the business pending one. we check the ack-awaiting-ack list first... so if for some weird random rare as fuck case we crash in between those two calls, and still somehow manage to get back in here, the check for ack-awaiting-ack will come true and we'll simply overwrite the pending in business one (this is desired)
}
void RpcBankServerClientsHelper::getAddFundsKeyDelivery()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
#endif
void RpcBankServerClientsHelper::pendingBalanceDetectedDelivery()
{
    //TODOreq: package as QByteArray and sned to MultiServerAbstraction. old code below

    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = static_cast<ServerPendingBalanceDetectedMessage*>(sender());
    pendingBalanceDetectedMessage->Header.MessageType = RpcBankServerMessageHeader::PendingBalanceDetectedMessageType; //TODOreq: shouldn't this be set by the dispenser or something? somewhere in the message itself? where the fuck am i setting the MessageType for actions (find MessageType usages = solution). The answer lies somewhere in the client code, as the server never sets a message type (except when reading a message type off the network from the client)
    pendingBalanceDetectedMessage->Header.MessageId = 0; //TODOreq: going to need a messageId scheme for broadcasts after all, since I'm going to be manually ack'ing them (and lazy acking the ack itself). But this is for later on after I get actions working...
    myBroadcast(pendingBalanceDetectedMessage);
    pendingBalanceDetectedMessage->doneWithMessage(); //old TODOreq: shouldn't this be after the ACK? might have to re-send it... i guess it depends on the guarantees made by myBroadcast. if before it returns it writes to a couchbase db and WAL promises the delivery, then yes calling doneWithMessage() now is probably* ok. just make sure you know to allocate one whenever we are walking the WAL (either as us or a neighbor [same code, different machine]). getNewOrRecycled _cannot_ be used (bitcoin thread owns dispenser). so maybe we shouldn't do doneWithMessage until the ack IS here??? idfk

    //TODOreq: Pretty sure my broadcasts still need their ACK'ing scheme to be implemented... but I'm going to wait until my Action's ACK-ing scheme is in place + working to do it (since I'll steal it's code :-P)
}
void RpcBankServerClientsHelper::confirmedBalanceDetectedDelivery()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = static_cast<ServerConfirmedBalanceDetectedMessage*>(sender());
    confirmedBalanceDetectedMessage->Header.MessageType = RpcBankServerMessageHeader::ConfirmedBalanceDetectedMessageType;
    confirmedBalanceDetectedMessage->Header.MessageId = 0;
    myBroadcast(confirmedBalanceDetectedMessage);
    confirmedBalanceDetectedMessage->doneWithMessage();
}
void RpcBankServerClientsHelper::initialize(MultiServerAbstractionArgs multiServerAbstractionArgs)
{
    emit d("RpcBankServerClientsHelper received init message");
    m_MultiServerAbstraction.initialize(multiServerAbstractionArgs);
    m_Initialized = true;
    emitInitializedSignalIfReady();
}
void RpcBankServerClientsHelper::start()
{
    emit d("RpcBankServerClientsHelper received start message");
    m_MultiServerAbstraction.start();
    emit started();
}
void RpcBankServerClientsHelper::beginStoppingProcedure()
{
    //TODOreq: make it so incoming Action Requests are rejected with special error code, perhaps by calling a method on m_MultiServerAbstraction... or just by setting up a flag and responding directly to any incoming Action Requests right away in 'this'
    //^^ It makes more sense to have 'this' take care of it, since that 2-stage server-shutdown procedure is specific to the rpc generator. HOWEVER it might be a race condition that a message is in MultiServerAbstraction still? I don't even know what the fuck I mean by this, probably wrong...

    emit beginningStopProcedureInitiated();
}
void RpcBankServerClientsHelper::stop()
{
    emit d("RpcBankServerClientsHelper received stop message");
    m_MultiServerAbstraction.stop();
    emit stopped();
}
void RpcBankServerClientsHelper::handleDoneClaimingBroadcastDispensers()
{
    emitInitializedSignalIfReady();
}
#if 0 // leaving this huge chunk of code here because i see a bunch of TODOreqs that I don't feel like reading
void IBankServerProtocolKnower::createBankAccountFailedUsernameAlreadyExists()
{
    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);
    //TODOreq: handle error where the message wasn't in pending for some reason :-/. do it for regular delivery() too...
    createBankAccountMessage->fail(CreateBankAccountMessage::FailedUsernameAlreadyExists);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();
}
void IBankServerProtocolKnower::createBankAccountFailedPersistError()
{
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUsernameDoesntExist()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedUsernameDoesntExist); //sets IMessage->Header.Success to false, which causes IMessage::quint32 failedReason to be set with the enum and also streamed. it isn't streamed when success is true. broadcasts have no use for the Success/failReason shit, so maybe it should be in a class that only Actions inherit? Actions on both client and server need it, and neither Broadcasts on client nor server need it. errors are action specific. FUCK, Success is in the _header_...... i could just not stream it for broadcasts, but then i have to do expensive switching... or stream an additional bool 'isAction'.... which would cost the same as just streaming Success in the first place rofl...

    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUseExistingKeyFirst()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedUseExistingKeyFirst);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedWaitForPendingToBeConfirmed()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedWaitForPendingToBeConfirmed);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
#endif
