#include <QApplication>

#include "designequalsimplementationgui.h"

#if 0
TODOreq:
actor -> a::x -> b::x2
         a::x -> b::x3
         a::x?<- b::y   <----- DOH, two entry points to x, probably causes the segfault? should fix this in gui then. HOWEVER, two entry points to a single slot might be desireable (two signals attached to one slot) -- but in THIS case it would be an infinite loop so maybe not (even still, "x" might have a complex switch statement that knows when to loop and when not to)
actor <- a::y2
#endif
//TODOreq: the deserializing + source generation should be pushed into a lib [and cli] for automation (*cough*build-chain*cough*), but core UML editting stuff doesn't need to (and probably couldn't (well... (fuck this "well")))
//LoL: if this design (first of all makes it far enough) ever gets too complicated or I realize I took some wrong directions after the fact, I can re-design it WITHIN ITSELF and then discard the old one :-P..... over and over infinitely...
//I keep fantacizing about this app ultimately generating __ANSI C__, Boost, or Qt... as a radio option at source generation time. ANSI C would be the coolest xD, and also (had:easily) the hardest.... BUT imfo every single C++ feature in existence can be coded in ANSI C. It's so stupid that the C++ committee keeps bumping versions and requirements and dependencies blah. Their definition of backwards compatibility is source compatibility, whereas I like to keep the runtime the same always and just keep making the precompiler fatter and fatter :-). C89/ANSI-C is pretty much the largest supported language/runtime in existence (and will probably continue to be for.. like.. ever). I'm not sure whether or not pthreads is ANSI-C though (GNU Pth is, but doesn't look like it supports win. Oh nvm anyways looks like Pth is a thread emulator only)
//^^At the very least, the "library" generation type mode (when no threads involved) could very easily be written in ANSI-C (but actually since I'm conceptuatlly using "slot invokes" instead of "method calls", thread safety is implied when designing so... bleh. This app is first and foremost written for Qt obviously)
//If I allow users to modify the generated code, I could use "states" and libClang to parse out their changes and bring them forward. Specifically, I record their "previously generated source state", intelligently diff it with their modifications, use libClang to get the good stuff (body of a method (which may have been empty at design time), for example) out, generate the new source state, and bring their modifications forward (again, using libClang). I could be strict about it and error out without changing anything if ANY of their changes are unable to be brought forward (requiring more hacking of this here app). Also maybe kind of makes sense that I use libClang to read in their changes for this app's 'project [de-]serialization', but that does kind of break the one definition rule UNLESS all of the generated code is not considered permanent. That would put the user in a weird state where designEquals might not be open and they modified code but it never got sucked back into this app's "project" and so was lost (like if the code generated was not in the heirarchy of a git repo since it's "temporary"). I have lots of strategies for raw C++ mode, one which just includes magic strings at the beginning and ending of methods designed in the UML explicitly for raw C++ (libClang solution is still cleaner though), but even that solution has the synchronization problem just mentioned. One solution to the synchronization problem is to never let them see the final C++ source code (unless explicitly requested) and to just generate binaries, EXCEPT they _DO_ see the C++ code if they ever "drop down to C++" mode... and when in such mode they are warned to NOT close designEquals until QtCreator is closed [and the changes are sucked back in for [de-]serialization. The temporary files could exist on something like /run/shm/ so they get the idea, and are deleted after being sucked back in to designEquals (which happens with QtCreator closes). That synchronization solution works for both raw C++ solutions just described.
//TODOreq: not sure this even makes any sense, but perhaps this app can GENERATE implicitly shared classes :-P. Maybe it's opt-in, opt-out, or radio between explicit/implicitly shared, idfk
//TODOreq: if you connect what will become fooSlot to barSlot before you connect actor to fooSlot, it is an error if fooSlot has any statements already. we could prompt the user if they want to insert (before, after, custom merge, etc) or choose new slot name for fooSlot (avoiding the merge altogether). if fooSlot is named but as of now empty, it is a valid operation
//TODOreq: one signal can be connected to multiple slots (backend + frontend). I mean this for within a single use case, but obviously additionally even across use cases.
//TODOreq: call slot defined in other use case ("slot reference")
//TODOreq: the dummy pre-named slot stuff might need to be refactored, for example if two dummy slots are on a single class lifeline (or maybe even just class), they might collide? I think really allowing empty string named slots and referring to them by pointer is what I should do
//TODOreq: C++ statement, including C++ IDE integration (spit out, suck in)
//TODOreq: do not allow the same instance of class [lifeline] to be in the same use case. attempting to do so should give an appropriate error message (or perhaps the already-used-instance should be unselectable (BUT i still think there should be some visual cue notifying them of why they can't select it. without said visual cue, one might go crazy trying to select an instance and have no clue why it isn't showing up in the list)
//TODOreq: perhaps "DesignEqualsImplementationType" (wherever a type is mentioned), with two "modes" selectable via enum: InternallyDesignedType, QtOrCppBuiltinOrOtherKnownByStringType. HOWEVER, how would I use an internally-designed type as a method argument as const/ref/ptr???
//this project has turned into massive spaghetti code, but i have infinite energy to (had:i'm inclined to) 'deal with it' because the very problem the app attempts to solve is spaghetti code itself
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DesignEqualsImplementationGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
