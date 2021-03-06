#include "mouseormotionormysexyfaceviewmakergui.h"

#include <QCoreApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QMessageBox>

#ifdef MOUSE_OR_MOTION_OR_MY_SEXY_FACE_VIEW_MAKER_USE_BACKEND_THREAD
#include "objectonthreadgroup.h"
#endif

#define MouseOrMotionOrMySexyFaceViewMakerGui_OVERRIDE_QT_BLINKING_CURSOR_COLOR_ARG "--qt-blinking-cursor-color"

MouseOrMotionOrMySexyFaceViewMakerGui::MouseOrMotionOrMySexyFaceViewMakerGui(QObject *parent)
    : QObject(parent)
    , m_Gui(new MouseOrMotionOrMySexyFaceViewMakerWidget(m_ViewSize))
    , m_OptionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied(-1)
    //, m_SystemTrayIconMenu(0)
{
#ifdef MOUSE_OR_MOTION_OR_MY_SEXY_FACE_VIEW_MAKER_USE_BACKEND_THREAD
    ObjectOnThreadGroup *backendThread = new ObjectOnThreadGroup(this);
    backendThread->addObjectOnThread<MouseOrMotionOrMySexyFaceViewMaker>("handleMouseAndOrMotionViewMakerReadyForConnections");
    backendThread->doneAddingObjectsOnThreads();
#else
    MouseOrMotionOrMySexyFaceViewMaker *mouseOrMotionOrMySexyFaceViewMaker = new MouseOrMotionOrMySexyFaceViewMaker(this);
#endif

    QStringList arguments = QCoreApplication::arguments();

    //optional arguments parsing (flags)
    int indexOfRequirePrimaryScreenWidth = arguments.indexOf("--require-primary-screen-width");
    bool convertOk;
    if(indexOfRequirePrimaryScreenWidth > -1)
    {
        arguments.removeAt(indexOfRequirePrimaryScreenWidth);
        if(arguments.size() <= indexOfRequirePrimaryScreenWidth)
        {
            qDebug("--require-primary-screen-width must be followed by a value");
            handleQuitRequested();
            return;
        }
        m_OptionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied = arguments.takeAt(indexOfRequirePrimaryScreenWidth).toInt(&convertOk);
        if(!convertOk)
        {
            qDebug("primary screen width was not valid int");
            handleQuitRequested();
            return;
        }
    }

    QString qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack;
    int qtBlinkingCursorColorIndex = arguments.indexOf(MouseOrMotionOrMySexyFaceViewMakerGui_OVERRIDE_QT_BLINKING_CURSOR_COLOR_ARG);
    if(qtBlinkingCursorColorIndex > -1)
    {
        arguments.removeAt(qtBlinkingCursorColorIndex);
        if(arguments.size() <= qtBlinkingCursorColorIndex)
        {
            qDebug(MouseOrMotionOrMySexyFaceViewMakerGui_OVERRIDE_QT_BLINKING_CURSOR_COLOR_ARG " must be followed by a value");
            handleQuitRequested();
            return;
        }
        qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack = arguments.takeAt(qtBlinkingCursorColorIndex);
        if(qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack.toLower() == "d3fault")
            qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack = "#d5d5d5"; //heh
    }

    //optional arguments parsing (ordered)
    m_ViewSize.setWidth(800);
    int argIndex = 0;
    if(arguments.size() > ++argIndex)
    {
        int viewWidth = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && viewWidth > 0)
        {
            m_ViewSize.setWidth(viewWidth);
        }
    }
    m_ViewSize.setHeight(600);
    if(arguments.size() > ++argIndex)
    {
        int viewHeight = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && viewHeight > 0)
        {
            m_ViewSize.setHeight(viewHeight);
        }
    }
    m_CaptureFps = 15;
    if(arguments.size() > ++argIndex)
    {
        int captureFps = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && captureFps > 0)
        {
            m_CaptureFps = qMin(captureFps, 1000); //1000 fps max (since QTimer uses millseconds)
        }
    }
    m_MotionDetectionFps = qMin(5, m_CaptureFps);
    if(arguments.size() > ++argIndex)
    {
        int motionDetectionFps = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && motionDetectionFps > 0)
        {
            m_MotionDetectionFps = qMin(motionDetectionFps, m_CaptureFps);
        }
    }
    m_BottomPixelRowsToIgnore = 25;
    if(arguments.size() > ++argIndex)
    {
        int bottomPixelRowsToIgnore = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && bottomPixelRowsToIgnore > -1) //&& < resolutionHeight
        {
            m_BottomPixelRowsToIgnore = bottomPixelRowsToIgnore;
        }
    }
    m_CameraDeviceName = "/dev/video0";
    if(arguments.size() > ++argIndex)
    {
        QString cameraDeviceNameString = arguments.at(argIndex).trimmed();
        if(!cameraDeviceNameString.isEmpty())
            m_CameraDeviceName = cameraDeviceNameString.toUtf8();

    }
    m_CameraResolution.setWidth(720);
    if(arguments.size() > ++argIndex)
    {
        int cameraResolutionWidth = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && cameraResolutionWidth > 0)
        {
            m_CameraResolution.setWidth(cameraResolutionWidth);
        }
    }
    m_CameraResolution.setHeight(480);
    //m_CameraResolution.setHeight(452); //derp damn bug was funnily hard to hunt down xD
    if(arguments.size() > ++argIndex)
    {
        int cameraResolutionHeight = arguments.at(argIndex).toInt(&convertOk);
        if(convertOk && cameraResolutionHeight > 0)
        {
            m_CameraResolution.setHeight(cameraResolutionHeight);
        }
    }
    //TODOoptional: could do drawn cursor image path, whether or not to save the image as video, etc...

