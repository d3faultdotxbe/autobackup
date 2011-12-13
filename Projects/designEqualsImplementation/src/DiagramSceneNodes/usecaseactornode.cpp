#include "usecaseactornode.h"

UseCaseActorNode::UseCaseActorNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString UseCaseActorNode::getNodeTypeAsString()
{
    return QString("Actor");
}
void UseCaseActorNode::drawMyPolygon()
{
    m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100)
                          << QPointF(100, 0) << QPointF(0, -100)
                          << QPointF(-100, 0);
}
#if 0
DiagramSceneNode *UseCaseActorNode::cloneSelf()
{
    DiagramSceneNode *clone = new UseCaseActorNode(m_UniqueId, m_ViewType);
    return clone;
}
#endif
