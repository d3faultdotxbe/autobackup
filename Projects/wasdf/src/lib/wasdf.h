#ifndef WASDF_H
#define WASDF_H

#include <QObject>

#include <QHash>
#include <QPointer>

class WasdfArduino;
class WasdfCalibrator;

enum class Finger
{
    LeftPinky_Finger0 = 0
  , Finger0_LeftPinky = LeftPinky_Finger0

  , LeftRing_Finger1 = 1
  , Finger1_LeftRing = LeftRing_Finger1

  , LeftMiddle_Finger2 = 2
  , Finger2_LeftMiddle = LeftMiddle_Finger2

  , LeftIndex_Finger3 = 3
  , Finger3_LeftIndex = LeftIndex_Finger3

  , LeftThumb_Finger4 = 4
  , Finger4_LeftThumb = LeftThumb_Finger4

  , RightThumb_Finger5 = 5
  , Finger5_RightThumb = RightThumb_Finger5

  , RightIndex_Finger6 = 6
  , Finger6_RightIndex = RightIndex_Finger6

  , RightMiddle_Finger7 = 7
  , Finger7_RightMiddle = RightMiddle_Finger7

  , RightRing_Finger8 = 8
  , Finger8_RightRing = RightRing_Finger8

  , RightPinky_Finger9 = 9
  , Finger9_RightPinky = RightPinky_Finger9
};
QString fingerEnumToHumanReadableString(Finger finger);
inline uint qHash(const Finger &key, uint seed)
{
    return qHash(static_cast<uint>(key), seed);
}
struct WasdfCalibrationFingerConfiguration
{
    int AnalogPinIdOnArduino = -1;
    int MinValue = 1023;
    int MaxValue = 0;
};

typedef QHash<Finger, WasdfCalibrationFingerConfiguration> WasdfCalibrationConfiguration;

class Wasdf : public QObject
{
    Q_OBJECT
public:
    explicit Wasdf(QObject *parent = 0);
private:
    void startWasdfActualSinceCalibrated();

    WasdfArduino *m_Arduino;
    QPointer<WasdfCalibrator> m_Calibrator;
    WasdfCalibrationConfiguration m_Calibration;
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void wasdfFinished(bool success);
public slots:
    void startWasdf();
private slots:
    void handleCalibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration);
    void handleFingerMoved(Finger finger, int newPosition);
};

#endif // WASDF_H
