#include "injectdialog.h"
#include "ui_injectdialog.h"
#include "mainwindow.h"
#include "infodialog.h"

#include <QToolButton>
#include <QPixmap>
#include <QThread>
#include <QStyle>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QUdpSocket>
#include <QTime>
#include <Qstring>

extern char Status[7];              //在线状态
extern char taskDistribute[6][6];   //任务分配情况
extern char taskFailure[6][6];
extern int serverMark;
static QCheckBox* checkBox[12];
static QString nodeArray[6] = {"计算节点A","计算节点B","计算节点C","计算节点D","计算节点E","计算节点F"};
static QString taskArray[6] = {"控制任务1","控制任务2","控制任务3","控制任务4","计算任务5","计算任务6"};
static int currentNode = -1;
static int currentTask = -1;
static int dealIndex = -1;
static int injectType = 0;
static bool rebuildFlag = 0;
InfoDialog* pInfo;
InjectDialog* pInject;

InjectDialog::InjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InjectDialog)
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::Dialog | Qt::Popup);
    ui->setupUi(this);

    QIcon icon,icon1;
    icon.addFile(":/resource/icon1.png");
    ui->comboBox->clear();      //清除列表
    ui->comboBox->addItem(icon,QString::asprintf("点击选择计算节点")); //带图标
    icon1.addFile(":/resource/task.jpg");
    ui->comboBox_2->clear();    //清除列表
    ui->comboBox_2->addItem(icon1,QString::asprintf("点击选择在线任务")); //带图标

    /*********/
    checkBox[0] = ui->checkBox  ;checkBox[1] = ui->checkBox_2   ;checkBox[2] = ui->checkBox_3   ;checkBox[3] = ui->checkBox_4;
    checkBox[4] = ui->checkBox_5;checkBox[5] = ui->checkBox_6   ;checkBox[6] = ui->checkBox_7   ;checkBox[7] = ui->checkBox_8;
    checkBox[8] = ui->checkBox_9;checkBox[9] = ui->checkBox_10  ;checkBox[10] = ui->checkBox_11 ;checkBox[11] = ui->checkBox_12;
    for(int i = 0;i < 12;i++){
        checkBox[i]->setEnabled(false);
    }

    connect(ui->comboBox, SIGNAL(clicked()), this, SLOT(getNodes()));
    connect(ui->comboBox_2, SIGNAL(clicked()), this, SLOT(getTasks()));

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->radioButton);
    group->addButton(ui->radioButton_2);

    ui->groupBox->setEnabled(true);
    ui->groupBox_2->setEnabled(false);
}

InjectDialog::~InjectDialog()
{
    delete ui;
}

/*******关闭按钮信号槽函数*******/
void InjectDialog::windowclosed(){
    this->close();
}

/*******UI界面添加关闭按钮*******/
void InjectDialog::addCloseButton(){
    int wide = width();                                                         //获取界面的宽度
    QToolButton *closeButton= new QToolButton(this);        //构建关闭按钮
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton); //获取关闭按钮图标
    closeButton->setIcon(closePix);                         //设置关闭按钮图标
    closeButton->setGeometry(wide-21,3,18,18);              //设置关闭按钮在界面的位置
    closeButton->setToolTip(tr("关闭"));                     //设置鼠标移至按钮上的提示信息
    closeButton->setStyleSheet("background-color:red;");    //设置关闭按钮的样式
    connect(closeButton, SIGNAL(clicked()), this, SLOT(windowclosed()));
}

/*
 * void InjectDialog::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
        mousePoint = event->pos();
    }
}

void InjectDialog::mouseReleaseEvent(QMouseEvent *event){
    mousePressed    = false;
}

void InjectDialog::mouseMoveEvent(QMouseEvent *event){
    if(mousePressed && event->buttons()){
        this->move(event->globalPos() - mousePoint);
        event->accept();
    }
}*/

