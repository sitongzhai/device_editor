//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_PROJECT_PROJECT_WIDGET_H
#define GUI_PROJECT_PROJECT_WIDGET_H

#include <qobject.h>
#include <qwidget.h>
#include <qtreewidget.h>
#include <qlabel.h>

namespace eda {

  class Project;

  class ProjectWidget : public QWidget {
    Q_OBJECT
  private:
    Project* project_;
    QLabel* path_label_;
    QTreeWidget* project_tree_;
    QTreeWidgetItem* device_;
    QTreeWidgetItem* netlists_;
    QTreeWidgetItem* constraints_;
    QTreeWidgetItem* others_;
    QMap<QString, QString> file_path_map_;
  public:
    ProjectWidget(QWidget* parent);
    ~ProjectWidget();
  public:
    void setProject(Project* project);
    void clear();
  private:
    void addFileItem(QString file_name, QTreeWidgetItem* parent);
    void clearAllNetlistItems();
    void clearAllConstraintItems();
    void clearAllOtherItems();
    void clearAllDeviceItems();
  private slots:
    void onOpenContainingFolder();
  
  };
}

#endif // !GUI_PROJECT_PROJECT_WIDGET_H
