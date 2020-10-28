#ifndef FAILUREDIALOG_H
#define FAILUREDIALOG_H

#include <QDialog>

namespace Ui {
class failureDialog;
}

class failureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit failureDialog(QWidget *parent = nullptr);
    ~failureDialog();

public:
    void addCloseButton();

private:
    Ui::failureDialog *ui;

public slots:
    void windowclosed();
};

#endif // FAILUREDIALOG_H
