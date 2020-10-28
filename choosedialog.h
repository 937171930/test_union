#ifndef CHOOSEDIALOG_H
#define CHOOSEDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseDialog;
}

class ChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseDialog(QWidget *parent = nullptr);
    ~ChooseDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChooseDialog *ui;
};

#endif // CHOOSEDIALOG_H
