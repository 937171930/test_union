#include "failuredialog.h"
#include "ui_failuredialog.h"

#include <QToolButton>
#include <QPixmap>
#include <QThread>
#include <QStyle>

extern int memoryFailure[6][4];         //注入内存故障记录数组
extern unsigned int addrFailure[6][20]; //内存故障地址记录数组
extern int clickedID;

failureDialog::failureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::failureDialog)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);    //无边框模式

    ui->setupUi(this);

    ui->textEdit->append("正在请求获取当前计算节点的PC、nPC地址...");
    ui->textEdit->append("正在当前nPC位置模拟设置SEU错误...");
    ui->textEdit->append("模拟SEU错误设置成功，返回内存故障地址:0x" + QString::number(addrFailure[clickedID][memoryFailure[clickedID][0]-1],16) + ".");
    ui->textEdit->append("检测到内存故障.");
    ui->textEdit->append("正在进行内存故障隔离...");
    ui->textEdit->append("内存故障隔离成功，当前总隔离数量：" +  QString::number(memoryFailure[clickedID][0]) + ".");
    ui->textEdit->append("内存故障列表：");
    for(int i = 0;i < memoryFailure[clickedID][0];i++){
        ui->textEdit->append("  0x" + QString::number(addrFailure[clickedID][i],16));
    }
}

failureDialog::~failureDialog()
{
    delete ui;
}

/*******关闭按钮信号槽函数*******/
void failureDialog::windowclosed(){
    this->close();
}

/*******UI界面添加关闭按钮*******/
void failureDialog::addCloseButton(){
    int wide = width();                                                         //获取界面的宽度
    QToolButton *closeButton= new QToolButton(this);    //构建关闭按钮
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton); //获取关闭按钮图标
    closeButton->setIcon(closePix);                         //设置关闭按钮图标
    closeButton->setGeometry(wide-21,3,18,18);              //设置关闭按钮在界面的位置
    closeButton->setToolTip(tr("关闭"));                     //设置鼠标移至按钮上的提示信息
    closeButton->setStyleSheet("background-color:red;");    //设置关闭按钮的样式
    connect(closeButton, SIGNAL(clicked()), this, SLOT(windowclosed()) );
}
