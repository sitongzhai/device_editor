//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_PROJECT_SOURCE_FILE_SELECTOR_H
#define GUI_PROJECT_SOURCE_FILE_SELECTOR_H

#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistwidget.h>
#include <qpushbutton.h>

namespace eda {
  class SourceFileSelector : public QWidget {
    Q_OBJECT
  private:
    QListWidget* file_list_;
    QPushButton* add_btn_;
    QPushButton* remove_btn_;
    QStringList files_;

    QString filter_;
    QString pwd_;
  public:
    SourceFileSelector(QWidget* parent = NULL);
    ~SourceFileSelector();

    QStringList files() { return files_; }
    void set_pwd(const QString& pwd) { pwd_ = pwd; }
    void set_filter(const QString& filter) { filter_ = filter; }

  signals:
    void listChanged(const QStringList&);

  protected slots:
    void onAddFile();
    void onRemoveFile();

  };

}
#endif // !GUI_PROJECT_SOURCE_FILE_SELECTOR_H
