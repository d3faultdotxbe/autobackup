//THIS FILE IS NOT GENERATED BY RPC GENERATOR. IT IS ONLY TO GET MY AUTO-DRILLDOWN STUFF WORKING ASAP IN THE COMPILING TEMPLATE SOURCE
#include "cleanroomdoc.h"

#include "cleanroomdb.h"

bool operator==(const CleanRoomDoc &e1, const CleanRoomDoc &e2)
{
    return e1.cleanRoomDocHash() == e2.cleanRoomDocHash();
}
uint qHash(const CleanRoomDoc &key, uint seed)
{
    //return qHash(CleanRoomDb::sha1Hex(key.Data) /*TODOoptimization: lazy generate+cache an sha1sum of the data for this purpose, but only if threadsafe. also applies to equals operator*/, seed) ^ qHash(key.Username, seed) ^ qHash(key.Timestamp, seed) ^ qHash(key.LicenseIdentifier /*TODOoptional: consider the effects of allowing a doc to change license identifiers. maybe don't allow it (a license identification change is a REVISION/derivative of a doc, so yea nvm)*/, seed);
    return qHash(key.cleanRoomDocHash(), seed);
}