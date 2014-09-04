#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H

#include <QObject>
#include <QHash>

#include "designequalsimplementationproject.h"

class DesignEqualsImplementationClass;
class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationProjectGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectGenerator(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, QObject *parent = 0);
    bool processClassStep0declaringClassInProject(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool processUseCase(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase);
    //bool processClassStep1writingTheFile(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool writeClassesToDisk();

    DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode() const;
    void setProjectGenerationMode(const DesignEqualsImplementationProject::ProjectGenerationMode &projectGenerationMode);
    QString destinationDirectoryPath() const;
    void setDestinationDirectoryPath(const QString &destinationDirectoryPath);
private:
    DesignEqualsImplementationProject::ProjectGenerationMode m_ProjectGenerationMode;
    QString m_DestinationDirectoryPath;

    QHash<DesignEqualsImplementationClass*, QList<QString> /* connect statments in the key class's initialization sequence */> m_ClassesInThisProjectGenerate;

    //designEqualsImplementationUseCase, rootClassLifeline, designEqualsImplementationUseCase->m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsZeroIfNoneConnectedFromActorYet.second
    bool recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, DesignEqualsImplementationClassLifeLine *classLifeline, DesignEqualsImplementationClassSlot *slotToWalk);
    bool writeClassToDisk(DesignEqualsImplementationClass *currentClass);
    void appendConnectStatementToClassInitializationSequence(DesignEqualsImplementationClass *classToGetConnectStatementInInitializationSequence, const QString &connectStatement);


    static inline QString firstCharacterToLower(const QString &inputString)
    {
        QString ret = inputString;
        if(!inputString.isEmpty())
        {
            QString firstChar = ret.at(0);
            QString firstCharToLower = firstChar.toLower();
            ret.replace(0, 1, firstCharToLower);
        }
        return ret;
    }
    static inline QString firstCharacterToUpper(const QString &inputString)
    {
        QString ret = inputString;
        if(!inputString.isEmpty())
        {
            QString firstChar = ret.at(0);
            QString firstCharToUpper = firstChar.toUpper();
            ret.replace(0, 1, firstCharToUpper);
        }
        return ret;
    }
    static inline QString getterNameForProperty(const QString &thePropertyName)
    {
        QString ret = "get" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
    static inline QString setterNameForProperty(const QString &thePropertyName)
    {
        QString ret = "set" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
    static inline QString changedSignalForProperty(const QString &thePropertyName)
    {
        QString ret = firstCharacterToLower(thePropertyName) + "Changed";
        return ret;
    }
    static inline QString memberNameForProperty(const QString &thePropertyName)
    {
        QString ret = "m_" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
signals:
    void e(const QString &msg);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H