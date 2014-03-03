#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- make some use of auto-failover shit?
- doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
- No ops cause too big of messages to be sent through the message queue. This is mostly done already via sanitization
- satoshis, since i've witnessed the hilarious rounding stuff (only where it matters (js does not)). however if i use satoshis then i'll never get off 0.00000001 (because integer math CHOPS. could be wrong though). i need to conver the satoshi to a doubles for the calculating, then call 'round()' to round it to nearest satoshi, then convert back to int64 i guess? i'm so confused, won't that solution not work for the larger numbers?


==1.1 Bugs==

- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further
- subscription update to all old subscribers (could be ~10k if app scales like it should), which only happens at buy event, shouldn't make the db unusable and currently would/does. basically just put the series of WServer::post calls on a separate thread (or all the get and subscribe stuff on a separate thread...)

*/

int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}