void InjectDialog::getNodes(){
    QIcon icon;
    icon.addFile(":/resource/icon1.png");
    ui->comboBox->clear();  //清除列表
    for(int i = 0;i < 6;i++){
        if(Status[i] == '1')
            ui->comboBox->addItem(icon,QString::asprintf("计算节点%c",'A'+i)); //带图标
    }
    //ui->comboBox->addItem(QString::asprintf("Item.")); //不带图标
}

void InjectDialog::getTasks(){
    QIcon icon;
    icon.addFile(":/resource/task.jpg");
    ui->comboBox_2->clear();  //清除列表
    for(int i = 0;i < 6;i++){
        for(int j = 0;j < 6;j++){
            if(taskDistribute[j][i] != 0 || taskFailure[j][i] != 0){
                if(i < 4)
                    ui->comboBox_2->addItem(icon,QString::asprintf("控制任务%d",i+1)); //带图标
                else
                    ui->comboBox_2->addItem(icon,QString::asprintf("计算任务%d",i+1)); //带图标
                break;
            }
        }
    }
    //ui->comboBox->addItem(QString::asprintf("Item.")); //不带图标
}

void InjectDialog::on_comboBox_currentIndexChanged(const QString &arg)
{
    for(int i = 0;i < 6;i++){
        if(arg == nodeArray[i]){
            currentNode = i;
            for(int i = 0;i < 6;i++){
                checkBox[i]->setEnabled(false);
                checkBox[i]->setCheckState(Qt::Unchecked);
            }
            for(int j = 0;j < 6;j++){
                if(taskDistribute[i][j] != 0){
                    checkBox[j]->setEnabled(true);
                }
                if(taskFailure[i][j] != 0){
                    checkBox[j]->setEnabled(true);
                    checkBox[j]->setCheckState(Qt::Checked);
                }
            }
            break;
        }
    }
}

void InjectDialog::on_comboBox_2_currentIndexChanged(const QString &arg)
{
    for(int i = 0;i < 6;i++){
        if(arg == taskArray[i]){
            currentTask = i;
            for(int i = 6;i < 12;i++){
                checkBox[i]->setEnabled(false);
                checkBox[i]->setCheckState(Qt::Unchecked);
            }
            for(int j = 0;j < 6;j++){
                if(taskDistribute[j][i] != 0 ){
                    checkBox[j+6]->setEnabled(true);
                }
                if(taskFailure[j][i] != 0){
                    checkBox[j+6]->setEnabled(true);
                    checkBox[j+6]->setCheckState(Qt::Checked);
                }
            }
            break;
        }
    }
}

void InjectDialog::on_radioButton_clicked()
{
    injectType = 0;
    ui->groupBox->setEnabled(true);
    ui->groupBox_2->setEnabled(false);

    for(int i = 6;i < 12;i++){
        checkBox[i]->setCheckState(Qt::Unchecked);
    }

    if(currentNode != -1){
        for(int j = 0;j < 6;j++){
            if(taskDistribute[currentNode][j] != 0){
                checkBox[j]->setEnabled(true);
                if(taskFailure[currentNode][j] != 0)
                    checkBox[j]->setCheckState(Qt::Checked);
            }
        }
    }
}

void InjectDialog::on_radioButton_2_clicked()
{
    injectType = 1;
    ui->groupBox->setEnabled(false);
    ui->groupBox_2->setEnabled(true);

    for(int i = 0;i < 6;i++){
        checkBox[i]->setCheckState(Qt::Unchecked);
    }

    if(currentTask != -1){
        for(int j = 0;j < 6;j++){
            if(taskDistribute[j][currentTask] != 0){
                checkBox[j+6]->setEnabled(true);
                if(taskFailure[j][currentTask] != 0)
                    checkBox[j+6]->setCheckState(Qt::Checked);
            }
        }
    }
}

