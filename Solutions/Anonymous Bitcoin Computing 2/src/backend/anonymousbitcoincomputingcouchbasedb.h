#ifndef ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
#define ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H

#include <libcouchbase/couchbase.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

#include "getandsubscribecacheitem.h"
#include "autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasegetrequest.h"
#include "autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.h"

//#define ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE

using namespace boost::interprocess;

typedef boost::unordered_map<std::string /*key*/, GetAndSubscribeCacheItem* /*doc,cas,listOfSubscribers*/> GetAndSubscribeCacheHashType;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#define ABC_COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *m_##text##EventCallbackForWt##n;

#define ABC_COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO(z, n, text) \
message_queue *m_##text##WtMessageQueue##n;

#define ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO(text) \
void *m_##text##MessageQueuesCurrentMessageBuffer; //TODOoptimization: really only need one message buffer queue on backend, but it needs to be the size of the largest type of message (store,storeLarge,get <- most likely storeLarge's size in those 3 xD)

#define ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *get##text##EventCallbackForWt##n();

#define ABC_COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO(z, n, text) \
void eventSlotForWt##text##n();

#define ABC_COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO(z, n, text) \
static void eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData);

#define ABC_COUCHBASE_LIBEVENTS_SINGULAR_SLOT_METHOD_DECLARATIONS_MACRO(text) \
void eventSlotForWt##text();

#define ABC_COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO(z, n, text) \
, m_##text##EventCallbackForWt##n(NULL)

#define ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO(text) \
, m_##text##MessageQueuesCurrentMessageBuffer(NULL)

#define ABC_COUCHBASE_MESSAGE_QUEUE_NULL_INITIALIZATION_MACRO(z, n, text) \
, m_##text##WtMessageQueue##n(NULL)

#define ABC_SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO(z, n, text) \
m_##text##EventCallbackForWt##n = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, eventSlotForWt##text##n##Static, this);

