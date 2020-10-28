#include "mainwindow.h"
#include "backendwindow.h"
#include "ui_mainwindow.h"
#include "infodialog.h"

#include <QToolButton>
#include <QPixmap>
#include <QStyle>
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QFile>
#include <QDesktopWidget>
#include <QPainter>
#include <QPointF>
#include <QPen>
#include <QDate>
#include <QMouseEvent>
#include <QStandardItemModel>

#include <qmath.h>
#include <myModel.h>
#include <udpReceiver.h>

int serverMark = -1;	//服务器标识
pthread_t thread[3];    //线程池
pthread_mutex_t mutex;  //互斥锁
const char* taskName[6] = {"controlTask1","controlTask2","controlTask3","controlTask4","calculateTask5","calculateTask6"};
char Status[7] = {'0','0','0','0','0','0',0}; 	//在线状态
char taskPool[6] = {3,3,3,3,3,3};  				//任务池
char taskDistribute[6][6];                      //任务分配情况
char taskFailure[6][6];                         //任务故障注入情况
char taskPriority[6];                           //单机高优先级运算任务
char taskPriorityToDo[6];                       //下发单机高优先级运算任务
char mechineLoad[6][4];                         //单机负载率
char showChoose[6] = {0};                       //负载感知显示选择；
int memoryFailure[6][4] = {{0}};                //注入内存故障记录数组
unsigned int addrFailure[6][20] = {{0}};        //内存故障地址记录数组
int packageNum = 0;                             //接收包序号
int killLastSignal = 1;                         //用于关闭最后主机显示的同步信号
int clickedID = 0;                              //failure pushbutton ID
failureDialog* failureDialogArray[6] = {NULL};   //failure dialog record array


extern MainWindow *w;
extern QFile *file;
extern InfoDialog* pInfo;
extern InjectDialog* pInject;

const int tableViewRow = 22;
const int tableViewCol = 3;
QStandardItemModel* model[6];
const QColor colors[] = {
    QColor(255,111,111),
    QColor(111,255,111),
    QColor(111,111,255),
    QColor(255,255,111),
    QColor(255,111,255),
    QColor(111,255,255),
    QColor(188,188,188),
};

