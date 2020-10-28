#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QList>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <QStandardItemModel>
//#include <arpa/inet.h>


#include "myLineEdit.h"
#include "failuredialog.h"
#include "injectdialog.h"

class MyThreadp;
class MyThreadp2;

#define MAX_READ_LINE 1024
#define overloadThreshold	80
#define balanceThreshold	15

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Display();
    void twinkle();

private slots:
    //添加槽函数
    void on_actionSHOW_triggered();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_18_clicked();

    void on_action_triggered();
    void on_action_3_triggered();

public:
    Ui::MainWindow *ui;
    static const int mark = 5;			//本机标识
    static const char taskCPUWeight[6];	//任务占用CPU权重
    static const char taskMEMWeight[6]; //任务占用内存权重
    static const char taskIOWeight[6];	//任务占用IO权重
    static const char taskNETWeight[6];	//任务占用网络权重
    static const char packageLen = 79;	//应答包长度
    static const QString IPpool[6];     //IP池
    QPoint mousePoint;
    bool mousePressed;

public:
    static int UDPsendAsClient(char const * server_ip_addr, int server_ip_port);
    static void statusUpdate(char* buffer);
    static void* UDPreceive(void*);
    int isTaskPoolEmpty();
    static void mechineLoadCal();
    static int findLowest();
    static int findHighest();
    static int findLowestWithoutIndex(int index);
    static void mechineOverloadDeal();
    static void mechineOfflineDeal();
    static int findLowestTask(int index);
    static int taskScheduling(int minIndex,int maxIndex);
    static void loadBalance();
    static void printInfo();
    static void taskPoolDistribute();
    static int UDPsendAsServer(char const * server_ip_addr, int server_ip_port);
    static void threadCancel(int index);
    static int findNewServer();
    static void* heartBeat(void*);
    static void replyToClient();
    static void* resetTimer(void*);
    static void threadCreateAsServer(void);
    static void* serverListenning(void*);
    void threadCreateAsClient(void);
    void setNodeLabel(void);
    void setTableWidget(void);
    void setTextEditor(void);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void logging(char type, QString msg);
    void onlineTableShow(QStandardItemModel *model,const char* taskDistribute,const char* taskStatus,int numPriority);
    void offlineTableShow(QStandardItemModel *model);
    failureDialog* failureInject(int index);

private:
    MyThreadp   *threadVector;
    MyThreadp2  *threadVector2;

public slots:
    void start(MainWindow *ui);
    void stop();
    void createData();
    void lineEditClicked();
    void lineEdit2Clicked();
    void lineEdit3Clicked();
    void lineEdit4Clicked();
    void lineEdit5Clicked();
    void lineEdit6Clicked();
    void lineEdit7Clicked();
    void lineEdit8Clicked();
    void lineEdit9Clicked();
    void lineEdit10Clicked();
    void lineEdit11Clicked();
    void lineEdit12Clicked();
    void lineEdit13Clicked();
    void lineEdit14Clicked();
    void lineEdit15Clicked();
    void lineEdit16Clicked();
    void lineEdit17Clicked();
    void lineEdit18Clicked();
    void lineEdit19Clicked();
    void lineEdit20Clicked();
    void lineEdit21Clicked();
    void lineEdit22Clicked();
    void lineEdit23Clicked();
    void lineEdit24Clicked();

protected:
    bool eventFilter(QObject *watched, QEvent *e);
    void paintOnMainWindow(QWidget *w, int index, int choose);
    void clearOnMainWindow(QWidget *w);

public:
    QTimer* m_Timer;
    QList<float> xList[4][6];
    QList<float> yList[4][6];
    QPointF points[6][4][56];

};

/*******线程类1*******/
class MyThreadp : public QThread
{
    Q_OBJECT
public:
    MyThreadp(MainWindow *ui);
    ~MyThreadp();

    void run();

signals:
    void showInfo();

public slots:
    void slotUpdateUi();

public:
    MainWindow *m_gui;
};

/*******线程类2*******/
class MyThreadp2 : public QThread
{
    Q_OBJECT
public:
    MyThreadp2(MainWindow *ui);
    ~MyThreadp2();

    void run();

signals:
    void showInfo();

public slots:
    void slotUpdateButton();

public:
    MainWindow *m_gui;
};

#endif // MAINWINDOW_H
