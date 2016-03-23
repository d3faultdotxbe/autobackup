#ifndef DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTSGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTSGRAPHICSITEMFORUSECASESCENE_H

#include <QObject>
#include <QGraphicsRectItem>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationChunkOfRawCppStatements;
class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene;

class DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene(DesignEqualsImplementationChunkOfRawCppStatements *chunkOfRawCppStatements, DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parentSlotGraphicsItem, QObject *parent = 0);
    virtual ~DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene() { }
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:
    DesignEqualsImplementationChunkOfRawCppStatements *m_ChunkOfRawCppStatements;
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *m_ParentSlotGraphicsItem;
    QGraphicsTextItem *m_TextGraphicsItem;
signals:
    void editCppModeRequested(Type *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, int statementIndexOfSlot);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCHUNKOFRAWCPPSTATEMENTSGRAPHICSITEMFORUSECASESCENE_H
