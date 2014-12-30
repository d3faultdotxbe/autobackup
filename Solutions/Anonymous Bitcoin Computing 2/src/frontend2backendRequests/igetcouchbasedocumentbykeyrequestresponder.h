#ifndef IGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define IGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include <string>
#include <sys/types.h>

class IGetCouchbaseDocumentByKeyRequestResponder //TODOoptimization: i only need once instance of these derived classes per TYPE, not per REQUEST/RESPONSE. they are "thread safe" ish (because all relevant data is a method param)
{
public:
    virtual void respond(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)=0;
    virtual void respondWithCAS(unsigned char getAndSubscribe, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)=0;
};

#endif // IGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
