#ifndef BACKENDWINDOW_H
#define BACKENDWINDOW_H

#include <QDialog>
#include <QThread>
#include <mainwindow.h>

class MyThread;

namespace Ui {
class BackendWindow;
}

/*******后台信息窗口类*******/
class BackendWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BackendWindow(QWidget *parent = 0);
    ~BackendWindow();
    void Display();

private:
    Ui::BackendWindow *ui;
    MyThread *threadVector;
    //MainWindow mWinShow;

public:
    void addCloseButton();
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* event);
    QPoint mousePoint;
    bool mousePressed;

public slots:
    void windowclosed();
    void start(BackendWindow *ui);
    void stop();
};

/*******线程类*******/
class MyThread : public QThread
{
    Q_OBJECT
public:
    //MyThread();
    MyThread(BackendWindow *ui);
    ~MyThread();

    void run();

signals:
    void showInfo();

public slots:
    void slotUpdateUi();

public:
    BackendWindow *m_gui;
};

#endif // BACKENDWINDOW_H
