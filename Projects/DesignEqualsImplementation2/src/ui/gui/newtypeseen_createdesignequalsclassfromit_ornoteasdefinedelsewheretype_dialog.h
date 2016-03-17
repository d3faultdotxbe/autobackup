#ifndef NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
#define NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H

#include <QDialog>

class QGridLayout;

class Type;
class DesignEqualsImplementationProject;

class NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfTypesToDecideOn, DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QList<QString> typesDecidedToBeDesignEqualsImplementationClass() const;
    QList<QString> typesDecidedToBeDefinedElsewhere() const;
    QList<Type*> newTypesSeen() const;
private:
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    QGridLayout *m_NewTypesGridLayout;

    QList<Type*> m_NewTypesSeen;
    QList<QString> buildTypesListDependingOnWhetherCreateClassIsChecked(bool checked) const;
private slots:
    void handleDialogAccepted();
};

#endif // NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
