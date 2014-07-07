#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include <QString>
#include <QList>

#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationClassSlot;

class DesignEqualsImplementationSlotInvocationStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassSlot *slotToInvoke, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    virtual ~DesignEqualsImplementationSlotInvocationStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
private:
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    SignalEmissionOrSlotInvocationContextVariables m_SlotInvocationContextVariables;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H