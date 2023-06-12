//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <qdir.h>
#include <qfileinfo.h>
#include <qboxlayout.h>

#include "gui/gui.h"
#include "gui/project/source_file_selector.h"

namespace eda {

  SourceFileSelector::SourceFileSelector(QWidget* parent) : QWidget(parent) {
    setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* vbox_layout = new QVBoxLayout(this);
    vbox_layout->setMargin(0);
    vbox_layout->setSpacing(1);
    setLayout(vbox_layout);
    file_list_ = new QListWidget(this);
    file_list_->setSelectionMode(QAbstractItemView::MultiSelection);
    vbox_layout->addWidget(file_list_);
    QHBoxLayout* hbox_layout = new QHBoxLayout();
    add_btn_ = new QPushButton("Add File(s)...", this);
    remove_btn_ = new QPushButton("Remove selected file(s)", this);
    hbox_layout->addWidget(add_btn_);
    hbox_layout->addWidget(remove_btn_);
    vbox_layout->addLayout(hbox_layout);
    connect(add_btn_, SIGNAL(clicked()), this, SLOT(onAddFile()));
    connect(remove_btn_, SIGNAL(clicked()), this, SLOT(onRemoveFile()));
  }

  SourceFileSelector::~SourceFileSelector() {}

  void SourceFileSelector::onAddFile() {
    QStringList file_names = Gui::getOpenFileNames(this, tr("Open File"), pwd_, filter_);
    foreach(QString file_name, file_names) {
      file_name = QDir::fromNativeSeparators(file_name);
      if (!files_.contains(file_name)) {
        files_.append(file_name);
        file_list_->addItem(file_name);
      }
    }
    if (!files_.isEmpty()) {
      pwd_ = QFileInfo(files_.last()).dir().absolutePath();
    }
    emit listChanged(file_names);
  }

  void SourceFileSelector::onRemoveFile() {
    QStringList file_lists;
    QList<QListWidgetItem*> selectedFiles = file_list_->selectedItems();
    for (auto iter_file = selectedFiles.begin(); iter_file != selectedFiles.end(); ++iter_file) {
      if ((*iter_file) == NULL) continue;
      QString file = (*iter_file)->text();
      delete (*iter_file);
      files_.removeOne(file);
      file_lists.push_back(file);
    }
    if (!files_.isEmpty()) {
      pwd_ = QFileInfo(files_.last()).dir().absolutePath();
    }
    emit listChanged(file_lists);
  }

}