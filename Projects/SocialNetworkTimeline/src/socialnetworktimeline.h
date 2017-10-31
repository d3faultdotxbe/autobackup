#ifndef SOCIALNETWORKTIMELINE_H
#define SOCIALNETWORKTIMELINE_H

#include <QObject>

class SocialNetworkTimeline : public QObject
{
    Q_OBJECT
public:
    explicit SocialNetworkTimeline(QObject *parent = 0);
signals:
    void e(QString);
    void o(QString);
public slots:
    void initialize();
    void appendJsonObjectToTimeline(const QJsonObject &data);






    //StupidKeyValueContentTracker Comm:  <-- TODOmb: it may be possible to stuff this functionality into a StupidKeyValueContentTrackerComm object (whose stub can be autogenerated by UserInterfaceSkeletonGenerator if I add the code to do it). but just because it's possible doesn't mean it should be done. since I'm unsure, I'll leave it as a TODOmb
signals:
    void initializeRequested();
    void addRequested(const QString &key, const QString & data);
    void modifyRequested(const QString &key, const QString & newValue);
    void removeKeyRequested(const QString &key);
    void commitRequested(const QString & commitMessage);
    void readKeyRequested(const QString &key, const QString & revision);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleInitializeFinished(bool success);
    void handleAddFinished(bool success);
    void handleModifyFinished(bool success);
    void handleRemoveKeyFinished(bool success);
    void handleCommitFinished(bool success);
    void handleReadKeyFinished(bool success, QString key, QString revision, QString data);
};

#endif // SOCIALNETWORKTIMELINE_H
