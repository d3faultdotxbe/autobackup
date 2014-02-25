#ifndef SINGLEUSESELFDELETINGMEMORYRESOURCE_H
#define SINGLEUSESELFDELETINGMEMORYRESOURCE_H

#include <Wt/WResource>
#include <Wt/Http/Response>

using namespace Wt;
using namespace std;

class SingleUseSelfDeletingMemoryResource : public WResource
{
public:
    SingleUseSelfDeletingMemoryResource(const std::string &data, WObject* parent = 0);
    ~SingleUseSelfDeletingMemoryResource();
    virtual void handleRequest(const Http::Request &request, Http::Response &response);
private:
    std::string *m_MemoryResource;
};

#endif // SINGLEUSESELFDELETINGMEMORYRESOURCE_H
