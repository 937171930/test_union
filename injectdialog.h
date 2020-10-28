#ifndef INJECTDIALOG_H
#define INJECTDIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include <QTimer>

namespace Ui {
class InjectDialog;
}

class InjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InjectDialog(QWidget *parent = nullptr);
    ~InjectDialog();

public:
    void addCloseButton();
    void udpsend();
    bool taskRebuild(int index);
    void progressRebuild();
    void progressRecovery();
    //void mousePressEvent(QMouseEvent* event);
    //void mouseReleaseEvent(QMouseEvent* ev);
    //void mouseMoveEvent(QMouseEvent* event);
    //QPoint mousePoint;
    //bool mousePressed;

private:
    Ui::InjectDialog *ui;

private:
    QProgressDialog *pd;
    QTimer *t;

public slots:
    void windowclosed();
    void getNodes();
    void getTasks();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_comboBox_2_currentIndexChanged(const QString &arg1);
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_checkBox_3_clicked(bool checked);
    void on_checkBox_4_clicked(bool checked);
    void on_checkBox_5_clicked(bool checked);
    void on_checkBox_6_clicked(bool checked);
    void on_checkBox_7_clicked(bool checked);
    void on_checkBox_8_clicked(bool checked);
    void on_checkBox_9_clicked(bool checked);
    void on_checkBox_10_clicked(bool checked);
    void on_checkBox_11_clicked(bool checked);
    void on_checkBox_12_clicked(bool checked);
    void perform();
    void perform1();
    void on_checkBox_18_clicked(bool checked);
};

#endif // INJECTDIALOG_H
