#include "wasdfarduino.h"

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QHashIterator>
#include <QDebug>

#include "wasdf.h"

//so now for the protocol. at least [so far (and I intend to keep it that way!)] the protocol is relatively simple. I need to also keep in mind that when the Arduino is in mouse/keyboard mode that it doesn't use the Serial.write shit (it could, but then I'd need a priveledged keyboard-press/mouse-movement-n-click daemon-thing on the PC side, and those daemon-things are OS specific!)

//Modes
//1) Calibrating
//2) Report finger movements verbosely over serial
//3) Keyboard/Mouse

//are (2) and (3) mutually exclusive? idfk tbh so I'll try to make it so they can both be used simultaneously (I seem to recall a fantasy where I'm coding and making music at the same time? in fact that is part of my 'perfect moment' (goal) in life so yea they are not/shouldn't be mutually exclusive (unless I'm just toggling modes ultra fast in that fantasy. since the fantasy hasn't happened yet I'm free to define it however I please)

//hmm json, binary, csv(s/comma/colon/newline/etc), hmm. time to research what kind of serialization arduino comes with, since it's libraries are way more limited than the PC sides of things
//I want to use json but I don't see any built-in libs and I'm sure as shit not about to write a parser. there are very likely lightweight self-contained pure c/c++ libraries I could just #include, but I'm pretty sure this arduino micro has.... a micro amount of space available. mb just 8kb compiled? *checks*. oh nice 28kb, that should be enough for anyone (ziiiiiing)
//meh after researching a decent sounding MIT-licensed ArduionJson library... and thinking about it more in the process... I don't think my protocol will be so complex to warrant json. I'll keep json in mind in case things start to get complex, but meh just a plaintext+colon+newlines protocol (like in MusicFingers, though obviously more robust since calibration mode must exist) sounds sufficient. I'll think about it more while I eat dinner (clam chowda)
//another way I could do things is to have 1 sketch for calibration and 1 sketch for normal operation, that would both give me more room to code the sketches and it would get dead (calibration) code paths out of normal operation
//TODOreq: the arduino could persist calibration/configuration to it's EEPROM, which would allow users to take their arduino around to various computers and use them without needing to recalibrate. The protocol would need to be more robust (we first query the arduino to see if it has a stored conf), but it wouldn't be too difficult to implement
//TODOreq: I have a choice of simply telling the arduino "hey we want values for these 10 pins" and then keeping the pin-to-fingers map local to the PC, or I can say "hey we want values for these 10 pins which correspond to these fingers". It makes very little difference AT FIRST (esp since both are C++ environments I recently found out (I thought arduino was C only xD)), but considering the t0d0r3q directly above this one (persisting calibration to eeprom), I should then go with the second approach of sending more of the information to the arduino. so once started, I should receive messages like this across serial: "finger0 moved to position 69" (compared to "analog pin 18 moved to position 420"). I do/will have a copy of that pin-to-fingers map on the PC side of course, but I no longer need it once we're up and running (and I suppose if we read calibration config from eeprom, the PC never even needs to know it TODOreq: make sure to never use it then if that's the case (might be safer just to read it just in case we accidentally use it)!!!) <-- disregard that actually we do want to use the calibration on the PC side, we want to constrain (map, actually (maybe both)) the reported sensor values to the determined calibrated min/max values
WasdfArduino::WasdfArduino(QObject *parent)
    : QObject(parent)
    , m_SerialPort(new QSerialPort("/dev/ttyACM0", this)) //TODOreq: be able to choose different serial ports at runtime, MusicFingers has some of this implemented (using ugly defines kek) and some decent ideas such as "use the _one_ containing the string 'arduino'" etc
    , m_SerialPortTextStream(m_SerialPort)
{
    m_SerialPort->setBaudRate(QSerialPort::Baud38400);
}
void WasdfArduino::openSerialPortIfNotOpen()
{
    if(!m_SerialPort->isOpen())
    {
        if(!m_SerialPort->open(QIODevice::ReadWrite /*QIODevice::Text <-- apparently not supported, but I guess it's text by default? <- nope it's binary but eh ascii isn't so hard to work with anyways. my protocol will be pure ascii*/))
        {
            emit e("failed to open serial port \"" + m_SerialPort->portName() + "\" for reading/writing");
            //TODOreq: error out, return false or emit finished(false) etc
        }
    }
}
void WasdfArduino::sendCommandToArduino(const QString &commandToSendToArduino)
{
    Q_ASSERT(commandToSendToArduino.length() < (1024)); //TODOreq: 1024 should be in a shared header file between sketch and this. the arduino calls String.reserve(1024) in setup()
    m_SerialPortTextStream << commandToSendToArduino << "\n"; //don't use endl, that uses \r\n sometimes I think but might be wrong since the QIODevice isn't using QIODevice::Text? fuggit
    m_SerialPortTextStream.flush();
}
void WasdfArduino::startInCalibrationMode()
{
    //in calibration mode, arduino sends _ALL_ (not just 10) analog pin readings over serial (TODOmb: user can pass an --exclude-analog-pins flag in case they're using the other analog pins for something else (but then they'd need to modify the sketch anyways, so maybe this isn't necessary?)

    disconnect(m_SerialPort, SIGNAL(readyRead())); //TODOreq: for some reason this fails with qt5-style pmf syntax. maybe a newer version of Qt doesn't (otherwise file a bugreport)
    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadCalibrationMode);

    openSerialPortIfNotOpen();

    sendCommandToArduino("calibrate"); //TODOreq: these strings should be in a shared header, shared between the arduino sketch and this sauce
}
void WasdfArduino::start(const WasdfCalibrationConfiguration &calibrationConfig)
{
    //this is where the 10 fingers get mapped to the 10 analog pins

    disconnect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadCalibrationMode);
    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadNormalFingerMovementMode);

    openSerialPortIfNotOpen();

    QHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(calibrationConfig);
    QString startCommand("startWithPinToFingerMap:");
    bool first = true;
    while(it.hasNext())
    {
        it.next();
        if(!first)
            startCommand.append(";"); //TODOreq: semi-colon delim in shared header
        first = false;
        //startCommand.append(static_cast<int>(it.key())); //we know that the fingers are sent 'in order' from 0-9, so we don't need to send them
        //startCommand.append(",");
        startCommand.append(it.value().AnalogPinIdOnArduino);
        startCommand.append(","); //TODOreq: comma delim in shared header
        startCommand.append(it.value().MinValue);
        startCommand.append(",");
        startCommand.append(it.value().MaxValue);
    }

    //example start command:
    //startWithPinToFingerMap:18,0,1023;19,1,1022;28,50,950;...
    //18 = analog pin id
    //0 = min value
    //1023 = max value
    //repeat for each finger, separated by semicolon
    //note: the analog pins can be sent in any order. the semicolons separate each finger and are in order of finger id (0-9)

    //TODOreq: on arduino side we do splitByColon then splitBySemiColon then splitByComma

    sendCommandToArduino(startCommand); //TODOreq: because serial is a piece of shit it sometimes gives us corrupt data. the arduino should be able to request a re-send of a command (and should maybe use a "SYNC" magic header thing just before that command, because who the fuck knows where we are) if a received command (not just start command) is mal-formed. however on the arduino->pc side of things it's ok if we just silently drop malformed messages (we receive finger movement messages at such a high rate that a few malformed ones is no biggy). still, parity and/or checksums for sending BOTH ways would be nice. TODOoptimization: if I send "checksumm'd messages" from arduino to pc, I should probably combine all 10 finger sensor readings into a single message, otherwise the overhead of messages might take up a large proportion of the bandwidth
    //TODOmb: for arduino->pc (or maybe both ways). moving to a very simple (not checksum'd) BEGINSYNC/ENDSYNC (with the colon separated message in between those 2 SYNCs) would probably be a good enough error detection (and more importantly, error ignoring) strategy
    //TODOprobably ------ JSON JSON JSON ------ checksum'd JSON (the checksum is not IN the json, but the checksum 'surrounds'/validates the JSON). if this protocol gets complex AT ALL (and the above comments indicate it might), JSON JSON JSON JSON
}
void WasdfArduino::handleSerialPortReadyReadCalibrationMode()
{
    while(m_SerialPort->canReadLine())
    {
        QString line = m_SerialPortTextStream.readLine();
        //line is simply: "[pinId]:[sensorValue]"
        QStringList lineSplitAtColon = line.split(":");
        if(lineSplitAtColon.size() != 2)
        {
            qDebug() << "line didn't split evenly into 2 parts:" << line; //TODOmb: use emit v() for verbose or some such instead? in general I don't like to use qDebug/etc in my proper apps
            continue;
        }
        bool convertOk;
        int analogPinId = lineSplitAtColon.first().toInt(&convertOk);
        if(!convertOk)
        {
            qDebug() << "lhs of colon didn't convert to int:" << line;
            continue;
        }
        //TODOmb: some error checking of the analog pin id. eg can't be less than 0 (or maybe it can? I have no idea what arduino's rules are on this), but [during calibration (TODOreq: after calibration we know EXACTLY what 'finger ids' we want, so we can filter out everything else)] I'm not sure what a good upper max would be (unless I read them out of arduino headers)
        int sensorValue = lineSplitAtColon.at(1).toInt(&convertOk);
        if(!convertOk)
        {
            qDebug() << "rhs of colon didn't convert to int:" << line;
            continue;
        }
        if(sensorValue < 0 || sensorValue > 1023)
        {
            qDebug() << "sensor value on rhs of colon was out of bounds:" << line;
            continue;
        }
        emit analogPinReadingChangedDuringCalibration(analogPinId, sensorValue);
    }
}
void WasdfArduino::handleSerialPortReadyReadNormalFingerMovementMode()
{
    //TODOreq:
}