const char MainWindow::taskCPUWeight[6] = {3,5,6,7,10,15};
const char MainWindow::taskMEMWeight[6] = {4,4,7,7,7,10};
const char MainWindow::taskIOWeight[6] = {5,5,5,5,6,6};
const char MainWindow::taskNETWeight[6] = {6,8,10,12,9,9};
const QString MainWindow::IPpool[6] = {             //IP池
    //"10.0.7.221",
    //"10.0.7.221",
    //"10.0.7.221",
    //"10.0.7.221",
    //"10.0.7.221",
    //"10.0.7.221"
    "192.168.1.11",
    "192.168.1.14",
    "192.168.1.16",
    "192.168.1.18",
    "192.168.1.17",
    "192.168.1.13"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //setWindowFlags(Qt::FramelessWindowHint);    //无边框模式
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //setFixedSize(1280, 720);
    ui->setupUi(this);
    setNodeLabel();
    setTableWidget();
    setTextEditor();

    this->start(this);    //启动QT定时刷新前台显示线程

    /*******启动定时器，控制画负载率曲线*******/
    for(int j = 0;j < 4;j++){
        for(int i = 0;i < 6;i++){
            int m_count = 0;
            while(m_count < 50){
                xList[j][i].append(m_count++);
                yList[j][i].append(0);
            }
        }
    }
    m_Timer = new QTimer(this);
    m_Timer->setInterval(1000);
    connect(m_Timer,SIGNAL(timeout()),this,SLOT(createData()));
    m_Timer->start();
    ui->frame_14->installEventFilter(this);
    ui->frame_16->installEventFilter(this);
    ui->frame_18->installEventFilter(this);
    ui->frame_20->installEventFilter(this);
    ui->frame_22->installEventFilter(this);
    ui->frame_24->installEventFilter(this);

    //添加链接，当点击发送到
    connect(ui->lineEdit, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->lineEdit_2, SIGNAL(clicked()), this, SLOT(lineEdit2Clicked()));
    connect(ui->lineEdit_3, SIGNAL(clicked()), this, SLOT(lineEdit3Clicked()));
    connect(ui->lineEdit_4, SIGNAL(clicked()), this, SLOT(lineEdit4Clicked()));
    connect(ui->lineEdit_5, SIGNAL(clicked()), this, SLOT(lineEdit5Clicked()));
    connect(ui->lineEdit_6, SIGNAL(clicked()), this, SLOT(lineEdit6Clicked()));
    connect(ui->lineEdit_7, SIGNAL(clicked()), this, SLOT(lineEdit7Clicked()));
    connect(ui->lineEdit_8, SIGNAL(clicked()), this, SLOT(lineEdit8Clicked()));
    connect(ui->lineEdit_9, SIGNAL(clicked()), this, SLOT(lineEdit9Clicked()));
    connect(ui->lineEdit_10, SIGNAL(clicked()), this, SLOT(lineEdit10Clicked()));
    connect(ui->lineEdit_11, SIGNAL(clicked()), this, SLOT(lineEdit11Clicked()));
    connect(ui->lineEdit_12, SIGNAL(clicked()), this, SLOT(lineEdit12Clicked()));
    connect(ui->lineEdit_13, SIGNAL(clicked()), this, SLOT(lineEdit13Clicked()));
    connect(ui->lineEdit_14, SIGNAL(clicked()), this, SLOT(lineEdit14Clicked()));
    connect(ui->lineEdit_15, SIGNAL(clicked()), this, SLOT(lineEdit15Clicked()));
    connect(ui->lineEdit_16, SIGNAL(clicked()), this, SLOT(lineEdit16Clicked()));
    connect(ui->lineEdit_17, SIGNAL(clicked()), this, SLOT(lineEdit17Clicked()));
    connect(ui->lineEdit_18, SIGNAL(clicked()), this, SLOT(lineEdit18Clicked()));
    connect(ui->lineEdit_19, SIGNAL(clicked()), this, SLOT(lineEdit19Clicked()));
    connect(ui->lineEdit_20, SIGNAL(clicked()), this, SLOT(lineEdit20Clicked()));
    connect(ui->lineEdit_21, SIGNAL(clicked()), this, SLOT(lineEdit21Clicked()));
    connect(ui->lineEdit_22, SIGNAL(clicked()), this, SLOT(lineEdit22Clicked()));
    connect(ui->lineEdit_23, SIGNAL(clicked()), this, SLOT(lineEdit23Clicked()));
    connect(ui->lineEdit_24, SIGNAL(clicked()), this, SLOT(lineEdit24Clicked()));

    memset(taskPriorityToDo,'0',6);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setNodeLabel(){
    ui->label->setStyleSheet("color:black");

    QString s = "资源占用率";
    ui->label_28->setWordWrap(true);
    ui->label_28->setAlignment(Qt::AlignTop);
    ui->label_28->setText(s.split("",QString::SkipEmptyParts).join("\n"));
    ui->label_44->setWordWrap(true);
    ui->label_44->setAlignment(Qt::AlignTop);
    ui->label_44->setText(s.split("",QString::SkipEmptyParts).join("\n"));
    ui->label_60->setWordWrap(true);
    ui->label_60->setAlignment(Qt::AlignTop);
    ui->label_60->setText(s.split("",QString::SkipEmptyParts).join("\n"));
    ui->label_76->setWordWrap(true);
    ui->label_76->setAlignment(Qt::AlignTop);
    ui->label_76->setText(s.split("",QString::SkipEmptyParts).join("\n"));
    ui->label_92->setWordWrap(true);
    ui->label_92->setAlignment(Qt::AlignTop);
    ui->label_92->setText(s.split("",QString::SkipEmptyParts).join("\n"));
    ui->label_108->setWordWrap(true);
    ui->label_108->setAlignment(Qt::AlignTop);
    ui->label_108->setText(s.split("",QString::SkipEmptyParts).join("\n"));
}

void MainWindow::setTableWidget(){
    model[0] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView->setModel(model[0]);
    model[0]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[0]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[0]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView->setColumnWidth(0,95);
    ui->tableView->setColumnWidth(1,70);
    ui->tableView->setColumnWidth(2,50);

    model[1] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView_2->setModel(model[1]);
    model[1]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[1]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[1]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView_2->setColumnWidth(0,95);
    ui->tableView_2->setColumnWidth(1,70);
    ui->tableView_2->setColumnWidth(2,50);

    model[2] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView_3->setModel(model[2]);
    model[2]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[2]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[2]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView_3->setColumnWidth(0,95);
    ui->tableView_3->setColumnWidth(1,70);
    ui->tableView_3->setColumnWidth(2,50);

    model[3] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView_4->setModel(model[3]);
    model[3]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[3]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[3]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView_4->setColumnWidth(0,95);
    ui->tableView_4->setColumnWidth(1,70);
    ui->tableView_4->setColumnWidth(2,50);

    model[4] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView_5->setModel(model[4]);
    model[4]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[4]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[4]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView_5->setColumnWidth(0,95);
    ui->tableView_5->setColumnWidth(1,70);
    ui->tableView_5->setColumnWidth(2,50);

    model[5] = new QStandardItemModel(tableViewRow,tableViewCol);
    ui->tableView_6->setModel(model[5]);
    model[5]->setHeaderData(0, Qt::Horizontal, tr("taskName"));
    model[5]->setHeaderData(1, Qt::Horizontal, tr("partition"));
    model[5]->setHeaderData(2, Qt::Horizontal, tr("mark"));
    ui->tableView_6->setColumnWidth(0,95);
    ui->tableView_6->setColumnWidth(1,70);
    ui->tableView_6->setColumnWidth(2,50);
}

void MainWindow::setTextEditor(){
    ui->lineEdit->setEchoMode(QLineEdit::Normal);
    ui->lineEdit->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_2->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_2->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_3->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_3->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_4->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_4->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_5->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_5->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_6->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_6->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_7->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_7->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_8->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_8->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_9->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_9->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_10->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_10->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_11->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_11->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_12->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_12->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_13->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_13->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_14->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_14->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_15->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_15->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_16->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_16->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_17->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_17->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_18->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_18->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_19->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_19->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_20->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_20->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_21->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_21->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_22->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_22->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_23->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_23->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_24->setEchoMode(QLineEdit::Normal);
    ui->lineEdit_24->setFocusPolicy(Qt::NoFocus);

}

void MainWindow::on_actionSHOW_triggered()
{
    //chinese support
    int id = QFontDatabase::addApplicationFont("../DroidSansFallback.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies (id).at(0);
    QFont font(msyh,10);
    qDebug()<<msyh<<endl;
    font.setPointSize(12);

    BackendWindow* p = new BackendWindow();
    p->setFont(font);
    p->move(w->width(),520);
    p->addCloseButton();
    p->show();

    //设置菜单不可交互
    ui->actionSHOW->setEnabled(false);
}

void MainWindow::onlineTableShow(QStandardItemModel *model,const char* taskDistribute,const char* taskFailure,int numPriority){
    int j = 0;
    for(int i = 0;i < 6;i++){     //任务监视表
        int temp = taskDistribute[i];
        while(temp--){
            QModelIndex index = model->index(j, 0, QModelIndex());
            model->setData(index, taskName[i]);
            index = model->index(j, 1, QModelIndex());
            model->setData(index, "partition" + QString::number(j));
            index = model->index(j, 2, QModelIndex());
            model->setData(index, "normal");
            model->item(j  ,0)->setBackground(QBrush(colors[i]));
            model->item(j  ,1)->setBackground(QBrush(colors[i]));
            model->item(j++,2)->setBackground(QBrush(colors[i]));
        }
        temp = taskFailure[i];
        while(temp--){
            QModelIndex index = model->index(j, 0, QModelIndex());
            model->setData(index, taskName[i]);
            index = model->index(j, 1, QModelIndex());
            model->setData(index, "partition" + QString::number(j));
            index = model->index(j, 2, QModelIndex());
            model->setData(index, "failure");
            model->item(j  ,0)->setBackground(QBrush(colors[i]));
            model->item(j  ,1)->setBackground(QBrush(colors[i]));
            model->item(j++,2)->setBackground(QBrush(colors[i]));
        }
    }
    while(numPriority--){
        QModelIndex index = model->index(j, 0, QModelIndex());
        model->setData(index, "高优先级运算任务");
        index = model->index(j, 1, QModelIndex());
        model->setData(index, "partition" + QString::number(j));
        index = model->index(j, 2, QModelIndex());
        model->setData(index, "normal");
        model->item(j  ,0)->setBackground(QBrush(QColor(255,0,0)));
        model->item(j  ,1)->setBackground(QBrush(QColor(255,0,0)));
        model->item(j++,2)->setBackground(QBrush(QColor(255,0,0)));
    }
    while(j < tableViewRow){
        QModelIndex index = model->index(j, 0, QModelIndex());
        model->setData(index, "");
        index = model->index(j, 1, QModelIndex());
        model->setData(index, "");
        index = model->index(j, 2, QModelIndex());
        model->setData(index, "");
        model->item(j  ,0)->setBackground(QBrush(colors[6]));
        model->item(j  ,1)->setBackground(QBrush(colors[6]));
        model->item(j++,2)->setBackground(QBrush(colors[6]));
    }
}

void MainWindow::offlineTableShow(QStandardItemModel *model){
    for(int j = 0;j < 22;){
        QModelIndex index = model->index(j, 0, QModelIndex());
        model->setData(index, "");
        index = model->index(j, 1, QModelIndex());
        model->setData(index, "");
        index = model->index(j, 2, QModelIndex());
        model->setData(index, "");
        model->item(j  ,0)->setBackground(QBrush(colors[6]));
        model->item(j  ,1)->setBackground(QBrush(colors[6]));
        model->item(j++,2)->setBackground(QBrush(colors[6]));
    }
}

/*******闪烁线程服务函数********/
void MainWindow::twinkle(){

}

/*******前台展示线程服务函数********/
void MainWindow::Display(){
    /*****前台展示信息*****/
    if(Status[0] == '1'){
        ui->radioButton->setChecked(true);  //在线状态
        ui->frame->setEnabled(true);        //可交互性
        onlineTableShow(model[0],taskDistribute[0],taskFailure[0],taskPriority[0] - '0');    //表格
        ui->lineEdit->setText(QString::number(mechineLoad[0][0]));   //Load Text
        ui->lineEdit_2->setText(QString::number(mechineLoad[0][1]));
        ui->lineEdit_3->setText(QString::number(mechineLoad[0][2]));
        ui->lineEdit_4->setText(QString::number(mechineLoad[0][3]));

        ui->lineEdit_26->setText(QString::number(memoryFailure[0][0]));   //Failure Text
        ui->lineEdit_28->setText(QString::number(memoryFailure[0][1]));
        ui->lineEdit_25->setText(QString::number(memoryFailure[0][2]));
        ui->lineEdit_27->setText(QString::number(memoryFailure[0][3]));

        if(memoryFailure[0][0] != 0){
            ui->lineEdit_29->setText(QString::number((int)(memoryFailure[0][1]*1.0/memoryFailure[0][0]*100)));
            ui->lineEdit_30->setText(QString::number((int)(memoryFailure[0][2]*1.0/memoryFailure[0][0]*100)));
            ui->lineEdit_31->setText(QString::number((int)(memoryFailure[0][3]*1.0/memoryFailure[0][0]*100)));
        }
        else{
            ui->lineEdit_29->setText(QString::number(0));
            ui->lineEdit_30->setText(QString::number(0));
            ui->lineEdit_31->setText(QString::number(0));
        }
    }
    else{
        ui->radioButton->setChecked(false);
        ui->frame->setEnabled(false);
        offlineTableShow(model[0]);
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_25->clear();
        ui->lineEdit_26->clear();
        ui->lineEdit_27->clear();
        ui->lineEdit_28->clear();
        ui->lineEdit_29->clear();
        ui->lineEdit_30->clear();
        ui->lineEdit_31->clear();
    }

    if(Status[1] == '1'){
        ui->radioButton_2->setChecked(true);//在线状态
        ui->frame_2->setEnabled(true);      //可交互性
        onlineTableShow(model[1],taskDistribute[1],taskFailure[1],taskPriority[1] - '0');    //表格
        ui->lineEdit_5->setText(QString::number(mechineLoad[1][0]));   //Load Text
        ui->lineEdit_6->setText(QString::number(mechineLoad[1][1]));
        ui->lineEdit_7->setText(QString::number(mechineLoad[1][2]));
        ui->lineEdit_8->setText(QString::number(mechineLoad[1][3]));

        ui->lineEdit_32->setText(QString::number(memoryFailure[1][0]));   //Failure Text
        ui->lineEdit_33->setText(QString::number(memoryFailure[1][1]));
        ui->lineEdit_34->setText(QString::number(memoryFailure[1][2]));
        ui->lineEdit_35->setText(QString::number(memoryFailure[1][3]));

        if(memoryFailure[1][0] != 0){
            ui->lineEdit_36->setText(QString::number((int)(memoryFailure[1][1]*1.0/memoryFailure[1][0]*100)));
            ui->lineEdit_37->setText(QString::number((int)(memoryFailure[1][2]*1.0/memoryFailure[1][0]*100)));
            ui->lineEdit_38->setText(QString::number((int)(memoryFailure[1][3]*1.0/memoryFailure[1][0]*100)));
        }
        else{
            ui->lineEdit_36->setText(QString::number(0));
            ui->lineEdit_37->setText(QString::number(0));
            ui->lineEdit_38->setText(QString::number(0));
        }
    }
    else{
        ui->radioButton_2->setChecked(false);
        ui->frame_2->setEnabled(false);
        offlineTableShow(model[1]);
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit_7->clear();
        ui->lineEdit_8->clear();
        ui->lineEdit_32->clear();
        ui->lineEdit_33->clear();
        ui->lineEdit_34->clear();
        ui->lineEdit_35->clear();
        ui->lineEdit_36->clear();
        ui->lineEdit_37->clear();
        ui->lineEdit_38->clear();
    }

    if(Status[2] == '1'){
        ui->radioButton_3->setChecked(true);
        ui->frame_3->setEnabled(true);
        onlineTableShow(model[2],taskDistribute[2],taskFailure[2],taskPriority[2] - '0');    //表格
        ui->lineEdit_9->setText(QString::number(mechineLoad[2][0]));   //Load Text
        ui->lineEdit_10->setText(QString::number(mechineLoad[2][1]));
        ui->lineEdit_11->setText(QString::number(mechineLoad[2][2]));
        ui->lineEdit_12->setText(QString::number(mechineLoad[2][3]));

        ui->lineEdit_39->setText(QString::number(memoryFailure[2][0]));   //Failure Text
        ui->lineEdit_40->setText(QString::number(memoryFailure[2][1]));
        ui->lineEdit_41->setText(QString::number(memoryFailure[2][2]));
        ui->lineEdit_42->setText(QString::number(memoryFailure[2][3]));

        if(memoryFailure[2][0] != 0){
            ui->lineEdit_43->setText(QString::number((int)(memoryFailure[2][1]*1.0/memoryFailure[2][0]*100)));
            ui->lineEdit_44->setText(QString::number((int)(memoryFailure[2][2]*1.0/memoryFailure[2][0]*100)));
            ui->lineEdit_45->setText(QString::number((int)(memoryFailure[2][3]*1.0/memoryFailure[2][0]*100)));
        }
        else{
            ui->lineEdit_43->setText(QString::number(0));
            ui->lineEdit_44->setText(QString::number(0));
            ui->lineEdit_45->setText(QString::number(0));
        }
    }
    else{
        ui->radioButton_3->setChecked(false);
        ui->frame_3->setEnabled(false);
        offlineTableShow(model[2]);
        ui->lineEdit_9->clear();
        ui->lineEdit_10->clear();
        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_39->clear();
        ui->lineEdit_40->clear();
        ui->lineEdit_41->clear();
        ui->lineEdit_42->clear();
        ui->lineEdit_43->clear();
        ui->lineEdit_44->clear();
        ui->lineEdit_45->clear();
    }

    if(Status[3] == '1'){
        ui->radioButton_4->setChecked(true);
        ui->frame_4->setEnabled(true);
        onlineTableShow(model[3],taskDistribute[3],taskFailure[3],taskPriority[3] - '0');    //表格
        ui->lineEdit_13->setText(QString::number(mechineLoad[3][0]));   //Load Text
        ui->lineEdit_14->setText(QString::number(mechineLoad[3][1]));
        ui->lineEdit_15->setText(QString::number(mechineLoad[3][2]));
        ui->lineEdit_16->setText(QString::number(mechineLoad[3][3]));

        ui->lineEdit_46->setText(QString::number(memoryFailure[3][0]));   //Failure Text
        ui->lineEdit_47->setText(QString::number(memoryFailure[3][1]));
        ui->lineEdit_48->setText(QString::number(memoryFailure[3][2]));
        ui->lineEdit_49->setText(QString::number(memoryFailure[3][3]));

        if(memoryFailure[3][0] != 0){
            ui->lineEdit_50->setText(QString::number((int)(memoryFailure[3][1]*1.0/memoryFailure[3][0]*100)));
            ui->lineEdit_51->setText(QString::number((int)(memoryFailure[3][2]*1.0/memoryFailure[3][0]*100)));
            ui->lineEdit_52->setText(QString::number((int)(memoryFailure[3][3]*1.0/memoryFailure[3][0]*100)));
        }
        else{
            ui->lineEdit_50->setText(QString::number(0));
            ui->lineEdit_51->setText(QString::number(0));
            ui->lineEdit_52->setText(QString::number(0));
        }
    }
    else{
        ui->radioButton_4->setChecked(false);
        ui->frame_4->setEnabled(false);
        offlineTableShow(model[3]);
        ui->lineEdit_13->clear();
        ui->lineEdit_14->clear();
        ui->lineEdit_15->clear();
        ui->lineEdit_16->clear();
        ui->lineEdit_46->clear();
        ui->lineEdit_47->clear();
        ui->lineEdit_48->clear();
        ui->lineEdit_49->clear();
        ui->lineEdit_50->clear();
        ui->lineEdit_51->clear();
        ui->lineEdit_52->clear();
    }

    if(Status[4] == '1'){
        ui->radioButton_5->setChecked(true);
        ui->frame_5->setEnabled(true);
        onlineTableShow(model[4],taskDistribute[4],taskFailure[4],taskPriority[4] - '0');    //表格
        ui->lineEdit_17->setText(QString::number(mechineLoad[4][0]));   //Load Text
        ui->lineEdit_18->setText(QString::number(mechineLoad[4][1]));
        ui->lineEdit_19->setText(QString::number(mechineLoad[4][2]));
        ui->lineEdit_20->setText(QString::number(mechineLoad[4][3]));

        ui->lineEdit_53->setText(QString::number(memoryFailure[4][0]));   //Failure Text
        ui->lineEdit_54->setText(QString::number(memoryFailure[4][1]));
        ui->lineEdit_55->setText(QString::number(memoryFailure[4][2]));
        ui->lineEdit_56->setText(QString::number(memoryFailure[4][3]));

        if(memoryFailure[4][0] != 0){
            ui->lineEdit_57->setText(QString::number((int)(memoryFailure[4][1]*1.0/memoryFailure[4][0]*100)));
            ui->lineEdit_58->setText(QString::number((int)(memoryFailure[4][2]*1.0/memoryFailure[4][0]*100)));
            ui->lineEdit_59->setText(QString::number((int)(memoryFailure[4][3]*1.0/memoryFailure[4][0]*100)));
        }
        else{
            ui->lineEdit_57->setText(QString::number(0));
            ui->lineEdit_58->setText(QString::number(0));
            ui->lineEdit_59->setText(QString::number(0));
        }
    }
    else{
        ui->radioButton_5->setChecked(false);
        ui->frame_5->setEnabled(false);
        offlineTableShow(model[4]);
        ui->lineEdit_17->clear();
        ui->lineEdit_18->clear();
        ui->lineEdit_19->clear();
        ui->lineEdit_20->clear();
        ui->lineEdit_53->clear();
        ui->lineEdit_54->clear();
        ui->lineEdit_55->clear();
        ui->lineEdit_56->clear();
        ui->lineEdit_57->clear();
        ui->lineEdit_58->clear();
        ui->lineEdit_59->clear();
    }

    if(Status[5] == '1'){
        ui->radioButton_6->setChecked(true);
        ui->frame_6->setEnabled(true);
        onlineTableShow(model[5],taskDistribute[5],taskFailure[5],taskPriority[5] - '0');    //表格
        ui->lineEdit_21->setText(QString::number(mechineLoad[5][0]));   //Load Text
        ui->lineEdit_22->setText(QString::number(mechineLoad[5][1]));
        ui->lineEdit_23->setText(QString::number(mechineLoad[5][2]));
        ui->lineEdit_24->setText(QString::number(mechineLoad[5][3]));

        ui->lineEdit_60->setText(QString::number(memoryFailure[5][0]));   //Failure Text
        ui->lineEdit_61->setText(QString::number(memoryFailure[5][1]));
        ui->lineEdit_62->setText(QString::number(memoryFailure[5][2]));
        ui->lineEdit_63->setText(QString::number(memoryFailure[5][3]));

        if(memoryFailure[5][0] != 0){
            ui->lineEdit_64->setText(QString::number((int)(memoryFailure[5][1]*1.0/memoryFailure[5][0]*100)));
            ui->lineEdit_65->setText(QString::number((int)(memoryFailure[5][2]*1.0/memoryFailure[5][0]*100)));
            ui->lineEdit_66->setText(QString::number((int)(memoryFailure[5][3]*1.0/memoryFailure[5][0]*100)));
        }
        else{
            ui->lineEdit_64->setText(QString::number(0));
            ui->lineEdit_65->setText(QString::number(0));
            ui->lineEdit_66->setText(QString::number(0));
        }

    }
    else{
        ui->radioButton_6->setChecked(false);
        ui->frame_6->setEnabled(false);
        offlineTableShow(model[5]);
        ui->lineEdit_21->clear();
        ui->lineEdit_22->clear();
        ui->lineEdit_23->clear();
        ui->lineEdit_24->clear();
        ui->lineEdit_60->clear();
        ui->lineEdit_61->clear();
        ui->lineEdit_62->clear();
        ui->lineEdit_63->clear();
        ui->lineEdit_64->clear();
        ui->lineEdit_65->clear();
        ui->lineEdit_66->clear();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
        mousePoint = event->pos();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    mousePressed    = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(mousePressed && event->buttons()){
        this->move(event->globalPos() - mousePoint);
        event->accept();
    }
}

void MainWindow::logging(char type, QString msg){
    while(!file->open(QIODevice::ReadWrite | QIODevice::Append));//以读写切追加写入的方式操作文本
    QTextStream txtOutput(file);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    switch(type){
        case 'e':
            txtOutput << "[ERROR] errmsg: " + msg + " " + current_date << endl;
            break;
        case 'i':
            txtOutput << "[INFO] infomsg: " + msg + " " + current_date << endl;
            break;
    }
    file->close();
}

/*******启动打印线程*******/
void MainWindow::start(MainWindow* ui){
    threadVector  = new MyThreadp(ui);
    threadVector->start();
    //threadVector2 = new MyThreadp2(ui);
    //threadVector2->start();
}

/*******结束打印线程*******/
void MainWindow::stop(){
    threadVector->terminate();
    threadVector->wait();
    //threadVector2->terminate();
    //threadVector2->wait();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------//
/*******线程类1构造函数*******/
MyThreadp::MyThreadp(MainWindow *gui)
{
    m_gui = gui;
    connect(this, SIGNAL(showInfo()), this, SLOT(slotUpdateUi()));
}

/*******线程类析构函数*******/
MyThreadp::~MyThreadp(){}

void MyThreadp::run()
{
    //发出信号，此时调用槽函数
    while(1){
        emit showInfo();
        msleep(50);
    }
}

/*******后台信息显示信号槽*******/
void MyThreadp::slotUpdateUi()
{
    m_gui->Display();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------------------------------------//
/*******线程类2构造函数*******/
MyThreadp2::MyThreadp2(MainWindow *gui)
{
    m_gui = gui;
    connect(this, SIGNAL(showInfo()), this, SLOT(slotUpdateButton()));
}

/*******线程类析构函数*******/
MyThreadp2::~MyThreadp2(){}

void MyThreadp2::run()
{
    //发出信号，此时调用槽函数
    while(1){
        emit showInfo();
        msleep(500);
    }
}

/*******按钮闪烁信号槽*******/
void MyThreadp2::slotUpdateButton(){
    m_gui->twinkle();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------------------------------------------------//
void MainWindow::createData(){
    if(killLastSignal)
        memset(Status,0,6*sizeof(char));
    killLastSignal = 1;
    for(int j = 0;j < 4;j++){
        for(int i = 0;i < 6;i++){
            yList[j][i].removeFirst();
            yList[j][i].append(mechineLoad[i][j]);
        }
    }
    for(int j = 0;j < 6;j++){
        for(int i = 0;i < 4;i++){
            for(int k = 0;k < 50;k++){
                points[j][i][k] = QPointF(k, yList[i][j][k]);
            }
            points[j][i][50] = QPointF(50, yList[i][j][49]);
            points[j][i][51] = QPointF(50, 0);
            points[j][i][52] = QPointF(0, 0);
            points[j][i][53] = QPointF(0, 100);
            points[j][i][54] = QPointF(50, 100);
            points[j][i][55] = QPointF(50, 0);
        }
    }

    ui->frame_14->update();
    ui->frame_16->update();
    ui->frame_18->update();
    ui->frame_20->update();
    ui->frame_22->update();
    ui->frame_24->update();
}

/*
 * index:计算节点序号
 * choose：CPU，内存，网络，IO选择
 */
void MainWindow::paintOnMainWindow(QWidget *w,int index,int choose)
{
    static const QColor QMatrix[4] = {       //不透明画笔
        QColor(120,0,0),
        QColor(0,120,0),
        QColor(0,0,120),
        QColor(120,120,0)
    };
    static const QColor QMatrixT[4] = {       //透明画笔
        QColor(120,0,0,100),
        QColor(0,120,0,100),
        QColor(0,0,120,100),
        QColor(120,120,0,100)
    };
    QPainter painter(w);
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setBrush(Qt::gray);
    pen.setWidthF(0);
    painter.setPen(pen);
    painter.setViewport(60,20,170,130);
    painter.setWindow(0,100,50,-100);
    painter.fillRect(0,100,50,-100,Qt::white);
    painter.drawLine(QPointF(0,20),QPointF(50,20));
    painter.drawLine(QPointF(0,40),QPointF(50,40));
    painter.drawLine(QPointF(0,60),QPointF(50,60));
    painter.drawLine(QPointF(0,80),QPointF(50,80));
    pen.setStyle(Qt::SolidLine);
    pen.setBrush(QMatrix[choose]);
    pen.setWidthF(0);
    painter.setPen(pen);
    for(int i = 1;i < yList[choose][index].count();i++){
        painter.drawLine(QPointF(xList[choose][index][i-1],yList[choose][index][i-1]),QPointF(xList[choose][index][i],yList[choose][index][i]));
    }

    //使用四个点绘制轮廓
    painter.drawPolygon(&points[index][choose][52], 4);

    //使用53个点填充多边形
    QBrush brush(QMatrixT[choose], Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawPolygon(points[index][choose], 53);
}

void MainWindow::clearOnMainWindow(QWidget *w)
{
    QPainter painter(w);
    QPen pen;
    pen.setStyle(Qt::DashDotDotLine);
    pen.setBrush(Qt::gray);
    pen.setWidthF(1);
    painter.setPen(pen);
    painter.setViewport(60,20,170,130);
    painter.setWindow(0,100,50,-100);
    painter.fillRect(0,100,50,-100,Qt::white);
    painter.drawLine(QPointF(0,20),QPointF(50,20));
    painter.drawLine(QPointF(0,40),QPointF(50,40));
    painter.drawLine(QPointF(0,60),QPointF(50,60));
    painter.drawLine(QPointF(0,80),QPointF(50,80));
}

bool MainWindow::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->frame_14)
    {
        if(e->type() == QEvent::Paint){
            if(Status[0] == '1')
                paintOnMainWindow(ui->frame_14, 0, showChoose[0]);
            else
                clearOnMainWindow(ui->frame_14);
        }
    }
    if(watched == ui->frame_16)
    {
        if(e->type() == QEvent::Paint){
            if(Status[1] == '1')
                paintOnMainWindow(ui->frame_16, 1, showChoose[1]);
            else
                clearOnMainWindow(ui->frame_16);
        }
    }
    if(watched == ui->frame_18)
    {
        if(e->type() == QEvent::Paint){
            if(Status[2] == '1')
                paintOnMainWindow(ui->frame_18, 2, showChoose[2]);
            else
                clearOnMainWindow(ui->frame_18);
        }
    }
    if(watched == ui->frame_20)
    {
        if(e->type() == QEvent::Paint){
            if(Status[3] == '1')
                paintOnMainWindow(ui->frame_20, 3, showChoose[3]);
            else
                clearOnMainWindow(ui->frame_20);
        }
    }
    if(watched == ui->frame_22)
    {
        if(e->type() == QEvent::Paint){
            if(Status[4] == '1')
                paintOnMainWindow(ui->frame_22, 4, showChoose[4]);
            else
                clearOnMainWindow(ui->frame_22);
        }
    }
    if(watched == ui->frame_24)
    {
        if(e->type() == QEvent::Paint){
            if(Status[5] == '1')
                paintOnMainWindow(ui->frame_24, 5, showChoose[5]);
            else
                clearOnMainWindow(ui->frame_24);
        }
    }
    return QWidget::eventFilter(watched,e);
}

//切换感知内容展示回调函数
//--------------------------------------------------------------------------------------------------------------------------------------------------//
void MainWindow::lineEditClicked() {
    showChoose[0] = 0;
}

void MainWindow::lineEdit2Clicked() {
    showChoose[0] = 1;
}

void MainWindow::lineEdit3Clicked() {
    showChoose[0] = 2;
}

void MainWindow::lineEdit4Clicked() {
    showChoose[0] = 3;
}

void MainWindow::lineEdit5Clicked() {
    showChoose[1] = 0;
}

void MainWindow::lineEdit6Clicked() {
    showChoose[1] = 1;
}

void MainWindow::lineEdit7Clicked() {
    showChoose[1] = 2;
}

void MainWindow::lineEdit8Clicked() {
    showChoose[1] = 3;
}

void MainWindow::lineEdit9Clicked() {
    showChoose[2] = 0;
}

void MainWindow::lineEdit10Clicked() {
    showChoose[2] = 1;
}

void MainWindow::lineEdit11Clicked() {
    showChoose[2] = 2;
}

void MainWindow::lineEdit12Clicked() {
    showChoose[2] = 3;
}

void MainWindow::lineEdit13Clicked() {
    showChoose[3] = 0;
}

void MainWindow::lineEdit14Clicked() {
    showChoose[3] = 1;
}

void MainWindow::lineEdit15Clicked() {
    showChoose[3] = 2;
}

void MainWindow::lineEdit16Clicked() {
    showChoose[3] = 3;
}

void MainWindow::lineEdit17Clicked() {
    showChoose[4] = 0;
}

void MainWindow::lineEdit18Clicked() {
    showChoose[4] = 1;
}

void MainWindow::lineEdit19Clicked() {
    showChoose[4] = 2;
}

void MainWindow::lineEdit20Clicked() {
    showChoose[4] = 3;
}

void MainWindow::lineEdit21Clicked() {
    showChoose[5] = 0;
}

void MainWindow::lineEdit22Clicked() {
    showChoose[5] = 1;
}

void MainWindow::lineEdit23Clicked() {
    showChoose[5] = 2;
}

void MainWindow::lineEdit24Clicked() {
    showChoose[5] = 3;
}

failureDialog* MainWindow::failureInject(int index){
    clickedID = index;
    if(memoryFailure[index][0] < 20){
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        addrFailure[index][memoryFailure[index][0]] = 0x40003000 + qrand() * 16;
        memoryFailure[index][0]++;
        memoryFailure[index][1]++;
        memoryFailure[index][2]++;
    }

    failureDialog* p = new failureDialog();
    p->move(260*(0.32+index),420);
    p->addCloseButton();
    p->show();

    return p;
}

void MainWindow::on_pushButton_clicked()
{
    if(failureDialogArray[0] != NULL)
        failureDialogArray[0]->close();
    failureDialogArray[0] = failureInject(0);
}

void MainWindow::on_pushButton_4_clicked()
{
    if(failureDialogArray[1] != NULL)
        failureDialogArray[1]->close();
    failureDialogArray[1] = failureInject(1);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(failureDialogArray[2] != NULL)
        failureDialogArray[2]->close();
    failureDialogArray[2] = failureInject(2);
}

void MainWindow::on_pushButton_8_clicked()
{
    if(failureDialogArray[3] != NULL)
        failureDialogArray[3]->close();
    failureDialogArray[3] = failureInject(3);
}

void MainWindow::on_pushButton_10_clicked()
{
    if(failureDialogArray[4] != NULL)
        failureDialogArray[4]->close();
    failureDialogArray[4] = failureInject(4);
}

void MainWindow::on_pushButton_12_clicked()
{
    if(failureDialogArray[5] != NULL)
        failureDialogArray[5]->close();
    failureDialogArray[5] = failureInject(5);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(taskPriority[0] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[0] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(taskPriority[1] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[1] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(taskPriority[2] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[2] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    if(taskPriority[3] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[3] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    if(taskPriority[4] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[4] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_11_clicked()
{
    if(taskPriority[5] < '3'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[5] += 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 300 + 500;
        QString str = "注入高优先级任务，中断响应时间:" + QString::number(tt) + "ns.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void MainWindow::on_pushButton_13_clicked()
{
    if(taskPriority[0] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[0] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}

void MainWindow::on_pushButton_14_clicked()
{
    if(taskPriority[1] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[1] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}

void MainWindow::on_pushButton_15_clicked()
{
    if(taskPriority[2] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[2] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}

void MainWindow::on_pushButton_16_clicked()
{
    if(taskPriority[3] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[3] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}

void MainWindow::on_pushButton_17_clicked()
{
    if(taskPriority[4] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[4] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}

void MainWindow::on_pushButton_18_clicked()
{
    if(taskPriority[5] > '0'){
        QUdpSocket qus;
        char msg[9] = {0};
        msg[0] = 0x55;
        msg[1] = 0x55;
        taskPriority[5] -= 1;
        memcpy(&msg[2],taskPriority,6);
        qus.writeDatagram(msg, QHostAddress(IPpool[serverMark]), (quint16)(54460 + (serverMark << 2)));
    }
}


//打开任务故障注入窗口
void MainWindow::on_action_triggered()
{
    pInject = new InjectDialog();
    pInject->move(w->width(),0);
    pInject->show();
}

//打开控制台信息窗口
void MainWindow::on_action_3_triggered()
{
    //chinese support
    int id = QFontDatabase::addApplicationFont("../DroidSansFallback.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies (id).at(0);
    QFont font(msyh,10);
    qDebug()<<msyh<<endl;
    font.setPointSize(12);

    pInfo = new InfoDialog();
    pInfo->setFont(font);
    pInfo->move(w->width(),520);
    pInfo->addCloseButton();
    pInfo->show();

    //设置菜单不可交互
    ui->action_3->setEnabled(false);
}

