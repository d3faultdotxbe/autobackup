#ifndef DESIGNEQUALSIMPLEMENTATIONGUICOMMON_H
#define DESIGNEQUALSIMPLEMENTATIONGUICOMMON_H

#define DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS 0
#define DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT 3
#define DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS 5

#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH 7
#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE 50

#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_LINE_WIDTH_MARGIN_AROUND_SIGNAL_NAME_TEXT 5

#define DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID (QGraphicsItem::UserType)
#define DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID (QGraphicsItem::UserType+1)
#define DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID (QGraphicsItem::UserType+2)
#define DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID (QGraphicsItem::UserType+3)

#include <QMetaType>
enum DesignEqualsImplementationMouseModeEnum { DesignEqualsImplementationMouseMoveMode, DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode };
Q_DECLARE_METATYPE(DesignEqualsImplementationMouseModeEnum)

#define DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled)

#define DESIGN_EQUALS_IMPLEMENTATION_ACTOR_STRING "Actor"

#define DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_PREFIX "uml/design-equals-implementation-"

//Anything that can be dropped into class diagram (used as mime TYPE)
#define DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT \
    DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_PREFIX "class-diagram-object"
#define DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT \
    DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_PREFIX "use-case-object"

#endif // DESIGNEQUALSIMPLEMENTATIONGUICOMMON_H
