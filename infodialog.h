#include <QDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();
    void Display(QString str);

public:
    void addCloseButton();
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* event);
    QPoint mousePoint;
    bool mousePressed;

private:
    Ui::InfoDialog *ui;

public slots:
    void windowclosed();
};