#ifndef MOUSE_OR_MOTION_OR_MY_SEXY_FACE_VIEW_MAKER_USE_BACKEND_THREAD
    handleMouseAndOrMotionViewMakerReadyForConnections(mouseOrMotionOrMySexyFaceViewMaker);
#endif
}
void MouseOrMotionOrMySexyFaceViewMakerGui::handleMouseAndOrMotionViewMakerReadyForConnections(QObject *mouseOrMotionOrMySexyFaceViewMakerAsQObject)
{
    MouseOrMotionOrMySexyFaceViewMaker *business = static_cast<MouseOrMotionOrMySexyFaceViewMaker*>(mouseOrMotionOrMySexyFaceViewMakerAsQObject);
    connect(business, SIGNAL(presentPixmapForViewingRequested(QPixmap)), m_Gui.data(), SLOT(presentPixmapForViewing(QPixmap)));
    connect(business, SIGNAL(detectedMySexyFaceStreamIsFrozen()), m_Gui.data(), SLOT(handleMySexyFaceStreamIsFrozen()));
    connect(m_Gui.data(), SIGNAL(setMouseOrMotionOrMySexyFaceViewModeRequested()), business, SLOT(setMouseOrMotionOrMySexyFaceViewMode()));
    connect(m_Gui.data(), SIGNAL(setMySexyFaceViewModeRequested()), business, SLOT(setMySexyFaceViewMode()));
    connect(business, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(business, SIGNAL(quitRequested()), this, SLOT(handleQuitRequested()));

    m_Gui->show();

#if 0
    QIcon systemTrayIconGraphic(":/systemTrayIcon.svg");
    QSystemTrayIcon *systemTrayIcon = new QSystemTrayIcon(systemTrayIconGraphic, this);
    m_SystemTrayIconMenu = new QMenu();

    m_SystemTrayIconMenu->addSection(tr("View Mode"));
    QActionGroup *viewModeActionGroup = new QActionGroup(m_SystemTrayIconMenu);
    QAction *autoViewModeAction = new QAction(tr("Auto (Mouse or Motion or My Sexy Face)"), viewModeActionGroup);
    autoViewModeAction->setCheckable(true);
    autoViewModeAction->setChecked(true);
    connect(autoViewModeAction, SIGNAL(triggered(bool)), business, SLOT(setMouseOrMotionOrMySexyFaceViewMode(bool)));
#if 0 //lazy and I really only need the "force my sexy face" mode atm
    QAction *lastMouseOrLastMouseModeAction = new QAction(tr("TODO: Last Mouse or Last Motion"), viewModeActionGroup); //TODOoptional: split these into more options
    lastMouseOrLastMouseModeAction->setCheckable(true);
    connect(lastMouseOrLastMouseModeAction, SIGNAL(triggered(bool)), business, SLOT(setMouseOrLastMouseViewMode(bool)));
#endif
    QAction *mySexyFaceMode = new QAction(tr("My Sexy Face"), viewModeActionGroup);
    mySexyFaceMode->setCheckable(true);
    connect(mySexyFaceMode, SIGNAL(triggered(bool)), business, SLOT(setMySexyFaceViewMode(bool)));
    viewModeActionGroup->setExclusive(true);
    m_SystemTrayIconMenu->addActions(viewModeActionGroup->actions());

    m_SystemTrayIconMenu->addSeparator();
    QAction *quitAction = new QAction(tr("&Quit"), m_SystemTrayIconMenu);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    m_SystemTrayIconMenu->addAction(quitAction);
    systemTrayIcon->setContextMenu(m_SystemTrayIconMenu);
    systemTrayIcon->setVisible(true);
#endif

    QMetaObject::invokeMethod(business, "startMakingMouseOrMotionOrMySexyFaceViews", Qt::QueuedConnection, Q_ARG(QSize, m_ViewSize), Q_ARG(int, m_CaptureFps), Q_ARG(int, m_MotionDetectionFps), Q_ARG(int, m_BottomPixelRowsToIgnore), Q_ARG(QString, m_CameraDeviceName), Q_ARG(QSize, m_CameraResolution), Q_ARG(int, m_OptionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied));
}
void MouseOrMotionOrMySexyFaceViewMakerGui::handleE(const QString &msg)
{
    QMessageBox::critical(0, tr("Error"), msg);
}
void MouseOrMotionOrMySexyFaceViewMakerGui::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
