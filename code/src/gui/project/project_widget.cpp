//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <qboxlayout.h>
#include <qurl.h>
#include <qdir.h>
#include <qaction.h>
#include <qdesktopservices.h>
#include <qfileinfo.h>

#include "gui/project/project_widget.h"
#include "gui/project/project.h"

namespace eda {

  ProjectWidget::ProjectWidget(QWidget* parent) : QWidget(parent) {
    this->setObjectName("PROJECT_WIDGET");
    QVBoxLayout* vbox_layout = new QVBoxLayout(this);
    vbox_layout->addWidget(new QLabel("<b>Manage Project</b>", this), 0, Qt::AlignTop);
    path_label_ = new QLabel(this);
    path_label_->setText("Current path:\n" + QDir::currentPath());
    path_label_->setWordWrap(true);
    path_label_->setMaximumWidth(236);
    path_label_->setContextMenuPolicy(Qt::ActionsContextMenu);
    vbox_layout->addWidget(path_label_);
    vbox_layout->addSpacing(10);
    project_tree_ = new QTreeWidget(this);
    project_tree_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    project_tree_->setHeaderLabel("No Project");
    vbox_layout->addWidget(project_tree_);

    device_ = new QTreeWidgetItem(project_tree_);
    device_->setText(0, "No device");
    netlists_ = new QTreeWidgetItem(device_);
    netlists_->setText(0, "Netlist files");
    constraints_ = new QTreeWidgetItem(device_);
    constraints_->setText(0, "Constraint files");
    others_ = new QTreeWidgetItem(device_);
    others_->setText(0, "Others");

  }
  ProjectWidget::~ProjectWidget() {
    clearAllDeviceItems();
    if (device_) {
      if (netlists_) {
        device_->removeChild(netlists_);
        delete netlists_;
      }
      if (constraints_) {
        device_->removeChild(constraints_);
        delete constraints_;
      }
      if (others_) {
        device_->removeChild(others_);
        delete others_;
      }
    }
    
  }
  void ProjectWidget::setProject(Project* project) {
    clear();

    if (project == NULL) return;

    project_ = project;

    project_tree_->setHeaderLabel("Project: " + project->project_name());

    QString device_name = project->family() + project->device() + project->package() + project->speed();
    device_->setText(0, device_name);

    if (project->hasXdlFile()) {
      addFileItem(project->xdl_file(), netlists_);
    }
    if (project->hasEdifFile()) {
      QString edif_file_name = QFileInfo(project->edif_file()).fileName();
      addFileItem(edif_file_name, netlists_);
    }
    if (project->hasBlifFile()) {
      QString blif_file_name = QFileInfo(project->blif_file()).fileName();
      addFileItem(blif_file_name, netlists_);
    }

    if (project->hasUcfFile()) {
      QString ucf_file_name = QFileInfo(project->ucf_file()).fileName();
      addFileItem(ucf_file_name, constraints_);
    }
    if (project->hasSdcFile()) {
      QString sdc_file_name = QFileInfo(project->sdc_file()).fileName();
      addFileItem(sdc_file_name, constraints_);
    }

    //FIXME: TODO
    //others_->setText(0, "Others");
  }
  void ProjectWidget::clear() {
    project_ = NULL;
    file_path_map_.clear();
    path_label_->setText("Current path:\n" + QDir::currentPath());
    project_tree_->setHeaderLabel("No Project");
    device_->setText(0, "No device");
    clearAllDeviceItems();
  }
  void ProjectWidget::addFileItem(QString file, QTreeWidgetItem* parent) {
    QTreeWidgetItem* file_item = NULL;
    if (parent == NULL) {
      file_item = new QTreeWidgetItem(project_tree_);
    } else {
      file_item = new QTreeWidgetItem(parent);
    }
    QFileInfo file_info(file);
    QString file_name = file_info.fileName();
    QString file_path = file_info.absoluteFilePath();
    file_path_map_[file_name] = file_path;
    QLabel* file_label = new QLabel(file_name, project_tree_);
    project_tree_->setItemWidget(file_item, 0, file_label);
    QAction* open_location_action = new QAction("Open Containing Folder", file_label);
    file_label->setContextMenuPolicy(Qt::ActionsContextMenu);
    file_label->addAction(open_location_action);
    connect(open_location_action, SIGNAL(triggered()), this, SLOT(onOpenContainingFolder()));
  }
  void ProjectWidget::clearAllNetlistItems() {
    if (netlists_ == NULL) return;
    for (int i = 0; i < netlists_->childCount(); i++) {
      QTreeWidgetItem* child = netlists_->child(i);
      netlists_->removeChild(child);
      project_tree_->removeItemWidget(child, 0);
      delete child;
    }
  }
  void ProjectWidget::clearAllConstraintItems() {
    if (constraints_ == NULL) return;
    for (int i = 0; i < constraints_->childCount(); i++) {
      QTreeWidgetItem* child = constraints_->child(i);
      constraints_->removeChild(child);
      project_tree_->removeItemWidget(child, 0);
      delete child;
    }
  }
  void ProjectWidget::clearAllOtherItems() {
    if (others_ == NULL) return;
    for (int i = 0; i < others_->childCount(); i++) {
      QTreeWidgetItem* child = others_->child(i);
      others_->removeChild(child);
      project_tree_->removeItemWidget(child, 0);
      delete child;
    }
  }
  void ProjectWidget::clearAllDeviceItems() {
    clearAllNetlistItems();
    clearAllConstraintItems();
    clearAllOtherItems();
  }
  void ProjectWidget::onOpenContainingFolder() {
    QAction* open_location_action = qobject_cast<QAction*>(sender());
    QLabel* file_label = qobject_cast<QLabel*>(open_location_action->parentWidget());
    QString file_path = file_path_map_[file_label->text()];
    QDesktopServices::openUrl(QFileInfo(file_path).absolutePath());
  }


}