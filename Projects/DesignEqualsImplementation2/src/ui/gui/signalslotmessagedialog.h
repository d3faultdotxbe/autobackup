#ifndef SIGNALSLOTMESSAGEDIALOG_H
#define SIGNALSLOTMESSAGEDIALOG_H

#include <QDialog>

#include "../../designequalsimplementationusecase.h"
#include "../../signalemissionorslotinvocationcontextvariables.h"
#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"

class QVBoxLayout;
class QCheckBox;
class QComboBox;
class QAbstractButton;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLine;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class IHaveTypeAndVariableNameAndPreferredTextualRepresentation;

class SignalSlotMessageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode , DesignEqualsImplementationClassSlot *destinationSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor,  bool destinationIsActor, DesignEqualsImplementationClassLifeLine *sourceClassLifeLine_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLine *destinationClassLifeLine_OrZeroIfNoDest, DesignEqualsImplementationClassSlot *sourceSlot_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSignal *signalToEmit_OrZeroIfNone() const;
    DesignEqualsImplementationClassSlot *slotToInvoke_OrZeroIfNone() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
    bool signalIsExistingSignalFlag() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QVBoxLayout *m_Layout;
    QCheckBox *m_SignalsCheckbox;
    QCheckBox *m_SlotsCheckbox;
    QWidget *m_SignalsWidget;
    QWidget *m_SlotsWidget;
    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot *m_ExistingSignalsComboBox;
    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot *m_ExistingSlotsComboBox;
    DesignEqualsImplementationClassSlot *m_SourceSlot_OrZeroIfSourceIsActor;
    DesignEqualsImplementationClassSlot *m_DestinationSlot_OrZeroIfNoDest;
    DesignEqualsImplementationClassSignal *m_SignalToEmit;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    QAbstractButton *m_OkButton;
    bool m_SourceIsActor;
    bool m_DestinationIsActor;
    //DesignEqualsImplementationClassSlot *m_SlotWithCurrentContext_OrZeroIfSourceIsActor;
    QList<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*> m_VariablesAvailableToSatisfyArgs;
    QList<QComboBox*> m_AllArgSatisfiers;
    QWidget *m_SignalArgsFillingInWidget;
    QWidget *m_SlotArgsFillingInWidget;

    DesignEqualsImplementationClassLifeLine *m_SourceClassLifeline_OrZeroIfSourceIsActor;
    DesignEqualsImplementationClassLifeLine *m_DestinationClassLifeline_OrZeroIfNoDest;

    bool m_SignalIsExistingSignalFlag;

    void showSignalArgFillingIn(const QString &signalName, const QList<MethodArgumentTypedef> &signalArguments);
    void collapseSignalArgFillingIn();

    void maybeShowSlotArgFillingInUsingAppropriateComboBoxValues();
    void showSlotArgFillingIn(const QString &slotName, const QList<MethodArgumentTypedef> &slotArguments);
    void collapseSlotArgFillingIn();

    bool allArgSatisfiersAreValid();
    //bool allSlotsArgsMatchedUpWithSignalArgsIfSlotEvenChecked();
private slots:
    void handleSignalCheckboxToggled(bool checked);
    void handleSlotCheckboxToggled(bool checked);
    //void handleExistingSignalComboBoxIndexChanged(int newIndex);
    //void handleExistingSlotsComboBoxCurrentIndexChanged(int newIndex);

    void handleSignalsComboBoxResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType);
    void handleSignalsComboBoxSyntaxIsValidChanged(bool syntaxIsValid);
    void handleParsedSignalNameChanged(const QString &parsedSignalName);
    void handleParsedSignalArgumentsChanged(QList<MethodArgumentTypedef> parsedSignalArguments);

    void handleSlotsComboBoxResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType);
    void handleSlotsComboBoxSyntaxIsValidChanged(bool syntaxIsValid);
    void handleParsedSlotNameChanged(const QString &parsedSlotName);
    void handleParsedSlotArgumentsChanged(const QList<MethodArgumentTypedef> &parsedSlotArguments);

    void tryValidatingDialog();

    void handleChooseSourceInstanceButtonClicked();

    bool acceptIfNoSignalsSlotsParsingNeeded_Or_AcceptIfSignalsSlotsParsingSucceeds();
    void handleOkAndMakeChildOfSignalSenderActionTriggered();

    void jitMaybeCreateSignalAndOrSlot();
};

#endif // SIGNALSLOTMESSAGEDIALOG_H
