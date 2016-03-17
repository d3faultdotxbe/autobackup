#ifndef IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
#define IDESIGNEQUALSIMPLEMENTATIONMETHOD_H

#include <QString>
#include <QList>
#include <QPair>

#include "type.h"
#include "designequalsimplementationclassmethodargument.h"

typedef QPair<QString /*type*/, QString /*name*/> MethodArgumentTypedef; //derp defined like twenty different places...

class DesignEqualsImplementationClass;

class IDesignEqualsImplementationMethod
{
public:
    enum MethodSignatureFlagsEnum
    {
        MethodSignatureForVisualAppearanceContainsArgumentVariableNames
        , MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames
    };

    DesignEqualsImplementationClassMethodArgument *createNewArgument(Type *argumentType, const QString &argumentVariableName);

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClassMethodArgument*> arguments() const;
    QList<MethodArgumentTypedef> argumentsAsMethodArgumentTypedefList() const;
    DesignEqualsImplementationClass *ParentClass;

    QString methodSignatureWithoutReturnType(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);
    QString argumentsToCommaSeparatedString(MethodSignatureFlagsEnum methodSignatureFlagsEnum = MethodSignatureForVisualAppearanceContainsArgumentVariableNames);

protected:
    virtual QObject *asQObject()=0;
private:
    QList<DesignEqualsImplementationClassMethodArgument*> m_Arguments;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONMETHOD_H
