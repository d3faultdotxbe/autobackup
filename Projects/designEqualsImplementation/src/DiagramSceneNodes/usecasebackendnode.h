#ifndef USECASEBACKENDNODE_H
#define USECASEBACKENDNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseBackEndNode : public DiagramSceneNode
{
public:
    UseCaseBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
    virtual DesignProjectTemplates::DesignProjectViewNodeType getNodeType();
    virtual void drawMyPolygon();
};

#endif // USECASEBACKENDNODE_H
