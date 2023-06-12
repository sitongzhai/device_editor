//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_PROJECT_NEW_PROJECT_WIZARD_H
#define GUI_PROJECT_NEW_PROJECT_WIZARD_H

#include <qwizard.h>
#include <qobject.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qtextedit.h>
#include <qcheckbox.h>

#include "gui/gui.h"

namespace eda {

  class SourceFileSelector;
  class Project;

  class NewProjectWizard : public QWizard {
    Q_OBJECT
  public:
    class NamePage;
    class DevicePage;
    class SourcePage;
    class ConclusionPage;
  private:
    Project* project_;
    NamePage* name_page_;
    DevicePage* device_page_;
    SourcePage* source_page_;
    ConclusionPage* conclusion_page_;
  public:
    NewProjectWizard(QWidget* parent);
    ~NewProjectWizard();

  };

  class NewProjectWizard::NamePage : public QWizardPage {
    Q_OBJECT
  private:
    Project* project_;
    QLineEdit* name_edit_;
    QLineEdit* path_edit_;
    QString path_str_;
    QTextEdit* description_edit_;
  public:
    NamePage(QWidget* parent, Project* project);
    ~NamePage() {}

  private:
    bool isComplete() const;
    bool validatePage();

  protected slots:
    void onClickDirectionButton();

  public slots:
    void onNameEditChanged(const QString& name);
    void onProjectPathEdited(const QString& name);

  };

  class NewProjectWizard::DevicePage : public QWizardPage {
    Q_OBJECT
  public:
    DevicePage(QWidget* parent, Project* project);
    ~DevicePage() { }
  private:
    bool isComplete() const;
    bool validatePage();
    void cleanupPage();

  public slots:
    void onDeviceNameChanged(int index);
    void onDeviceFamilyChanged(int index);

  private:
    Project* project_;
    QComboBox* family_combo_;
    QComboBox* device_combo_;
    QComboBox* package_combo_;
    QComboBox* speed_combo_;
  };

  class NewProjectWizard::SourcePage : public QWizardPage {
    Q_OBJECT
  public:
    SourcePage(QWidget* parent, Project* project);
    ~SourcePage() {}

    SourceFileSelector* file_selecter() { return file_selector_; }
  private:
    bool validatePage();
    void cleanupPage();

  private:
    Project* project_;
    SourceFileSelector* file_selector_;
    QCheckBox* copy_file_box_;
  };

  class NewProjectWizard::ConclusionPage : public QWizardPage {
    Q_OBJECT
  public:
    ConclusionPage(QWidget* parent, Project* project);
    ~ConclusionPage();

  private:
    void initializePage();
    bool validatePage();
    void cleanupPage();

  private:
    Project* project_;
    QTextEdit* conclusion_text_;
  };


}

#endif // !GUI_PROJECT_NEW_PROJECT_WIZARD_H
