#include "wasdfcalibrator.h"

#include <QTimer>
#include <QHashIterator>

#include "wasdf.h"

//Calibrator or Calibrater? fuggit~
//need to write this somewhere to remember/understand it better: DURING calibration there's information pertaining to every pin (>= 10) on the arduino, but ONCE CALIBRATED we only want to emit/deliver information pertaining to exactly 10 pins. it's our job as the calibrator to figure out which 10. it is (was) slightly confusing that there are 2 sets of "configutations"
//TODOreq: "at rest" detection/calibration, which should be a RANGE of motion where we ignore. it needn't be hugging the extremeties (0 or 1023), can be right smack in the middle. that range-size should initially be a hardcoded percentage (10%?) of their full 0-1023 utilization, but longer term would be nice if they could specify their own range-size during the calibration phase
//^"at rest" detection could be done after all 10 fingers are calibrated ("ok now put your fingers at rest and keep them still for 5 seconds"), or (ideally) we could do it on the fly while doing the initial 10 finger calibration (the less steps to calibration, the better). to do it "on the fly" requires careful logic: I should detect the "at rest" point (which the at rest 'range' is based around) for a finger on my right hand while a finger on my left hand is being pin-detected-and-full-movement-range-calibrated (aka normal calibration). the reason for this is that fingers on the same hand tend to bend alongside other ones (whereas fingers on the opposite don't). an example flow: while calibrating left index finger, do at rest detection of right index finger; while calibrating right middle finger, do at rest detection of left index finger, so on and so forth. I'm not quite sure when the at rest detection of the very last finger (left pinky) should be done, but there's bound to be sometime to do it eventually~. maybe during the "ok now choose which finger you want as your ok/next/continue/yes finger?" stage (which, if it even exists (right index finger for now)) follows immediately after the calibration stage.
WasdfCalibrator::WasdfCalibrator(QObject *parent)
    : QObject(parent)
    , m_Timer(new QTimer(this))
{
    m_Timer->setInterval(5000); //TODOmb: visualize the countdown second by second
    m_Timer->setSingleShot(true);
    connect(m_Timer, &QTimer::timeout, this, &WasdfCalibrator::handleTimerTimedOut);
}
WasdfCalibrator::PinNumDetectionAndCalibrationData WasdfCalibrator::getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled()
{
    PinNumDetectionAndCalibrationData currentPinDataWinning;
    QHashIterator<int /*pin number on arduino*/, PinNumDetectionAndCalibrationData> it(m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId);
    while(it.hasNext())
    {
        it.next();
        if(it.value().AccumulatedDistanceThePinHasMoved > currentPinDataWinning.AccumulatedDistanceThePinHasMoved)
        {
            currentPinDataWinning = it.value();
        }
    }
    //TODOreq: error checking? what if all pins didn't move at all for example, would likely mean the arduino is wired wrong
    return currentPinDataWinning;
}
void WasdfCalibrator::calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger()
{
    if(m_OrderedFingerIterator.hasNext())
        calibrateFingerAndThenCallCalibrateNextFingerAgain(m_OrderedFingerIterator.next());
    else
        emit calibrationComplete(m_Calibration);
}
void WasdfCalibrator::calibrateFingerAndThenCallCalibrateNextFingerAgain(Finger fingerToCalibrate)
{
    m_FingerCurrentlyBeingCalibrated = fingerToCalibrate;
    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.clear();
    emit o("Move your right index finger back and forth as much as you can and as far as you can for a few seconds."); //TODOreq: the wording here sucks, the user will be confused: "aren't as far and as much the same?" an animated .gif explaining it to the user would be best, but even that gif needs to have carefully selected wording xD
    m_Timer->start();
}
void WasdfCalibrator::handleAnalogPinReadingChanged(int pinNumberOnArduino, int newPinPosition)
{
    PinNumDetectionAndCalibrationData currentPinData = m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.value(pinNumberOnArduino, PinNumDetectionAndCalibrationData());

    //this line of code is to facilitate analog pin number detection:
    currentPinData.AccumulatedDistanceThePinHasMoved += static_cast<long>(qAbs(currentPinData.PreviousPinPosition - newPinPosition));

    //these next few lines of code are for finger movement range calibration:
    currentPinData.PinCalibrationData.AnalogPinIdOnArduino = pinNumberOnArduino;
    currentPinData.PinCalibrationData.MaxValue = qMax(currentPinData.PinCalibrationData.MaxValue, newPinPosition);
    currentPinData.PinCalibrationData.MinValue = qMin(currentPinData.PinCalibrationData.MinValue, newPinPosition);

    m_AccumulatedDistancesEachAnalogPinMoved_ByAnalogPinId.insert(pinNumberOnArduino, currentPinData);
}
void WasdfCalibrator::startCalibrating()
{
    m_OrderedFingerIterator.reset(); //not really necessary unless they call startCalibrating again at a later date (RE-calibration)
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
void WasdfCalibrator::handleTimerTimedOut()
{
    //pseudo: m_FingerCurrentlyBeingCalibrated = m_FingerThatMovedTheMostTotalDistance;

    PinNumDetectionAndCalibrationData pinCalibrationDataOfRightIndexFinger = getPinCalibrationDataWithTheFurthestAccumulatedDistanceTraveled();
    m_Calibration.insert(m_FingerCurrentlyBeingCalibrated, pinCalibrationDataOfRightIndexFinger.PinCalibrationData);
    emit o("We've determined that your " + fingerEnumToHumanReadableString(m_FingerCurrentlyBeingCalibrated) + " finger is connected to arduino's analog pin #" + QString::number(pinCalibrationDataOfRightIndexFinger.PinCalibrationData.AnalogPinIdOnArduino));
    calibrateNextFingerAndEmitCalibrationCompleteAfterLastFinger();
}
