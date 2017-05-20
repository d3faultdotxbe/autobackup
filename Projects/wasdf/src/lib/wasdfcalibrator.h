﻿#ifndef WASDFCALIBRATOR_H
#define WASDFCALIBRATOR_H

#include <QObject>

#include <QHash>

#include "wasdf.h"

class QTimer;

class WasdfCalibrator : public QObject
{
    Q_OBJECT
public:
    explicit WasdfCalibrator(QObject *parent = 0);
private:
    struct PinNumDetectionAndCalibrationData
    {
        //these two variables are used for pin number detection:
        int PreviousPinPosition = 0;
        long AccumulatedDistanceThePinHasMoved = 0;

        //this struct is used for finger movement range calibration:
        WasdfCalibrationFingerConfiguration PinCalibrationData;
    };

    //void calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
    PinNumDetectionAndCalibrationData getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled();

    QHash<int /*pin number on arduino*/, PinNumDetectionAndCalibrationData> m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId;
    QTimer *m_Timer;
    //Finger m_FingerCurrentlyBeingCalibrated;
    WasdfCalibrationConfiguration m_Calibration;
signals:
    void o(const QString &msg);
    void calibrationComplete(const WasdfCalibrationConfiguration &calibrationConfiguration);
public slots:
    void handleAnalogPinReadingChanged(int pinNumberOnArduino, int newPinPosition);
    void startCalibrating();
private slots:
    void handleTimerTimedOut();
};

#endif // WASDFCALIBRATOR_H
