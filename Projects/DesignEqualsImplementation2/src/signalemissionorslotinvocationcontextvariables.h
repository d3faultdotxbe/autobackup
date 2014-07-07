#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>
#include <QMetaType>

struct SignalEmissionOrSlotInvocationContextVariables
{
    QString VariableNameOfObjectInCurrentContextWhoseSlotIsAboutToBeInvoked; //Slot only. TODOoptional: move to interface so signals don't have+ignore this
    QList<QString> OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments; //Signal emissions and slot invocations both use "variables in current context" to populate teh args
};

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H