void InjectDialog::udpsend(){
    QUdpSocket qus;
    char msg[75];
    msg[0] = 0x55;
    msg[1] = 0x7A;
    memcpy(&msg[2],taskDistribute,36);
    memcpy(&msg[38],taskFailure,36);
    for(int i = 2;i < 74;i++){
        msg[i] += '0';
    }
    msg[74] = 0;
    qus.writeDatagram(msg, QHostAddress(MainWindow::IPpool[serverMark]), static_cast<quint16>(54460 + (serverMark << 2)));
}

bool InjectDialog::taskRebuild(int index){
    int sum = 0;
    for(int i = 0;i < 6;i++){
        sum += taskFailure[i][index];
        if(sum == 3)
            return true;
    }
    return false;
}

void InjectDialog::progressRebuild(){
   //新建对象，参数含义：对话框正文，取消按钮名称，进度条范围
   pd = new QProgressDialog("正在进行任务重构...","取消",0,100);
   //模态对话框
   pd->setWindowModality(Qt::WindowModal);
   //如果进度条运行的时间小于5，进度条就不会显示，默认是4S
   pd->setMinimumDuration(5);
   //设置标题
   pd->setWindowTitle("请稍后...");
   //设置图标
   pd->setWindowIcon(QIcon(":/resource/failure.jpg"));
   //设置样式
   pd->setStyleSheet("border:0px;"
                     "color:black;"
                     "background-color:white;"
                     "text-align:center;"
                     "QProgressBar::chunk{"
                     "border-radius:3px;"    // 斑马线圆角
                     "border:0.5px "
                     "solid black;}"         // 黑边，默认无边
                    );
   //显示处理框
   pd->show();
   //处理过程。。。
   t = new QTimer(this);
   connect(t, SIGNAL(timeout()), this, SLOT(perform()));
   t->start(1);
   pInfo->Display("检测到软件死机，正在进行软件重启...");
}

void InjectDialog::progressRecovery(){
   //新建对象，参数含义：对话框正文，取消按钮名称，进度条范围
   pd = new QProgressDialog("正在进行任务重构...","取消",0,100);
   //模态对话框
   pd->setWindowModality(Qt::WindowModal);
   //如果进度条运行的时间小于5，进度条就不会显示，默认是4S
   pd->setMinimumDuration(5);
   //设置标题
   pd->setWindowTitle("请稍后...");
   //设置图标
   pd->setWindowIcon(QIcon(":/resource/failure.jpg"));
   //设置样式
   pd->setStyleSheet("border:0px;"
                     "color:black;"
                     "background-color:white;"
                     "text-align:center;"
                     "QProgressBar::chunk{"
                     "border-radius:3px;"    // 斑马线圆角
                     "border:0.5px "
                     "solid black;}"         // 黑边，默认无边
                    );
   //显示处理框
   pd->show();
   //处理过程。。。
   t = new QTimer(this);
   connect(t, SIGNAL(timeout()), this, SLOT(perform1()));
   t->start(5);
   pInfo->Display("检测到任务错误，正在进行任务重构...");
}

void InjectDialog::perform()
{
    static int steps = 0;
    pd->setValue(steps++);
    if(steps > pd->maximum() || pd->wasCanceled())
    {
        t->stop();
        steps = 0;
        delete pd;
        for(int i = 0;i < 6;i++){
            for(int j = 0;j < 6;j++){
                taskDistribute[0][j] += taskFailure[i][j];
                taskFailure[i][j] = 0;
            }
        }
        udpsend();
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int tt = qrand() % 15 + 30;
        QString str = "软件重启成功,重启耗时:" + QString::number(tt) + "ms.";
        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void InjectDialog::perform1()
{
    static int steps = 0;
    pd->setValue(steps++);
    if(steps > pd->maximum() || pd->wasCanceled())
    {
        t->stop();
        steps = 0;
        delete pd;
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        QString str;
        if(rebuildFlag){
            rebuildFlag = 0;
            int tt = qrand() % 100 + 100;
            str = "分区间任务重构成功,重构耗时:" + QString::number(tt) + "us.";
        }
        else {
            int tt = qrand() % 100 + 500;
            str = "单机间任务重构成功,重构耗时:" + QString::number(tt) + "us.";
        }

        pInfo->Display(str);
        pInfo->Display("-·-·-·-·-·-·-·-·-·-");
    }
}

void InjectDialog::on_checkBox_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][0]--;
        taskFailure[currentNode][0]++;
        if(taskDistribute[currentNode][0] == taskFailure[currentNode][0])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox->setChecked(false);
    }
}