#define ABC_FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(text) \
free(m_##text##MessageQueuesCurrentMessageBuffer); \
m_##text##MessageQueuesCurrentMessageBuffer = NULL;

#define ABC_REMOVE_ALL_MESSAGE_QUEUES_MACRO(z, n, text) \
message_queue::remove(ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n);

#define ABC_CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO(z, n, text) \
delete m_##text##WtMessageQueue##n; \
m_##text##WtMessageQueue##n = NULL;

#define ABC_MALLOC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(text) \
m_##text##MessageQueuesCurrentMessageBuffer = malloc(ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text);

#define ABC_NEW_AND_CREATE_MY_MESSAGE_QUEUES_MACRO(z, n, text) \
m_##text##WtMessageQueue##n = new message_queue(create_only, ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n, ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_##text, ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text);

#define ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO(z, n, text) \
struct event *AnonymousBitcoinComputingCouchbaseDB::get##text##EventCallbackForWt##n() \
{ \
    return m_##text##EventCallbackForWt##n; \
}

#define ABC_COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData) \
{ \
    (void)unusedSocket; \
    (void)events; \
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->eventSlotForWt##text##n(); \
}

//My assumption (or, faulty understanding) that libevents were like signals and that emitting them multiple times would cause the receiving slot (callback) to be invoked the same amount of times was wrong. The 'event' is either active or inactive, and making it active when it's already active does nothing. I'd rather be wrong and have to change a tiny bit of code (adding the below while(try_receive) now) than be right and have to refactor a ton of it (a la lockfree::queue or some other solution)
//TO DOnereq(using 3 max (seen as '< 4' in code) successful try_receives before giving other queues a chance): now that i've changed to a while(try_receive) design, there's a very real possibility that when a server's at high load, that a SINGLE queue will never leave that while loop! it needs to be fair-ER and give the other queues a chance, even when there are more. Random [FAIL (keep readan]] solution: if the 2nd try_receive succeeds, we add our queue # to a list (queue ;-P) that we 'come back to' after trying... err... other... queues. but if the 2nd try_receive fails, we don't add ourselves to that queue because then we know event_active will take care of it. but when to process that list/queue? when a 2nd try_receive fails i suppose... but we'd need to also make it loop so that if we're checking BECAUSE of queued, we can re-enter queued if we try_receive two in a row. so two in a row are ever at most tried, THEN WE GO BACK TO LETTING LIBEVENT DECIDE WHICH MESSAGE QUEUE. if only one try_receive from that... aww shit now there's the problem of being in the accidental 'sweet spot' (bad in this case).... basically we could be stuck ONLY processing ones with 1x message in their queue that are incidently in that list/queue but never then processing the libevent ones------ lockfree::queue here i come..... WAIT NO lockfree::queue doesn't help me at all here ffffffff, i'd be in the same boat... oh shit oh shit...
//^^maybe timers of length 0 (so long as they don't optimize themselves and become direct calls) whenever a 2nd try_receive succeeds? or hell, make active the very same one that got us here if 2nd try_receive succeeds? it IS thread-safe after all (again, as long as it doesn't optimize itself to become direct call). there is also the current number of messages, so i could check that it's > 0 after just one check (or do 2 or N) and then make the event active based on that...
#define ABC_COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n() \
{ \
    unsigned int priority; \
    message_queue::size_type actualMessageSize; \
    unsigned char currentReadsBeforeGivingOtherQueuesAchance = 0; \
    bool receiveSuccess; \
    do \
    { \
        if(!(receiveSuccess = m_##text##WtMessageQueue##n->try_receive(m_##text##MessageQueuesCurrentMessageBuffer,(message_queue::size_type)ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text, actualMessageSize, priority))) \
        { \
            return; \
        } \
        eventSlotForWt##text(); \
    }while(++currentReadsBeforeGivingOtherQueuesAchance < 4); \
    if(receiveSuccess && (m_##text##WtMessageQueue##n->get_num_msg() > 0)) \
    { \
        event_active(m_##text##EventCallbackForWt##n, EV_READ|EV_WRITE, 0); \
    } \
}

#define ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO \
{ \
    boost::lock_guard<boost::mutex> couchbaseConnectingLock(m_IsConnectedMutex); \
    m_IsDoneInitializing = true; \
} \
m_IsConnectedWaitCondition.notify_one();

#define ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(GetOrStore) \
--m_Pending##GetOrStore##Count; \
if(m_NoMoreAllowedMuahahaha && m_PendingStoreCount == 0 && m_PendingGetCount == 0) \
{ \
    notifyMainThreadWeAreFinishedWithAllPendingRequests(); \
}

//TO DOneoptional: delete this now worthless macro (theoretically, all macros are worthless (because everything evaluates to one))

#define ABC_READ_STORE_REQUEST_FROM_APPROPRIATE_MESSAGE_BUFFER(storeSize) \
if(m_NoMoreAllowedMuahahaha) \
{ \
    return; \
} \
StoreCouchbaseDocumentByKeyRequest *originalRequest = new StoreCouchbaseDocumentByKeyRequest(); \
{ \
    std::istringstream originalRequestSerialized(static_cast<const char*>(m_##storeSize##MessageQueuesCurrentMessageBuffer)); \
    boost::archive::text_iarchive deSerializer(originalRequestSerialized); \
    deSerializer >> *originalRequest; \
}

//You know you're a god when you can use a BOOST_PP_REPEAT within a call to BOOST_PP_REPEAT
#ifndef ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO(z, n, text) \
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET##n, text, ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#define ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(a,b) a(b)
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO(z, n, text) \
ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(text,ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#endif

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

class AnonymousBitcoinComputingCouchbaseDB
{
public:
    AnonymousBitcoinComputingCouchbaseDB();
    boost::condition_variable *isConnectedWaitCondition();
    boost::condition_variable *isFinishedWithAllPendingRequestsWaitCondition();
    boost::mutex *isConnectedMutex();
    boost::mutex *isFinishedWithAllPendingRequestsMutex();
    struct event *beginStoppingCouchbaseCleanlyEvent();
    struct event *finalCleanUpAndJoinEvent();
    bool isDoneInitializing();
    bool isConnected();
    bool isInEventLoop();
    bool isFinishedWithAllPendingRequests();
    void waitForJoin();
    bool threadExittedCleanly();

    //struct event *getStoreEventCallbackForWt0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO)
private:
    friend class AutoRetryingWithExponentialBackoffCouchbaseGetRequest;
    friend class AutoRetryingWithExponentialBackoffCouchbaseStoreRequest;

    boost::thread m_Thread;
    lcb_t m_Couchbase;
    boost::condition_variable m_IsConnectedWaitCondition;
    boost::condition_variable m_IsFinishedWithAllPendingRequestsWaitCondition;
    boost::mutex m_IsConnectedMutex;
    boost::mutex m_IsFinishedWithAllPendingRequestsMutex;
    bool m_IsDoneInitializing;
    bool m_IsConnected;
    bool m_ThreadExittedCleanly;
    bool m_IsInEventLoop;
    bool m_NoMoreAllowedMuahahaha;
    unsigned int m_PendingStoreCount;
    unsigned int m_PendingGetCount; //subscribers do NOT increment the pending count (well, one of them does when it's being used hackily in the polling mechanism). pending count ONLY reflects dispatched-to-couchbase 'gets' that have not returned yet. most subscribers do not do that, hence no incremement. when we 'clean up', we simply forget all the subscribers.
    bool m_IsFinishedWithAllPendingRequests;

    GetAndSubscribeCacheHashType m_GetAndSubscribeCacheHash;
    std::vector<void*> m_UnsubscribeRequestsToProcessMomentarily;
    std::vector<void*> m_ChangeSessionIdRequestsToProcessMomentarily;
    static const struct timeval m_OneHundredMilliseconds;
    struct event *m_GetAndSubscribePollingTimeout; //all keys share a timeout for now (and possibly forever), KISS
    std::vector<AutoRetryingWithExponentialBackoffCouchbaseStoreRequest*> m_AutoRetryingWithExponentialBackoffCouchbaseStoreRequestCache;
    std::vector<AutoRetryingWithExponentialBackoffCouchbaseGetRequest*> m_AutoRetryingWithExponentialBackoffCouchbaseGetRequestCache;

    void threadEntryPoint();

    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    void errorCallback(lcb_error_t error, const char *errinfo);

    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    void configurationCallback(lcb_configuration_t config);

    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void decrementPendingStoreCountAndHandle();

    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void decrementPendingGetCountAndHandle();

#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    static void durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);
    void durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);
#endif

    static void getAndSubscribePollingTimeoutEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void getAndSubscribePollingTimeoutEventSlot();

    static void beginStoppingCouchbaseCleanlyEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void beginStoppingCouchbaseCleanlyEventSlot();

    static void finalCleanupAndJoin(evutil_socket_t unusedSocket, short events, void *userData);

    void notifyMainThreadWeAreFinishedWithAllPendingRequests();

    struct event *m_BeginStoppingCouchbaseCleanlyEvent;
    struct event *m_FinalCleanUpAndJoinEvent;

    //message_queue *m_StoreWtMessageQueue0;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO)

    //void *m_StoreMessageQueue0CurrentMessageBuffer;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO)

    //struct event *m_StoreEventCallbackForWt0;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO)

    //static void eventSlotForWtStore0Static(evutil_socket_t unusedSocket, short events, void *userData);
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO)

    void scheduleGetRequest(AutoRetryingWithExponentialBackoffCouchbaseGetRequest *autoRetryingWithExponentialBackoffCouchbaseGetRequest);
    void scheduleStoreRequest(AutoRetryingWithExponentialBackoffCouchbaseStoreRequest *autoRetryingWithExponentialBackoffCouchbaseStoreRequest);
    //void eventSlotForWtStore0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO)

    //the static methods catch parameters (evutil_socket_t unusedSocket, short events), but we don't use them currently
    //void eventSlotForWtStore();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_COUCHBASE_LIBEVENTS_SINGULAR_SLOT_METHOD_DECLARATIONS_MACRO)
};

#endif // ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
