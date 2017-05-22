#include "wasdf.h"

#include <QCoreApplication>
#include <QSettings>

#include "wasdfarduino.h"
#include "wasdfcalibrator.h"
#include "wasdfcalibrationconfigurationsettingsreaderwriter.h"

#define SETTINGS_KEY_IS_CALIBRATED "isCalibrated"

Wasdf::Wasdf(QObject *parent)
    : QObject(parent)
    , m_Arduino(new WasdfArduino(this))
{
    QCoreApplication::setOrganizationName("wasdf organization");
    QCoreApplication::setOrganizationDomain("wasdf.com");
    QCoreApplication::setApplicationName("wasdf");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    connect(m_Arduino, &WasdfArduino::e, this, &Wasdf::e);
}
void Wasdf::startWasdfActualSinceCalibrated()
{
    connect(m_Arduino, &WasdfArduino::fingerMoved, this, &Wasdf::handleFingerMoved);
    m_Arduino->start(m_Calibration); //TODOoptimization: once the AtRestPosition or AtRestMinValue/AtRestMinValue stuff is implemented, it would be best if the arduino was told of those values and DIDN'T WRITE TO SERIAL whenever a finger was considered "at rest"... as opposed to us filtering that out on the PC side. the quieter we keep the serial line, the less chance of corruption/errors
}
void Wasdf::startWasdf()
{
    QSettings settings;
    bool isCalibrated = settings.value(SETTINGS_KEY_IS_CALIBRATED, false).toBool();
    if(!isCalibrated)
    {
        m_Arduino->startInCalibrationMode();
        m_Calibrator = new WasdfCalibrator(this);

        //TODOreq: wtf for some reason this doesn't work with qt5-style syntax, which is weird because the WasdfArduino::e signal connects just fine to Wasdf::e signal. I also confirmed that the QPointer isn't the problem
        //connect(m_Calibrator.data(), &WasdfCalibrator::o, this &Wasdf::o);
        connect(m_Calibrator.data(), SIGNAL(o(QString)), this, SIGNAL(o(QString))); //qt4-style connect for now

        connect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        connect(m_Calibrator.data(), &WasdfCalibrator::calibrationComplete, this, &Wasdf::handleCalibrationComplete);
        m_Calibrator->startCalibrating();
    }
    else
    {
        WasdfCalibrationConfigurationSettingsReaderWriter::readFromSettings(settings, &m_Calibration);
        emit o("Calibration read from settings");
        startWasdfActualSinceCalibrated();
    }
}
void Wasdf::handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration)
{
    m_Calibration = calibrationConfiguration;
    QSettings settings;
    WasdfCalibrationConfigurationSettingsReaderWriter::writeToSettings(settings, calibrationConfiguration);
    if(!m_Calibrator.isNull())
    {
        disconnect(m_Arduino, &WasdfArduino::analogPinReadingChangedDuringCalibration, m_Calibrator.data(), &WasdfCalibrator::handleAnalogPinReadingChanged);
        m_Calibrator->deleteLater();
    }
    startWasdfActualSinceCalibrated();
}
void Wasdf::handleFingerMoved(Finger finger, int newPosition)
{
    emit o("Finger '" + fingerEnumToHumanReadableString(finger) + "' moved to position: " + QString::number(newPosition));


    //TODOreq: this is only here for testing
    static int numMovementsReceived = 0;
    ++numMovementsReceived;
    if(numMovementsReceived == 2000)
        emit wasdfFinished(true);
}
QString fingerEnumToHumanReadableString(Finger finger)
{
    //optimized for most commonly used fingers (middle fingers vs thumbs is a tie if you ask me xD. I'd say thumb beats middle for DAILY (normal non-wasdf) USE, but this isn't going to be daily use now is it? ex: when using your mouse you use your middle finger to right click a lot)
    switch(finger)
    {
        case Finger::Finger6_RightIndex:
            return "right index";
        break;
        case Finger::Finger3_LeftIndex:
            return "left index";
        break;
        case Finger::Finger7_RightMiddle:
            return "right middle";
        break;
        case Finger::Finger2_LeftMiddle:
            return "left middle";
        break;
        case Finger::Finger5_RightThumb:
            return "right thumb";
        break;
        case Finger::Finger4_LeftThumb:
            return "left thumb";
        break;
        case Finger::Finger8_RightRing:
            return "right ring";
        break;
        case Finger::Finger1_LeftRing:
            return "left ring";
        break;
        case Finger::Finger9_RightPinky:
            return "right pinky";
        break;
        case Finger::Finger0_LeftPinky:
            return "left pinky"; //rip left pinky, nobody loves/uses him (jk I hope to)
        break;
    }
    return "#error finger#"; //should (will) never get here
}