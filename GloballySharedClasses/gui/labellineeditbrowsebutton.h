#ifndef LABELLINEEDITBROWSEBUTTON_H
#define LABELLINEEDITBROWSEBUTTON_H

#include <QBoxLayout>
#include <QLineEdit>
#include <QFileDialog>

class LabelLineEditBrowseButton : public QBoxLayout
{
    Q_OBJECT
public:
    enum FileDialogMode { OpenFileMode, SaveFileMode, OpenDirectoryMode };
    explicit LabelLineEditBrowseButton(const QString &labelText, FileDialogMode fileDialogMode = OpenFileMode, const QString &pushButtonText = tr("Browse"), QBoxLayout::Direction direction = QBoxLayout::LeftToRight, QWidget *parent = 0);
    QLineEdit *lineEdit();
    QFileDialog &fileDialog();
private:
    QLineEdit *m_MyLineEdit;
    QFileDialog m_FileDialog;
signals:
    void pathSelectedFromBrowseDialog(const QString &pathSelected);
private slots:
    void handleBrowseButtonClicked();
};

#endif // LABELLINEEDITBROWSEBUTTON_H
