#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H

#include <QWidget>

#include "designequalsimplementationguicommon.h"

class QTabWidget;

class DesignEqualsImplementationProject;
class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *project, QWidget *parent = 0);
    QTabWidget *classDiagramAndUseCasesTabWidget();
    DesignEqualsImplementationProject *designEqualsImplementationProject();
    void ensureUseCaseTabOpenedAkaCreatedAndMakeCurrentWidget(DesignEqualsImplementationUseCase *useCase);
private:
    QTabWidget *m_ClassDiagramAndUseCasesTabWidget;
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
signals:
    void newUseCaseRequested();
    void editCppModeRequested();
    void mouseModeChanged(DesignEqualsImplementationMouseModeEnum newMouseMode);
public slots:
    void requestEditCppMode();
private slots:
    void requestNewUseCase();
    void handleUseCaseAdded(DesignEqualsImplementationUseCase *useCase);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