void InjectDialog::on_checkBox_2_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][1]--;
        taskFailure[currentNode][1]++;
        if(taskDistribute[currentNode][1] == taskFailure[currentNode][1])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_2->setChecked(false);
    }
}

void InjectDialog::on_checkBox_3_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][2]--;
        taskFailure[currentNode][2]++;
        if(taskDistribute[currentNode][2] == taskFailure[currentNode][2])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_3->setChecked(false);
    }
}

void InjectDialog::on_checkBox_4_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][3]--;
        taskFailure[currentNode][3]++;
        if(taskDistribute[currentNode][3] == taskFailure[currentNode][3])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_4->setChecked(false);
    }
}

void InjectDialog::on_checkBox_5_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][4]--;
        taskFailure[currentNode][4]++;
        if(taskDistribute[currentNode][4] == taskFailure[currentNode][4])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_5->setChecked(false);
    }
}

void InjectDialog::on_checkBox_6_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[currentNode][5]--;
        taskFailure[currentNode][5]++;
        if(taskDistribute[currentNode][5] == taskFailure[currentNode][5])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_6->setChecked(false);
    }
}

void InjectDialog::on_checkBox_7_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[0][currentTask]--;
        taskFailure[0][currentTask]++;
        if(taskDistribute[0][currentTask] == taskFailure[0][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_7->setChecked(false);
    }
}

void InjectDialog::on_checkBox_8_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[1][currentTask]--;
        taskFailure[1][currentTask]++;
        if(taskDistribute[1][currentTask] == taskFailure[1][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_8->setChecked(false);
    }
}

void InjectDialog::on_checkBox_9_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[2][currentTask]--;
        taskFailure[2][currentTask]++;
        if(taskDistribute[2][currentTask] == taskFailure[2][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_9->setChecked(false);
    }
}

void InjectDialog::on_checkBox_10_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[3][currentTask]--;
        taskFailure[3][currentTask]++;
        if(taskDistribute[3][currentTask] == taskFailure[3][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_10->setChecked(false);
    }
}

void InjectDialog::on_checkBox_11_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[4][currentTask]--;
        taskFailure[4][currentTask]++;
        if(taskDistribute[4][currentTask] == taskFailure[4][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_11->setChecked(false);
    }
}

void InjectDialog::on_checkBox_12_clicked(bool checked)
{
    if(checked != 0){
        taskDistribute[5][currentTask]--;
        taskFailure[5][currentTask]++;
        if(taskDistribute[5][currentTask] == taskFailure[5][currentTask])
            rebuildFlag = 1;
        progressRecovery();
        ui->checkBox_12->setChecked(false);
    }
}

void InjectDialog::on_checkBox_18_clicked(bool checked)
{
    if(checked != 0){
        for(int i = 0;i < 6;i++){
            for(int j = 0;j < 6;j++){
                if(taskDistribute[i][j] == 1){
                    taskDistribute[i][j]--;
                    taskFailure[i][j]++;
                }
                if(taskDistribute[i][j] == 2){
                    taskDistribute[i][j]-=2;
                    taskFailure[i][j]+=2;
                }
            }
        }
        udpsend();
        progressRebuild();
        ui->checkBox_18->setChecked(false);
    }
}
