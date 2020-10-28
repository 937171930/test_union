#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backendwindow.h"
#include "ui_backendwindow.h"

#include <QToolButton>
#include <QPixmap>
#include <QStyle>
#include <QMouseEvent>

extern int packageNum;
extern int serverMark;
extern char Status[7];
extern char mechineLoad[6][4];
extern char taskPool[6];
extern char taskDistribute[6][6];
extern char taskPriority[6];
extern MainWindow *w;

/*******后台信息窗口类构造方法********/
BackendWindow::BackendWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackendWindow)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);    //无边框模式

    ui->setupUi(this);

    this->start(this);    //QT定时刷新状态信息线程
}

/*******后台信息窗口类析构方法********/
BackendWindow::~BackendWindow()
{
    delete ui;
}

/*******后台信息打印线程服务函数********/
void BackendWindow::Display(){
    //打印当前信息
    ui->textEdit->append("");

    QString str = "Package Number:" + QString::number(packageNum);
    ui->textEdit->append(str);

    ui->textEdit->append("Current Server:Node " + QString::number(serverMark));

    ui->textEdit->append("Status:");
    str = "";
    for(int j = 0;j < 6;j++){
        str.append(Status[j]);
    }
    ui->textEdit->append(str);

    ui->textEdit->append("Load:");
    for(int i = 0;i < 6;i++){
        QString str = "";
        for(int j = 0;j < 4;j++){
            str += QString::number(mechineLoad[i][j]) + " ";
        }
        ui->textEdit->append(str);
    }

    ui->textEdit->append("taskPool:");
    str = "";
    for(int j = 0;j < 6;j++){
        str += QString::number(taskPool[j]) + " ";
    }
    ui->textEdit->append(str);

    ui->textEdit->append("taskDistribute:");
    for(int i = 0;i < 6;i++){
        QString str = "";
        for(int j = 0;j < 6;j++){
            str += QString::number(taskDistribute[i][j]) + " ";
        }
        ui->textEdit->append(str);
    }

    ui->textEdit->append("taskPriority:");
    str = "";
    for(int j = 0;j < 6;j++){
        str += QString::number(taskPriority[j] - '0') + " ";
    }
    ui->textEdit->append(str);

    packageNum++;
}

/*******关闭按钮信号槽函数*******/
void BackendWindow::windowclosed(){
    this->stop();
    this->close();
    w->ui->actionSHOW->setEnabled(true);
}

/*******UI界面添加关闭按钮*******/
void BackendWindow::addCloseButton(){
    int wide = width();                                                         //获取界面的宽度
    QToolButton *closeButton= new QToolButton(this);    //构建关闭按钮
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton); //获取关闭按钮图标
    closeButton->setIcon(closePix);                         //设置关闭按钮图标
    closeButton->setGeometry(wide-21,3,18,18);              //设置关闭按钮在界面的位置
    closeButton->setToolTip(tr("关闭"));                     //设置鼠标移至按钮上的提示信息
    closeButton->setStyleSheet("background-color:red;");    //设置关闭按钮的样式
    connect(closeButton, SIGNAL(clicked()), this, SLOT(windowclosed()) );
}

void BackendWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
        mousePoint = event->pos();
    }
}

void BackendWindow::mouseReleaseEvent(QMouseEvent *event){
    mousePressed    = false;
}

void BackendWindow::mouseMoveEvent(QMouseEvent *event){
    if(mousePressed && event->buttons()){
        this->move(event->globalPos() - mousePoint);
        event->accept();
    }
}

/*******启动打印线程*******/
void BackendWindow::start(BackendWindow *ui){
    threadVector = new MyThread(ui);
    threadVector->start();
}

/*******结束打印线程*******/
void BackendWindow::stop(){
    threadVector->terminate();
    threadVector->wait();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------//
/*******线程类构造函数*******/
MyThread::MyThread(BackendWindow *gui)
{
    m_gui = gui;
    connect(this, SIGNAL(showInfo()), this, SLOT(slotUpdateUi()));
}

/*******线程类析构函数*******/
MyThread::~MyThread(){}

void MyThread::run()
{
    //发出信号，此时调用槽函数
    while(1){
        emit showInfo();
        msleep(500);
    }
}

/*******后台信息显示信号槽*******/
void MyThread::slotUpdateUi()
{
    m_gui->Display();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------//




