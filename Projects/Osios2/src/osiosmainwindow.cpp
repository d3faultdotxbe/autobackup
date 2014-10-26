#include "osiosmainwindow.h"

#include <QTabWidget>

#include "iactivitytab_widget_formainmenutabwidget.h"
#include "mainmenuitems/timelinetab_widget_formainmenutabwidget.h"
#include "mainmenuitems/writertab_widget_formainmenutabwidget.h"

#include "timelinenodetypes/mainmenuactivitychangedtimelinenode.h"

OsiosMainWindow::OsiosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_MainMenuItemsTabWidget(new QTabWidget())
{
    setWindowTitle(tr("Osios"));

    //all added tabs must implement IMainMenuActivityTab
    m_MainMenuItemsTabWidget->addTab(new TimelineTab_Widget_ForMainMenuTabWidget(), tr("Timeline"));
    m_MainMenuItemsTabWidget->addTab(new WriterTab_Widget_ForMainMenuTabWidget(), tr("Writer"));

    setCentralWidget(m_MainMenuItemsTabWidget);

    connect(m_MainMenuItemsTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleMainMenuItemsTabWidgetCurrentTabChanged()));
}
void OsiosMainWindow::handleMainMenuItemsTabWidgetCurrentTabChanged()
{
    const MainMenuActivityChangedTimelineNode mainMenuActivityChangedTimelineNode(static_cast<IActivityTab_Widget_ForMainMenuTabWidget*>(m_MainMenuItemsTabWidget->currentWidget())->mainMenuActivityType());
    emit actionOccurred(static_cast<const ITimelineNode &>(mainMenuActivityChangedTimelineNode));
}
