#ifndef DESIGNPROJECT_H
#define DESIGNPROJECT_H

#include <QObject>
#include <QList>

#include "designprojectview.h"

class DesignProject : public QObject
{
    Q_OBJECT
public:    
    QList<DesignProjectView*> *m_DesignProjectViewsForThisProject; //initially just the class diagram but use cases are added
    explicit DesignProject(QString projectName);
    QString getProjectName();
    void addProjectView(DesignProjectView *designProjectView);
    void createEmptyProject(); //called if the recently instantiated DesignProject is "new"/empty
    //TODOreq: void loadExistingProject(/*args*/);
    DesignProjectView *getCurrentProjectView();
    void setCurrentProjectView(DesignProjectView *newCurrentProjectView);
private:
    QString m_ProjectName;
    QList<DesignProjectView*> *m_ProjectViewList;
    DesignProjectView *m_CurrentProjectView;
signals:
    void projectViewAdded(DesignProjectView*);
    void currentProjectViewChanged(DesignProjectView *newCurrentProjectView);

public slots:

};

#endif // DESIGNPROJECT_H
